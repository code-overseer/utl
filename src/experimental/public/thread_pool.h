#pragma once
#include "latch.h"
#include "semaphore.h"
#include "utl_config.h"

#include <memory>
#include <stdexcept>
#include <thread>
#include <tuple>
#include <type_traits>
#include <utility>
// todo intrusive_ptr
// todo atomic_reference_counter

namespace utl::experimental {
using size_t = decltype(sizeof(0));

class job_handle;
class jthread : public std::thread {
public:
    using thread::thread;

    ~jthread() {
        if (this->joinable()) {
            this->join();
        }
    }
};

inline constexpr size_t dynamic_extent = (size_t)-1;

template <size_t N = dynamic_extent>
class thread_pool;

namespace details {
namespace thread_pool {

class completion_interface {
public:
    struct deleter {
        void operator()(completion_interface* ptr) const noexcept { ptr->~completion_interface(); }
    };

    virtual bool wait_for(std::chrono::microseconds) = 0;
    virtual void wait() = 0;
    virtual bool try_wait() const = 0;

    template <typename R, typename P>
    bool wait_for(std::chrono::duration<R, P> d) {
        return this->wait_for(std::chrono::duration_cast<std::chrono::microseconds>(d));
    }

protected:
    completion_interface(completion_interface const&) = delete;
    completion_interface& operator=(completion_interface const&) = delete;
    completion_interface() = default;
    virtual ~completion_interface() = default;
};

class execution_interface {
public:
    virtual size_t count() const noexcept = 0;
    virtual void execute(size_t thread_idx, size_t job_idx) final = 0;

protected:
    execution_interface(execution_interface const&) = delete;
    execution_interface& operator=(execution_interface const&) = delete;
    execution_interface() = default;
    virtual ~execution_interface() = default;
};

template <typename... Fs>
class multi_execution : public completion_interface, public execution_interface {

public:
    template <typename... Args>
    multi_execution(Args&&... args) : executions(std::forward<Args>(args)...) {}

    ~multi_execution() final = default;

    size_t count() const noexcept final { return sizeof...(Fs); }

    void execute(size_t thread_idx, size_t job_idx) final {
        execute(thread_idx, job_idx, index_sequence_for<Fs...>{});
    }

private:
    bool wait_for(std::chrono::microseconds d) final {
        return latch.wait_for(d) && latch.try_wait();
    }

    void wait() final {
        latch.wait();
        std::ignore = try_wait();
    }

    bool try_wait() const final {
        if (!latch.try_wait()) {
            return false;
        }

        propogate_exceptions();
        return true;
    }

    template <size_t... Is>
    void execute(size_t, size_t job_idx, std::index_sequence<Is...>) {
        static constexpr auto vtable[] = {&multi_execution::execute<Is>...};
        (this->*vtable[job_idx])();
    }

    template <size_t I>
    void execute() {
        UTL_ON_SCOPE_EXIT {
            latch.count_down();
        };
        try {
            std::get<I>(executions)();
        } catch (std::exception const& e) {
            std::get<I>(exceptions)(std::current_exception());
        } catch (...) {
            std::terminate();
        }
    }
    template <size_t... Is>
    void propogate_exceptions(std::index_sequence<Is...>) const {
        std::atomic_thread_fence(std::memory_order_acquire);
        if ((... || std::get<I>(executions))) {
            throw std::runtime_error("Uncaught exception");
        }
    }

    void propogate_exceptions() const { propogate_exceptions(index_sequence_for<Fs...>{}); }

    std::tuple<std::exception_ptr> exceptions;
    std::tuple<Fs...> executions;
    latch latch;
};

template <typename F>
class single_execution<F> : public completion_interface, public execution_interface {
public:
    template <typename A>
    job_execution(A&& arg) : execution(std::forward<A>(arg))
                           , latch(1) {}
    ~job_execution() final = default;

    constexpr size_t count() const noexcept { return 1; }

    void execute(size_t, size_t) { execute(); }

private:
    bool wait_for(std::chrono::microseconds d) final {
        return latch.wait_for(d) && latch.try_wait();
    }

    void wait() final {
        latch.wait();
        if (exception) {
            rethrow_exception(exception);
        }
    }

    bool try_wait() const final {
        if (!latch.try_wait()) {
            return true;
        }

        if (exception) {
            rethrow_exception(exception);
        }

        return true;
    }

    void execute() {
        UTL_ON_SCOPE_EXIT {
            latch.count_down();
        };
        try {
            execution();
        } catch (std::exception const& e) {
            exception = std::current_exception();
        } catch (...) {
            std::terminate();
        }
    }

    std::exception_ptr exception;
    F execution;
    latch latch;
};

template <typename F>
class parallel_execution : public completion_interface, public execution_interface {
public:
    template <typename A>
    parallel_execution(A&& arg, size_t count) : execution(std::forward<A>(arg))
                                              , latch(count) {}
    ~parallel_execution() final = default;

    size_t count() const noexcept { return count; }

    void execute(size_t thread_idx, size_t job_idx) {
        UTL_ON_SCOPE_EXIT {
            latch.count_down();
        };
        try {
            execution(thread_idx, job_idx);
        } catch (std::exception const& e) {
            exceptions_thrown.store(true, std::memory_order_release);
        } catch (...) {
            std::terminate();
        }
    }

private:
    bool wait_for(std::chrono::microseconds d) final {
        return latch.wait_for(d) && latch.try_wait();
    }

    void wait() final {
        latch.wait();
        if (exceptions_thrown.load(std::memory_order_acquire)) {
            // TODO
            throw std::runtime_error("Uncaught exception");
        }
    }

    bool try_wait() const final {
        if (!latch.try_wait()) {
            return true;
        }

        if (exceptions_thrown.load(std::memory_order_acquire)) {
            // TODO
            throw std::runtime_error("Uncaught exception");
        }

        return true;
    }

    // TODO handle exception properly
    std::atomic<bool> exceptions_thrown = false;
    size_t count;
    F execution;
    latch latch;
};

template <size_t N>
class job_collection : public completion_interface {

public:
    template <typename... J, std::enable_if_t<sizeof...(J) == N, int> = 0>
    job_collection(J const&... header_ptr) : jobs_{header_ptr...} {}

private:
    bool wait_for(std::chrono::microseconds d) final {
        for (auto const& ptr : jobs_) {
            auto const begin = std::chrono::high_resolution_clock::now();
            if (!ptr->wait_for(d)) {
                return;
            }

            using clock_duration = decltype(std::chrono::high_resolution_clock::now() - begin);
            auto const min_val = std::min<std::common_type_t<clock_duration, decltype(d)>>(
                (std::chrono::high_resolution_clock::now() - begin), d);

            d -= min_val;
        }
    }
    void wait() final {
        for (auto const& ptr : jobs_) {
            ptr->wait();
        }
    }

    bool try_wait() const final {
        return std::all_of(
            begin(jobs_), end(jobs_), [](auto const& ptr) { return ptr->try_wait(); });
    }

    std::array<utl::intrusive_ptr<job_header>> jobs_;
};

class pool_handle : private utl::atomic_reference_count<pool_handle> {
    template <size_t N = dynamic_extent>
    friend class ::thread_pool;

    static constexpr struct construct_tag_t {
    } construct_tag = {};

public:
    virtual ~pool_handle() = default;

    template <typename Executor>
    void schedule_execution(Executor& execution) {
        auto const count = execution.count();
    }

protected:
    pool_handle(size_t count) : threads_(new jthread[count]), size_(count) {}

private:
    std::unique_ptr<jthread[]> threads_;
    size_t size_;
};

class job_header : private utl::atomic_reference_count<job_header> {
public:
    job_header() = default;

protected:
    void set_completion(completion_interface* interface) { completion_.reset(interface); }

private:
    utl::intrusive_ptr<job_header> dependency_;
    std::unique_ptr<completion_interface, completion_interface::deleter> completion_;
};

template <typename T>
class job : public job_header, public T {
    static_assert(std::is_base_of_v<completion_interface, T>, "Invalid job");
    static_assert(std::is_base_of_v<execution_interface, T>, "Invalid job");
    using execution_base = T;

public:
    template <typename... Args>
    job(pool_handle& pool, Args&&... args) : execution_base{std::forward<Args>(args)...} {
        job_header::set_completion(this);
    }
};

} // namespace thread_pool
} // namespace details

class job_handle {
    using pool_type = details::thread_pool::pool_handle;
    using header_type = details::thread_pool::job_header;
    using header_ptr = utl::intrusive_ptr<header_type>;

    template <typename... Fs>
    static auto create_handle(intrusive_ptr<pool_type> pool, Fs&&... f);

    template <typename... Headers,
        std::enable_if_t<(... && std::is_same_v<decay_t<Headers>, header_ptr>), bool> = true>
    static auto create_collection_header(intrusive_ptr<pool_type> pool, Headers&&... headers) {}

    template <typename... Headers,
        std::enable_if_t<(... && std::is_same_v<decay_t<Headers>, header_ptr>), bool> = true>
    job_handle(intrusive_ptr<pool_type> pool, Headers&&... headers)
        : pool_(std::move(pool))
        , job_(create_collection_header(pool_, std::forward<Headers>(headers)...)) {}

public:
    template <typename... Jobs,
        std::enable_if_t<
            ((sizeof...(Jobs) > 1) && ... && std::is_same_v<decay_t<Jobs>, job_handle>), bool> =
            true>
    static job_handle combine(Jobs&&... jobs) {
        Jobs const* array[]{&jobs...};
        auto const& pool = (*array)->pool_;
        if (!std::all_of(
                array, array + sizeof...(Jobs), [&](auto ptr) { return ptr->pool_ == pool; })) {
            throw std::runtime_error("[job_handle] only jobs from the same pool can be combined");
        }

        return job_handle(pool, std::forward<Jobs>(jobs).job_...);
    }

public:
    job_handle() = default;
    job_handle(job_handle const&) = default;
    job_handle& operator=(job_handle const&) = default;
    job_handle(job_handle&&) noexcept = default;
    job_handle& operator=(job_handle&&) noexcept = default;

    template <typename... Fs>
    explicit job_handle(intrusive_ptr<pool_type> pool, Fs&&... f)
        : job_(create_handle(std::move(pool), std::forward<Fs>(f)...)) {}

    void wait() {
        if (job_) {
            job_->wait();
        }
    }

    bool try_wait() const { return !job_ || job_->try_wait(); }

    ~job_handle() { wait(); }

private:
    utl::intrusive_ptr<pool_type> pool_;
    utl::intrusive_ptr<header_type> job_;
};

class thread_pool {
    template <typename... Fs>
    using multi_handle_t =
        std::std::enable_if_t<((sizeof...(Fs) > 1) && ... && std::is_invocable_v<Fs>), job_handle>;
    template <typename... Fs>
    using multi_result_t =
        std::std::enable_if_t<(sizeof...(Fs) > 1), std::tuple<std::invoke_result_t<Fs>...>>;
    template <typename F>
    using single_handle_t = std::enable_if_t<std::is_invocable_v<F>, job_handle>;

    using pool_handle_t = details::thread_pool::pool_handle;

    template <typename F>
    using parallel_handle_t = std::enable_if_t<std::is_invocable_v<F, size_t, size_t>, job_handle>;
    template <typename F>
    using parallel_t = std::enable_if_t<std::is_invocable_v<F, size_t, size_t>, void>;

    using init_arg_t = std::conditional_t<N == dynamic_extent, size_t, invalid_t>;

public:
    constexpr explicit thread_pool(size_t n) noexcept : size_(n) {}
    constexpr size_t size() const noexcept { return size_; }

    template <typename F>
    single_handle_t<F> schedule(F&& execution) {
        return job_handle(pool_handle(), std::forward<F>(execution));
    }

    template <typename... Fs>
    multi_handle_t<Fs...> schedule(Fs&&... executions) {}

    template <typename F>
    parallel_handle_t<F> schedule(size_t count, F&& executions);

    template <typename F>
    single_handle_t<F> schedule(job_handle dependency, F&& execution);

    template <typename... Fs>
    multi_handle_t<Fs...> schedule(job_handle dependency, Fs&&... executions);

    template <typename F>
    parallel_handle_t<F> schedule(job_handle dependency, size_t count, F&& executions);

    template <typename F>
    std::invoke_result_t<F> execute(F&& execution) {
        return std::invoke(std::forward<F>(execution));
    }

    template <typename... Fs>
    multi_result_t<Fs...> execute(Fs&&... executions) {
        return multi_execute(index_sequence_for<Fs...>{}, std::forward<Fs>(executions)...);
    }

    template <typename F>
    parallel_t<F> execute(size_t count, F&& execution) {
        schedule(count, executions).wait();
    }

    template <typename F>
    std::invoke_result_t<F> execute(job_handle dependency, F&& execution) {
        return dependency.wait(), std::invoke(std::forward<F>(execution));
    }

    template <typename... Fs>
    multi_result_t<Fs...> execute(job_handle dependency, Fs&&... executions) {
        dependency.wait();
        return multi_execute(index_sequence_for<Fs...>{}, std::forward<Fs>(executions)...);
    }

    template <typename F>
    parallel_t<F> execute(job_handle dependency, size_t count, F&& execution) {
        schedule(dependency, count, executions).wait();
    }

private:
    template <typename T, typename U, typename F, size_t I>
    static auto tuple_executor(T& t, U& exe) {
        return [&t, &exe]() { std::get<I>(t).emplace(std::forward<F>(std::get<I>(exe))()); };
    }

    template <typename... Ts, size_t... Is>
    static std::tuple<Ts...> optional_to_tuple(
        std::tuple<std::optional<Ts>...>&& t, std::index_sequence<Is...>) {
        return std::tuple<Ts...>{(*std::get<Is...>(std::move(t)))...};
    }

    template <typename... Fs, size_t... Is>
    multi_result_t<Fs...> multi_execute(std::index_sequence<Is...>, Fs&&... executions) {
        std::tuple<std::optional<invoke_result_t<Fs>>...> t;
        auto exe = std::forward_as_tuple(std::forward<Fs>(executions)...);

        schedule(tuple_executor<Fs, Is>(t, exe)...).wait();

        return optional_to_tuple(std::move(t), index_sequence<Is...>{});
    }

    intrusive_ptr<pool_handle_t> const& pool_handle() {
        if (!pool_handle_) {
            pool_handle_ = utl::make_intrusive_ptr<pool_handle_t>(size());
        }

        return pool_handle_;
    }

    intrusive_ptr<pool_handle_t> pool_handle_;
    size_t size_;
};

} // namespace utl::experimental
