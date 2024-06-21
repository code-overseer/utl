

namespace utl::experimental {
namespace details::thread_pool {
class group_tag;
class dependency_tag;

template <typename T, typename Tag>
class job_node : private UTL_SCOPE intrusive::bidirectional_node<job_node<T, Tag>> {
public:
    using bidirectional_node::linked;

    friend constexpr T* node_cast(job_node* node) {
        static_assert(UTL_SCOPE is_base_of<job_node, T>::value);
        return (T*)node;
    }

    friend constexpr T& node_cast(job_node& node) {
        static_assert(UTL_SCOPE is_base_of<job_node, T>::value);
        return (T&)(node);
    }
};

template <typename To, typename From, typename T>
job_node<T, To>* node_cast(job_node<T, From>* other) noexcept {
    static_assert(UTL_SCOPE is_base_of<job_node<T, To>, T>::value);
    return (job_node<T, To>*)node_cast(other);
}

template <typename To, typename From, typename T>
job_node<To>& node_cast(job_node<From>& other) noexcept {
    static_assert(UTL_SCOPE is_base_of<job_node<T, To>, T>::value);
    return (job_node<T, To>&)node_cast(other);
}

template <typename To, typename T>
job_node<T, To>* node_cast(T* other) noexcept {
    static_assert(UTL_SCOPE is_base_of<job_node<T, To>, T>::value);
    return (job_node<T, To>*)other;
}

template <typename To, typename From>
job_node<T, To>& node_cast(job_executable& other) noexcept {
    static_assert(UTL_SCOPE is_base_of<job_node<T, To>, T>::value);
    return (job_node<T, To>&)other;
}

template <typename T>
using group_node = job_node<T, group_tag>;
template <typename T>
using dependency_node = job_node<T, dependency_tag>;

class job_executable : group_node<job_executable>, dependency_node<job_executable> {
public:
    virtual size_t size() const noexcept = 0;
    virtual void execute(size_t thread_idx, size_t job_idx) = 0;
    virtual ~job_executable() noexcept = default;
};

template <typename F>
class single_executor : public job_executable {
public:
    template <typename... Args, UTL_SCOPE enable_if_t<UTL_SCOPE is_constructible_v<F, Args...>>>
    single_executor(Args&&... args) noexcept(UTL_SCOPE is_nothrow_constructible_v<F, Args...>)
        : callable_(UTL_SCOPE forward<Args>(args)...) {}

private:
    UTL_CONSTEXPR_CXX20 size_t size() const noexcept final { return 1; }
    void execute(size_t thread_idx, size_t job_idx) final { callable_(); }
    UTL_ATTRIBUTE(NO_UNIQUE_ADDRESS) F callable_;
};

template <typename... Fs>
class multi_executor : public job_executable {
public:
    template <typename... Args, UTL_SCOPE enable_if_t<UTL_SCOPE is_constructible_v<F, Args...>>>
    multi_executor(Args&&... args) noexcept(UTL_SCOPE is_nothrow_constructible_v<F, Args...>)
        : callables_(UTL_SCOPE forward<Args>(args)...) {}

private:
    UTL_CONSTEXPR_CXX20 size_t size() const noexcept final { return sizeof...(Fs); }

    template <size_t I>
    void execute() {
        UTL_SCOPE get<I>(callables_)();
    }

    template <size_t... Is>
    void execute(size_t job_idx, UTL_SCOPE index_sequence<Is...>) {
        static_assert(
            UTL_SCOPE
                is_same_v<UTL_SCOPE index_sequence<Is...>, UTL_SCOPE index_sequence_for<Fs...>>,
            "");
        UTL_ASSERT(job_idx < sizeof...(Is));
        static constexpr auto vtable[] = {&multi_executor::execute<Is>...};
        (this->*vtable[job_idx])();
    }

    void execute(size_t, size_t job_idx) final {
        static constexpr UTL_SCOPE index_sequence_for<Fs...> sequence = {};
        execute(job_idx, sequence);
    }

    UTL_ATTRIBUTE(NO_UNIQUE_ADDRESS) UTL_SCOPE tuple<Fs...> callables_;
};

template <typename F>
class parallel_executor : public job_executable {
public:
    template <typename... Args, UTL_SCOPE enable_if_t<UTL_SCOPE is_constructible_v<F, Args...>>>
    parallel_executor(size_t size, Args&&... args) noexcept(
        UTL_SCOPE is_nothrow_constructible_v<F, Args...>)
        : callable_(UTL_SCOPE forward<Args>(args)...)
        , size_(size) {}

private:
    size_t size() const noexcept final { return size_; }
    void execute(size_t thread_idx, size_t job_idx) final { callable_(thread_idx, job_idx); }

    UTL_ATTRIBUTE(NO_UNIQUE_ADDRESS) F callable_;
    size_t size_;
};

} // namespace details::thread_pool

class job_handle {
    template <size_t I, typename T>
    using type_for = T;
    using job_executable = details::thread_pool::job_executable;
    template <typename Tag>
    using node_type = job_node<details::thread_pool::job_executable, Tag>;

public:
    template <typename... Jobs>
    [[nodiscard]] static job_handle combine(job_handle&& first, Jobs&&... jobs) {
        static constexpr UTL_SCOPE make_index_sequence<sizeof...(Jobs) + 1> sequence = {};
        static_assert(sizeof...(Jobs) > 0);
        static_assert(!(... || UTL_SCOPE is_reference_v<Jobs>));
        static_assert((... && UTL_SCOPE is_convertible_v<Jobs&, job_handle&>));
        return (move(first) + ... + move(jobs));
    }

    job_handle() noexcept = default;
    template <UTL_CONCEPT_CXX20(invocable) F UTL_REQUIRES_CXX11(UTL_TRAIT_is_invocable(F))>
    explicit job_handle(F&& callable)
        : job_(new details::thread_pool::single_executor<UTL_SCOPE decay_t<F>>(
              UTL_SCOPE forward<F>(callable))) {}
    template <UTL_CONCEPT_CXX20(invocable)... Fs UTL_REQUIRES_CXX11(
        sizeof...(Fs) > 1 && UTL_SCOPE conjunction<UTL_SCOPE is_invocable<Fs>...>::value)>
    explicit job_handle(Fs&&... callables)
        : job_(new details::thread_pool::multi_executor<UTL_SCOPE decay_t<Fs>...>(
              UTL_SCOPE forward<Fs>(callables)...)) {}
    template <UTL_CONCEPT_CXX20(invocable<size_t, size_t>) F UTL_REQUIRES_CXX11(
        UTL_TRAIT_is_invocable(F, size_t, size_t))>
    explicit job_handle(size_t size, F&& callable)
        : job_(new details::thread_pool::parallel_executor<UTL_SCOPE decay_t<F>>(
              size, UTL_SCOPE forward<F>(callable))) {}

    template <UTL_CONCEPT_CXX20(invocable) F UTL_REQUIRES_CXX11(UTL_TRAIT_is_invocable(F))>
    explicit job_handle(job_handle&& dependency, F&& callable)
        : job_((job_handle(UTL_SCOPE forward<F>(callable)) << UTL_SCOPE move(dependency))) {
        *this = (UTL_SCOPE move(*this) << UTL_SCOPE move(dependency));
    }
    template <UTL_CONCEPT_CXX20(invocable)... Fs UTL_REQUIRES_CXX11(
        sizeof...(Fs) > 1 && UTL_SCOPE conjunction<UTL_SCOPE is_invocable<Fs>...>::value)>
    explicit job_handle(job_handle&& dependency, Fs&&... callables)
        : job_((job_handle(size, UTL_SCOPE forward<Fs>(callable)...)
              << UTL_SCOPE move(dependency))) {}
    template <UTL_CONCEPT_CXX20(invocable<size_t, size_t>) F UTL_REQUIRES_CXX11(
        UTL_TRAIT_is_invocable(F, size_t, size_t))>
    explicit job_handle(job_handle&& dependency, size_t size, F&& callable)
        : job_handle(
              (job_handle(size, UTL_SCOPE forward<F>(callable)) << UTL_SCOPE move(dependency))) {}

    job_handle(job_handle const&) = delete;
    job_handle& operator=(job_handle const&) = delete;
    job_handle(job_handle&& other) noexcept : job_(UTL_SCOPE exchange(other.job_, nullptr)) {}
    job_handle& operator=(job_handle&& other) noexcept {
        auto to_delete = UTL_SCOPE exchange(job_, UTL_SCOPE exchange(other.job_, nullptr));
        destroy();
    }

    ~job_handle() noexcept { destroy(); }

    [[nodiscard]] explicit operator bool() const noexcept { return to_delete != nullptr; }

private:
    explicit job_handle(job_executable* job) noexcept : job_(job) {}

    template <typename T>
    static constexpr void splice_back(node_type<T>* list, node_type<T>* list2) noexcept {
        auto head2 = list2;
        auto tail2 = get_prev(*list2);
        auto tail = get_prev(*list);
        set_next(*tail2, list);
        set_prev(*head2, tail);
        set_prev(*list, tail2);
        set_next(*tail, head2);
    }

    static constexpr void destroy(dependency_node* ptr) noexcept {
        dependency_node* const origin = ptr;
        do {
            auto next = get_next(*ptr);

            auto const group = node_cast<group_tag>(ptr);
            for (auto c = get_next(*group); c != group; c = get_next(*c)) {
                splice_back(origin, node_cast<dependency_tag>(c));
            }

            delete ptr;
            ptr = next;
        } while (origin != ptr);
    }

    void destroy() noexcept {
        auto ptr = UTL_SCOPE exchange(job_, nullptr);
        if (ptr != nullptr) {
            destroy(ptr);
        }
    }

    template <typename Tag>
    node_type<Tag>* get() const noexcept {
        return node_cast<Tag>(job_);
    }

    template <typename Tag>
    node_type<Tag>* release() noexcept {
        return node_cast<Tag>(UTL_SCOPE exchange(job_, nullptr));
    }

    [[nodiscard]] friend job_handle operator+(job_handle&& left, job_handle&& right) UTL_THROWS {
        auto left_ptr = left.release<group_tag>();
        auto right_ptr = right.release<group_tag>();
        if (left_ptr == nullptr) {
            return {right_ptr};
        }
        if (right_ptr == nullptr) {
            return {left_ptr};
        }

        splice_back(left_ptr, right_ptr);
        return {left_ptr};
    }

    friend job_handle& operator+=(job_handle& left, job_handle&& other) UTL_THROWS {
        return left = UTL_SCOPE move(left) + UTL_SCOPE move(other);
    }

    [[nodiscard]] friend job_handle operator>>(
        job_handle&& upstream, job_handle&& downstream) UTL_THROWS {
        return move(downstream) << move(upstream);
    }

    [[nodiscard]] friend job_handle operator<<(
        job_handle&& dependent, job_handle&& dependency) UTL_THROWS {

        auto downstream = dependent.release<dependency_tag>();
        auto upstream = dependency.release<dependency_tag>();
        if (downstream == nullptr) {
            return job_handle{node_cast(upstream)};
        }
        if (upstream == nullptr) {
            return job_handle{node_cast(downstream)};
        }

        auto downstream_previous = get_previous(*downstream);
        bool const has_dependencies_already = downstream_previous != downstream;
        if (has_dependencies_already) {
            splice_back(node_cast<group_tag>(downstream_previous), node_cast<group_tag>(upstream));
        } else {
            splice_back(upstream, downstream);
        }

        return job_handle{downstream};
    }

    job_executable* job_;
};

} // namespace utl::experimental
