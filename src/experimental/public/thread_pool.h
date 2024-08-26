#pragma once
#include "latch.h"
#include "semaphore.h"
#include "utl_config.h"

#include <chrono>
#include <memory>
#include <stdexcept>
#include <thread>
#include <tuple>
#include <type_traits>
#include <utility>
// todo intrusive_ptr
// todo atomic_reference_counter

UTL_NAMESPACE_BEGIN

//     A
//    / \
//   B   C
//  / \ / \
// D   E   F
//  \ / \ / \
//   G   H   I
//    \ /   / \
//     J   K   L

// 1 >> N => N vertex from 1 to N
// N >> 1 => N vertex from N to 1
// N >> N => N vertex one for each node
// N >> M where M != 1 && N != 1 => Not possible

// graph<J, group<G,H>, scatter<E, group<D, graph<group<K,L>, I, F>>>, group<B,C>, A>
// barriers<0,   <1,1>,        <2,      <1,      <     <0,0>, 2, 2>>>,      <2,2>  2>
namespace task_graph {

template <typename... Vs>
class vertex_space;
template <typename VertexSpace, typename... Edges>
class graph;
template <typename, size_t>
struct vertex;
template <typename, size_t...>
struct vertex_group;

template <size_t From, size_t To>
struct edge {
    static_assert(From != To, "Loop detected!");
};

template <typename Edge, typename VertexSpace>
struct edge_source {};
template <size_t From, size_t To, typename... Vs>
struct edge_source<edge<From, To>, vertex_space<Vs...>> {
    using type = template_element_t<From, vertex_space<Vs...>>;
};
template <typename Edge, typename VertexSpace>
struct edge_destination {};
template <size_t From, size_t To, typename... Vs>
struct edge_destination<edge<From, To>, vertex_space<Vs...>> {
    using type = template_element_t<To, vertex_space<Vs...>>;
};
template <typename Edge, typename VertexSpace>
using edge_source_t = typename edge_source<Edge, VertexSpace>::type;
template <typename Edge, typename VertexSpace>
using edge_destination_t = typename edge_destination<Edge, VertexSpace>::type;

template <typename VertexSpace, typename... Es>
struct edge_list;
template <typename... Vs, size_t... I, size_t... J>
struct edge_list<vertex_space<Vs...>, edge<I, J>...> {
    static_assert((... && (template_count<edge<I, J>, type_list<edge<I, J>...>>::value == 1)),
        "Non-unique edge detected");
};

template <typename... Vs, size_t I>
struct vertex<vertex_space<Vs...>, I> {
    static_assert(I < sizeof...(Vs), "Index out of range");
    using type = template_element_t<I, vertex_space<Vs...>>;

private:
    template <size_t N>
    using vertex_type = vertex<vertex_space<Vs...>, N>;
    template <typename... E>
    using edge_result = edge_list<vertex_space<Vs...>, E...>;
    template <size_t... Ns>
    using group_type = vertex_group<vertex_space<Vs...>, Ns...>;

public:
    template <size_t J>
    constexpr group_type<I, J> operator+(vertex_type<J>) const noexcept {
        return {};
    }

    template <size_t... Js>
    constexpr group_type<I, Js...> operator+(group_type<Js...>) const noexcept {
        return {};
    }

    template <size_t J>
    constexpr edge_result<edge<I, J>> operator>>(vertex_type<J>) const noexcept {
        return {};
    }

    template <size_t J>
    constexpr edge_result<edge<J, I>> operator<<(vertex_type<J>) const noexcept {
        return {};
    }

    template <size_t... Js>
    constexpr edge_result<edge<I, Js>...> operator>>(group_type<Js...>) const noexcept {
        return {};
    }

    template <size_t... Js>
    constexpr edge_result<edge<Js, I>...> operator<<(group_type<Js...>) const noexcept {
        return {};
    }
};

template <typename... Vs, size_t... Is>
struct vertex_group<vertex_space<Vs...>, Is...> {
private:
    template <size_t N>
    using vertex_type = vertex<vertex_space<Vs...>, N>;
    template <size_t... Ns>
    using group_type = vertex_group<vertex_space<Vs...>, Ns...>;
    template <typename... E>
    using edge_result = edge_list<vertex_space<Vs...>, E...>;

public:
    template <size_t... Js>
    constexpr group_type<Is..., Js...> operator+(group_type<Js...>) const noexcept {
        return {};
    }

    template <size_t J>
    constexpr group_type<Is..., J> operator+(vertex_type<J>) const noexcept {
        return {};
    }

    template <size_t J>
    constexpr edge_result<edge<Is, J>...> operator>>(vertex_type<J>) const noexcept {
        return {};
    }

    template <size_t J>
    constexpr edge_result<edge<J, Is>...> operator<<(vertex_type<J>) const noexcept {
        return {};
    }

    template <size_t... Js>
    UTL_CONSTRAINT_CXX20(sizeof...(Js) == sizeof...(Is))
    constexpr auto operator>>(group_type<Js...>) const noexcept
        -> UTL_ENABLE_IF_CXX11(edge_result<edge<Is, Js>...>, sizeof...(Js) == sizeof...(Is)) {
        return {};
    }

    template <size_t... Js>
    UTL_CONSTRAINT_CXX20(sizeof...(Js) == sizeof...(Is))
    constexpr auto operator<<(group_type<Js...>) const noexcept
        -> UTL_ENABLE_IF_CXX11(edge_result<edge<Js, Is>...>, sizeof...(Js) == sizeof...(Is)) {
        return {};
    }
};

// https://godbolt.org/z/34ac46bqr (C++11)
// https://godbolt.org/z/x51Mf41q7 (C++14)

namespace details {
template <typename V>
struct permanent_mark {
    __UTL_HIDE_FROM_ABI friend constexpr auto has_mark(permanent_mark) noexcept;
};

template <typename V>
struct activate_permanent_mark {
    __UTL_HIDE_FROM_ABI friend constexpr auto has_mark(permanent_mark<V>) noexcept {
        return true_type{};
    }
};

template <typename V>
__UTL_HIDE_FROM_ABI decltype(has_mark(permanent_mark<V>{})) has_permanent_mark_impl(int) noexcept;
template <typename V>
__UTL_HIDE_FROM_ABI false_type has_permanent_mark_impl(short) noexcept;

template <typename V, typename R = decltype(has_permanent_mark_impl<V>(0))>
using has_permanent_mark UTL_NODEBUG = R;

template <typename V>
struct temporary_mark {
    __UTL_HIDE_FROM_ABI friend constexpr auto has_mark(temporary_mark) noexcept;
};

template <typename V>
struct activate_temporary_mark {
    __UTL_HIDE_FROM_ABI friend constexpr auto has_mark(temporary_mark<V>) noexcept {
        return true_type{};
    }
};

template <typename V>
__UTL_HIDE_FROM_ABI decltype(has_mark(temporary_mark<V>{})) has_temporary_mark_impl(int) noexcept;
template <typename V>
__UTL_HIDE_FROM_ABI false_type has_temporary_mark_impl(short) noexcept;

template <typename V, typename R0 = decltype(has_temporary_mark_impl<V>(0)),
    typename R1 = decltype(has_permanent_mark_impl<V>(0))>
using has_temporary_mark = bool_constant<!R1::value && R0::value>;

template <size_t, typename>
struct vertex_visitor;

template <size_t Idx, typename... Vs, size_t... Fs, size_t... Ts>
struct vertex_visitor<Idx, graph<vertex_space<Vs...>, edge<Fs, Ts>...>> {};

template <bool HasCycle, typename Visitor>
struct complete_visit_result : false_type {};

template <size_t Idx, typename... Vs, size_t... Fs, size_t... Ts>
struct complete_visit_result<true,
    vertex_visitor<Idx, graph<vertex_space<Vs...>, edge<Fs, Ts>...>>> :
    activate_permanent_mark<vertex_visitor<Idx, graph<vertex_space<Vs...>, edge<Fs, Ts>...>>>,
    true_type {};

template <bool HasTempMark, typename Visitor>
struct visit_vertex_result : false_type {};

template <typename V, typename R0 = decltype(has_permanent_mark_impl<V>(0)),
    typename R1 = decltype(has_temporary_mark_impl<V>(0))>
using visit_vertex UTL_NODEBUG = visit_vertex_result<!R0::value && R1::value, V>;

template <typename Visitor>
struct complete_visit {};

template <typename Edge, typename Visitor>
struct visit_next_vertex : true_type {};

template <size_t Idx, size_t Dst, typename Graph>
struct visit_next_vertex<edge<Idx, Dst>, vertex_visitor<Idx, Graph>> :
    visit_vertex<vertex_visitor<Dst, Graph>> {};

template <size_t Idx, typename... Vs, size_t... Fs, size_t... Ts>
struct complete_visit<vertex_visitor<Idx, graph<vertex_space<Vs...>, edge<Fs, Ts>...>>> :
    complete_visit_result<
        conjunction<visit_next_vertex<edge<Fs, Ts>,
            vertex_visitor<Idx, graph<vertex_space<Vs...>, edge<Fs, Ts>...>>>...>::value,
        vertex_visitor<Idx, graph<vertex_space<Vs...>, edge<Fs, Ts>...>>> {};

template <typename... Vs, size_t... Fs, size_t... Ts, size_t Idx>
struct visit_vertex_result<false,
    vertex_visitor<Idx, graph<vertex_space<Vs...>, edge<Fs, Ts>...>>> :
    activate_temporary_mark<vertex_visitor<Idx, graph<vertex_space<Vs...>, edge<Fs, Ts>...>>>,
    complete_visit<vertex_visitor<Idx, graph<vertex_space<Vs...>, edge<Fs, Ts>...>>> {};

template <size_t... Is, typename Graph>
__UTL_HIDE_FROM_ABI auto is_acyclic_impl(Graph g, index_sequence<Is...>) noexcept
    -> conjunction<visit_vertex<vertex_visitor<Is, Graph>>...>;
template <typename... Vs, size_t... Fs, size_t... Ts>
__UTL_HIDE_FROM_ABI auto is_acyclic_impl(graph<vertex_space<Vs...>, edge<Fs, Ts>...> g) noexcept
    -> decltype(is_acyclic_impl(g, index_sequence_for<Vs...>{}));

template <typename Graph>
using is_acyclic_impl_t UTL_NODEBUG = decltype(is_acyclic_impl(Graph{}));
} // namespace details

template <typename Graph>
struct is_acyclic : false_type {};

template <typename... Vs, size_t... Fs, size_t... Ts>
struct is_acyclic<graph<vertex_space<Vs...>, edge<Fs, Ts>...>> :
    details::is_acyclic_impl_t<graph<vertex_space<Vs...>, edge<Fs, Ts>...>> {};

template <size_t, typename>
struct dependency_count {};

template <size_t I, typename... Vs, size_t... Fs, size_t... Ts>
struct dependency_count<I, graph<vertex_space<Vs...>, edge<Fs, Ts>...>> :
    size_constant<(0 + ... + (Ts == I))> {};

template <size_t>
class barrier {};

#if UTL_TRAIT_SUPPORTED_is_final
#  define UTL_TRAIT_is_final_or(X, F) UTL_TRAIT_is_final(F)
#else
#  define UTL_TRAIT_is_final_or(X, F) X
#endif

namespace details {
template <size_t I, typename F,
    bool = UTL_TRAIT_is_object(F) && UTL_TRAIT_is_empty(F) && !UTL_TRAIT_is_polymorphic(F) &&
        !UTL_TRAIT_is_final_or(true, F)>
struct element;
#undef UTL_TRAIT_is_final_or

template <size_t I, typename F>
class UTL_ATTRIBUTE(EMPTY_BASES) element<I, F, true> : private F {
public:
    using value_type = F;

    template <typename... Args>
    __UTL_HIDE_FROM_ABI explicit element(Args&&... args) noexcept(
        UTL_TRAIT_is_nothrow_constructible(F, Args...))
        : F(__UTL forward<Args>(args)...) {}
    __UTL_HIDE_FROM_ABI element(element const&) = default;
    __UTL_HIDE_FROM_ABI element& operator=(element const&) = default;
    __UTL_HIDE_FROM_ABI element(element&&) noexcept = default;
    __UTL_HIDE_FROM_ABI element& operator=(element&&) noexcept = default;

    template <typename... Args>
    __UTL_HIDE_FROM_ABI invoke_result_t<F, Args...> operator()(Args&&... args) const {
        __UTL invoke(*static_cast<F*>(this), __UTL forward<Args>(args)...);
    }

    __UTL_HIDE_FROM_ABI F&& value() && noexcept { return __UTL move(*static_cast<F*>(this)); }
    __UTL_HIDE_FROM_ABI F const&& value() const&& noexcept {
        return __UTL move(*static_cast<F const*>(this));
    }
    __UTL_HIDE_FROM_ABI F& value() & noexcept { return *static_cast<F*>(this); }
    __UTL_HIDE_FROM_ABI F const& value() const& noexcept { return *static_cast<F const*>(this); }
};

template <size_t I, typename F>
class element<I, F, false> {
public:
    using value_type = F;

    template <typename... Args>
    __UTL_HIDE_FROM_ABI invoke_result_t<F, Args...> operator()(Args&&... args) const {
        __UTL invoke(callable, __UTL forward<Args>(args)...);
    }

    template <typename... Args>
    __UTL_HIDE_FROM_ABI explicit element(Args&&... args) noexcept(
        UTL_TRAIT_is_nothrow_constructible(F, Args...))
        : callable(__UTL forward<Args>(args)...) {}
    __UTL_HIDE_FROM_ABI element(element const&) = default;
    __UTL_HIDE_FROM_ABI element& operator=(element const&) = default;
    __UTL_HIDE_FROM_ABI element(element&&) noexcept = default;
    __UTL_HIDE_FROM_ABI element& operator=(element&&) noexcept = default;

    __UTL_HIDE_FROM_ABI F&& value() && noexcept { return callable; }
    __UTL_HIDE_FROM_ABI F const&& value() const&& noexcept { return callable; }
    __UTL_HIDE_FROM_ABI F& value() & noexcept { return callable; }
    __UTL_HIDE_FROM_ABI F const& value() const& noexcept { return callable; }

private:
    F callable;
};

template <typename Seq, typename... Vs>
class graph_tuple_impl;
template <typename... Vs, size_t... Is>
class graph_tuple_impl<index_sequence<Is...>, Vs...> : element<Is, Vs>... {
    template <size_t I>
    using element_type UTL_NODEBUG = template_element_t<I, type_list<Vs...>>;
    template <size_t I>
    using base_type UTL_NODEBUG = element<I, element_type<I>>;
    static constexpr size_t paths =
        numeric::sum<size_t>((dependency_count<Is, graph>::value == 0)...);

public:
    template <typename... Args>
    __UTL_HIDE_FROM_ABI explicit vertex_space_impl(Args&&... args) noexcept(
        __UTL conjunction<__UTL is_nothrow_constructible<element<Is, Vs>, Args>...>::value)
        : element<Is, Vs>(__UTL forward<Args>(args))... {}
    __UTL_HIDE_FROM_ABI vertex_space_impl(vertex_space_impl const&) noexcept(
        __UTL conjunction<__UTL is_nothrow_copy_constructible<Vs>...>::value) = default;
    __UTL_HIDE_FROM_ABI vertex_space_impl& operator=(vertex_space_impl const&) noexcept(
        __UTL conjunction<__UTL is_nothrow_copy_assignable<Vs>...>::value) = default;
    __UTL_HIDE_FROM_ABI vertex_space_impl(vertex_space_impl&&) noexcept(
        __UTL conjunction<__UTL is_nothrow_move_constructible<Vs>...>::value) = default;
    __UTL_HIDE_FROM_ABI vertex_space_impl& operator=(vertex_space_impl&&) noexcept(
        __UTL conjunction<__UTL is_nothrow_move_assignable<Vs>...>::value) = default;

    template <size_t I, typename... Args>
    __UTL_HIDE_FROM_ABI invoke_result_t<base_type<I>, Args...> operator()(
        size_constant<I>, Args&&... args) const
        noexcept(UTL_TRAIT_is_nothrow_invocable_r(
            invoke_result_t<base_type<I>, Args...>, base_type<I>, Args...)) {
        return __UTL invoke(*static_cast<base_type<I>*>(this), __UTL forward<Args>(args)...);
    }

    template <size_t I>
    __UTL_HIDE_FROM_ABI element_type<I> const& get() const& noexcept {
        return static_cast<base_type<I> const*>(this)->value();
    }

    template <size_t I>
    __UTL_HIDE_FROM_ABI element_type<I>& get() & noexcept {
        return static_cast<base_type<I>*>(this)->value();
    }

    template <size_t I>
    __UTL_HIDE_FROM_ABI element_type<I> const&& get() const&& noexcept {
        return __UTL move(*static_cast<base_type<I> const*>(this)).value();
    }

    template <size_t I>
    __UTL_HIDE_FROM_ABI element_type<I>&& get() && noexcept {
        return __UTL move(*static_cast<base_type<I>*>(this)).value();
    }
};
template <typename... Vs>
using graph_tuple = graph_tuple_impl<index_sequence_for<Vs...>, Vs...>;

template <size_t... Is, typename Graph>
__UTL_HIDE_FROM_ABI graph_tuple<barrier<dependency_count<Is, Graph>::value>...> decl_barriers(
    index_sequence<Is...>) noexcept;
} // namespace details

template <typename... Vs>
class vertex_space : details::graph_tuple<Vs...> {
    using base_type UTL_NODEBUG = details::graph_tuple<Vs...>;

public:
    using base_type::base_type;
    using base_type::get;
    using base_type::operator();
};

template <typename... Vs, size_t... Fs, size_t... Ts>
class graph<vertex_space<Vs...>, edge<Fs, Ts>...> {

    static_assert((... && (template_count<edge<Fs, Ts>, type_list<edge<Fs, Ts>...>>::value == 1)),
        "Non-unique edge detected");
    using exception_span = __UTL span<__UTL exception_ptr, sizeof...(Vs)>;

public:
    template <typename S>
    void begin(S& scheduler) {
        static_assert(is_acyclic<graph>::value, "Cycle detected!");
        [&]<size_t... Is>(index_sequence<Is...>) { (..., begin<Is>(scheduler)); }(
            index_sequence_for<Vs...>{});
    }

    __UTL expected<void, exception_span> wait() noexcept UTL_LIFETIMEBOUND {
        graph_barrier_.wait();
        return create_result();
    }

    template <typename R, typename P>
    __UTL expected<void, exception_span> wait(std::chrono::duration<R, P> duration) noexcept UTL_LIFETIMEBOUND {
        graph_barrier_.wait(duration);
        return create_result();
    }

#if UTL_WITH_EXCEPTIONS
    exception_span exceptions() noexcept UTL_LIFETIMEBOUND { return exceptions_; }
#else
    exception_span exceptions() noexcept UTL_LIFETIMEBOUND { return {}; }
#endif

private:
    __UTL expected<void, exception_span> create_result() noexcept UTL_LIFETIMEBOUND {
#if UTL_WITH_EXCEPTIONS
        auto exp_span = exceptions();
        auto const has_error = __UTL any_of(exp_span.data(), exp_span + exp_span.size(),
            [](auto const& ptr) { return ptr != nullptr; });
        if (has_error) {
            return __UTL unexpected(exp_span);
        }
#endif

        return __UTL expected(__UTL in_place);
    }

private:
    using vertices = vertex_space<Vs...>;
    using vertex_barriers = decltype(details::decl_barriers(index_sequence_for<Vs...>{}));

    template <size_t I, typename S>
    void begin(S& scheduler) {
        if constexpr (dependency_count<I, graph>::value == 0) {
            this->schedule<I>(scheduler);
        }
    }

    template <size_t I, typename S>
    void schedule(S& scheduler) {
        static constexpr size_constant<I> index{};
        scheduler.schedule([&]() {
            UTL_TRY {
                __UTL invoke(vertices_, index);
                (..., on_complete<I, Fs, Ts>(scheduler));
                graph_barrier_.arrive(); // noexcept
            } UTL_CATCH(...) {
                exceptions_[I] = __UTL current_exception();
                (..., on_fail<I, Fs, Ts>());
            }
        });
    }

    template <size_t Current, size_t From, size_t To, typename S>
    void on_complete(S& scheduler) {
        if constexpr (Current == From) {
            if (__UTL get_element<To>(vertex_barriers_).arrive()) {
                this->schedule<To>(scheduler);
            }
        }
    }

    template <size_t Current, size_t From, size_t To>
    UTL_ATTRIBUTE(FLATTEN) void on_fail() noexcept {
        if constexpr (Current == From) {
            graph_barrier_.arrive(); // noexcept
            (..., on_fail<From, Fs, Ts>());
        }
    }

    barrier<sizeof...(Vs)> graph_barrier_;
#if UTL_WITH_EXCEPTIONS
    exception_ptr exceptions_[sizeof...(Vs)];
#endif
    alignas(64) vertex_space<Vs...> vertices_;
    alignas(64) vertex_barriers vertex_barriers_;
};
} // namespace task_graph

//     A
//    / \
//   B   C
//  / \ / \
// D   E   F
//  \ / \ / \
//   G   H   I
//    \ /   / \
//     J   K   L

void func(utl::thread_pool<16> pool) {
    auto vs = make_vertices(A, B, C, D, E, F, G, H, I, J, K, L);
    auto [a, b, c, d, e, f, g, h, i, j, k, l] = split(vs);

    auto graph = make_graph(move(vs), k + l >> i, i >> f, j >> g + h, h >> f, g + h >> e, g >> d,
        d >> b, e >> b + c, f >> c, b + c >> a);

    pool.execute(graph);

    // graph is copyable and/or movable depending on nodes

    // graph_handle moveable only
    // ctor of graph handle will call `new`; allocators cannot be used due to type-hiding
    // requirement
    utl::job::graph_handle handle(graph); // copy/move graph
    utl::job::graph_handle handle2(move(graph));

    pool.execute(handle); // execute and wait until done
    // future moveable only
    // empty future does noting

    auto future = pool.execute_async(move(handle));

    auto future2 = future.then(move(handle2));
    // future.detach(); // Similar to thread::detach
    // future.wait(); // Similar to thread::join
    // release the graph handle back for reuse, will wait until completion
    handle = future.release();

    // dtor of future should call wait
}

UTL_NAMESPACE_END

// A : B, C
// B : D, E
// C : E, F
// D : G
// E : G, H
// F : H, I
// G : J
// H : J
// I : K, L
// J :
// K :
// L :

// I : waiting for 2
// G : waiting for 1
