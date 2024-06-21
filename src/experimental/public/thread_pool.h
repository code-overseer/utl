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

/*


UTL_NAMESPACE_BEGIN

template <typename...>
class group_t;
template <typename...>
class graph_t;
template <typename...>
class scatter_t;
UTL_INLINE_CXX17 constexpr class identity_t {
} identity = {};
template <typename...>
struct unwrap;

template <typename>
struct is_group : UTL_SCOPE false_type {};
template <typename... Ts>
struct is_group<group_t<Ts...>> : UTL_SCOPE true_type {};
template <typename>
struct is_graph : UTL_SCOPE false_type {};
template <typename... Ts>
struct is_graph<graph_t<Ts...>> : UTL_SCOPE true_type {};
template <typename>
struct is_scatter : UTL_SCOPE false_type {};
template <typename... Ts>
struct is_scatter<scatter_t<Ts...>> : UTL_SCOPE bool_constant<(sizeof...(Ts) <= 2)> {};

template <template <typename...> class Container>
struct is_instruction : UTL_SCOPE false_type {};
template <>
struct is_instruction<group_t> : UTL_SCOPE true_type {};
template <>
struct is_instruction<graph_t> : UTL_SCOPE true_type {};
template <>
struct is_instruction<scatter_t> : UTL_SCOPE true_type {};

template <typename T>
struct is_instruction_type : UTL_SCOPE false_type {};
template <template <typename...> class Container, typename... Ts>
struct is_instruction_type<Container<Ts...>> : is_instruction<Container> {};

template <typename T>
using is_executable = is_invocable<T>;

template <typename T>
using is_valid_argument =
    UTL_SCOPE disjunction<is_executable<remove_cvref_t<T>>, is_instruction_type<remove_cvref_t<T>>>;

#if UTL_CXX20
template <typename T>
concept executable = invocable<T>;

template <typename T>
concept instruction_type = is_instruction_type<T>::value;

template <typename T>
concept valid_argument = (executable<remove_cvref_t<T>> || instruction_type<remove_cvref_t<T>>);
#endif

template <>
class graph_t<> {
    template <UTL_CONCEPT_CXX20(valid_argument) T UTL_REQUIRES_CXX11(is_valid_argument<T>::value)>
    UTL_ATTRIBUTE(NO_DISCARD)
    constexpr graph_t<UTL_SCOPE decay_t<T>> operator>>(T&& execution) const noexcept {
        return graph_t<UTL_SCOPE decay_t<T>>{UTL_SCOPE forward<T>(execution)};
    }
};
template <typename... Ts>
class graph_t {
public:
    template <UTL_CONCEPT_CXX20(valid_argument) T UTL_REQUIRES_CXX11(is_valid_argument<T>::value)>
    UTL_ATTRIBUTE(NO_DISCARD)
    constexpr graph_t<Ts..., UTL_SCOPE decay_t<T>> operator>>(T&& execution) const&& noexcept {
        static_assert((UTL_TRAIT_is_constructible(UTL_SCOPE decay_t<T>, T) && ... &&
                          UTL_TRAIT_is_move_constructible(Ts)),
            "");

        return UTL_SCOPE apply(
            [&](Ts&&... ts) {
                return graph_t<Ts..., UTL_SCOPE decay_t<T>>{
                    UTL_SCOPE move(ts)..., UTL_SCOPE forward<T>(execution)};
            },
            UTL_SCOPE move(executables));
    }

    template <UTL_CONCEPT_CXX20(valid_argument) T UTL_REQUIRES_CXX11(is_valid_argument<T>::value)>
    UTL_ATTRIBUTE(NO_DISCARD)
    constexpr graph_t<Ts..., UTL_SCOPE decay_t<T>> operator>>(T&& execution) const& noexcept {
        static_assert((UTL_TRAIT_is_constructible(UTL_SCOPE decay_t<T>, T) && ... &&
                          UTL_TRAIT_is_copy_constructible(Ts)),
            "");
        return UTL_SCOPE apply(
            [&](Ts const&... ts) {
                return graph_t<Ts..., UTL_SCOPE decay_t<T>>{ts..., UTL_SCOPE forward<T>(execution)};
            },
            executables);
    }

private:
    UTL_SCOPE tuple<Ts...> executables;
}

template <typename... Ts>
class group_t {

public:
    template <UTL_CONCEPT_CXX20(valid_argument)... Us UTL_REQUIRES_CXX11(
        UTL_SCOPE conjunction<is_valid_argument<Us>...>::value)>
    group_t(Us&&... args) : executables(UTL_SCOPE forward<Us>(args)...) {}

    template <typename T>
    scatter_t<UTL_SCOPE decay_t<T>, group_t> operator*(T&& t) const&& noexcept {
        return scatter_t<UTL_SCOPE decay_t<T>, group_t>{
            UTL_SCOPE forward<T>(t), UTL_SCOPE move(*this)};
    }

    template <typename T>
    scatter_t<UTL_SCOPE decay_t<T>, group_t> operator*(T&& t) const& noexcept {
        static_assert(sizeof...(Ts) > 1, "");
        return scatter_t<UTL_SCOPE decay_t<T>, group_t>{UTL_SCOPE forward<T>(t), *this};
    }

private:
    UTL_SCOPE tuple<Ts...> executables;
};

template <typename T>
concept group_type = is_group<UTL_SCOPE decay_t<T>>::value;
template <typename T>
concept not_group_type = !group_type<T>;

template <not_group_type T, group_type U>
scatter_t<UTL_SCOPE decay_t<T>, UTL_SCOPE decay_t<U>> operator*(T&& t, U&& u) noexcept {
    return UTL_SCOPE forward<U>(u) * UTL_SCOPE forward<T>(t);
}

template <>
class scatter_t<> {
public:
    template <UTL_CONCEPT_CXX20(valid_argument) T UTL_REQUIRES_CXX11(is_valid_argument<T>::value)>
    UTL_ATTRIBUTE(NO_DISCARD)
    constexpr scatter_t<T&&> operator->*(T&& execution UTL_ATTRIBUTE(LIFETIMEBOUND)) const
        noexcept(UTL_TRAIT_is_nothrow_constructible(scatter_t<T>, T)) {
        return scatter_t<T&&>{UTL_SCOPE forward<T>(execution)};
    }
};

template <typename T>
class scatter_t<T> {
    static_assert(UTL_SCOPE is_reference<T>::value, "Invalid scatter");
    static_assert(is_valid_argument<UTL_SCOPE decay_t<T>>::value, "Invalid scatter");
    using reference = T;
    using value_type = UTL_SCOPE decay_t<reference>;

public:
    scatter_t(reference t) noexcept : src_(t) {}

    template <UTL_CONCEPT_CXX20(instruction_type) U UTL_REQUIRES_CXX11(
        is_instruction_type<Container>::value)>
    UTL_ATTRIBUTE(NO_DISCARD)
    constexpr scatter_t<value_type, UTL_SCOPE decay_t<U>> operator*(U&& execution) const noexcept {
        return scatter_t<value_type, UTL_SCOPE decay_t<U>>{
            UTL_SCOPE forward<reference>(src_), UTL_SCOPE forward<U>(execution)};
    }

private:
    reference src_;
};

template <typename T, typename U>
class scatter_t<T, U> {
public:
    // TODO U must be a group_t or scatter

    template <typename TT, typename UU>
    scatter_t(TT&& t, UU&& u) noexcept(
        UTL_TRAIT_is_nothrow_constructible(T, TT) && UTL_TRAIT_is_nothrow_constructible(U, UU))
        : first(UTL_SCOPE forward<TT>(t))
        , second(UTL_SCOPE forward<UU>(u)) {}

private:
    T first;
    U second;
};

template <typename... Ts>
class scatter_t {
    static_assert(sizeof...(Ts) <= 2, "Invalid operation, `scatter` is a binary operation");
};

template <typename... Ts>
struct find_entrypoints;

#if UTL_CXX20

template <typename T0, typename T1>
struct entrypoint_concat {};

template <typename T>
struct is_entrypoint_finder : UTL_SCOPE false_type {};

template <typename... Ts>
struct is_entrypoint_finder<find_entrypoints<Ts...>> : UTL_SCOPE true_type {};

template <typename T>
concept entrypoint_finder = is_entrypoint_finder<T>::value;

template <typename T>
concept entrypoint_result = entrypoint_finder<T> && UTL_TRAIT_has_member_type(T);

template <entrypoint_result T, entrypoint_result U>
struct entrypoint_concat<T, U> {
    using type = UTL_SCOPE concat_elements<typename T::type, typename U::type>;
};

template <entrypoint_result T, entrypoint_finder U>
struct entrypoint_concat<T, U> : T {};

template <entrypoint_finder T, entrypoint_result U>
struct entrypoint_concat<T, U> : U {};

template <>
struct find_entrypoints {
    using type = UTL_SCOPE tuple<>;
};

template <executable F, typename... Ts>
struct find_entrypoints<F, Ts...> : find_entrypoints<Ts...> {};

template <typename... Ts, typename... Us>
struct find_entrypoints<group_t<Ts...>, Us...> :
    entrypoint_concat<find_entrypoints<Ts...>, find_entrypoints<Us...>> {};

template <typename T0, typename T1, typename... Us>
struct find_entrypoints<scatter_t<T0, T1>, Us...> :
    entrypoint_concat<find_entrypoints<T0, T1>, find_entrypoints<Us...>> {};

template <typename T0, typename... Ts, typename... Us>
struct find_entrypoints<graph_t<T0, Ts...>, Us...> :
    UTL_SCOPE concat_elements<UTL_SCOPE tuple<T0&>,
        typename entrypoint_concat<find_entrypoints<Ts...>, find_entrypoints<Us...>>::type> {};

#else

template <typename T0, typename T1 UTL_TYPENAME_CXX11(void)>
struct entrypoint_concat {
    using type = UTL_SCOPE tuple<>;
}

template <typename... Ts, typename... Us>
struct entrypoint_concat<find_entrypoints<Ts...>, find_entrypoints<Us...> UTL_REQUIRES_CXX11(
    UTL_TRAIT_has_member_type(
        find_entrypoints<Ts...>) && UTL_TRAIT_has_member_type(find_entrypoints<Us...>))> {
    using type = UTL_SCOPE concat_elements<typename find_entrypoints<Ts...>::type,
        typename find_entrypoints<Us...>::type>;
};

template <typename... Ts, typename... Us>
struct entrypoint_concat<find_entrypoints<Ts...>, find_entrypoints<Us...> UTL_REQUIRES_CXX11(
    UTL_TRAIT_has_member_type(
        find_entrypoints<Ts...>) && !UTL_TRAIT_has_member_type(find_entrypoints<Us...>))> {
    using type = typename find_entrypoints<Ts...>::type;
};

template <typename... Ts, typename... Us>
struct entrypoint_concat<find_entrypoints<Ts...>, find_entrypoints<Us...> UTL_REQUIRES_CXX11(
    !UTL_TRAIT_has_member_type(
        find_entrypoints<Ts...>) && UTL_TRAIT_has_member_type(find_entrypoints<Us...>))> {
    using type = typename find_entrypoints<Ts...>::type;
};

namespace details {

template <typename T, typename = void>
struct find_entrypoints_impl {
    using type = UTL_SCOPE tuple<>;
};

template <typename F, typename... Ts>
struct find_entrypoints_impl<find_entrypoints<F, Ts...>,
    UTL_SCOPE enable_if_t<is_executable<F>::value>> :
    find_entrypoints_impl<find_entrypoints<Ts...>> {};

template <typename... Ts, typename... Us>
struct find_entrypoints_impl<find_entrypoints<group_t<Ts...>, Us...>> :
    entrypoint_concat<find_entrypoints_impl<Ts...>, find_entrypoints_impl<Us...>> {};

template <typename T0, typename T1, typename... Us>
struct find_entrypoints_impl<find_entrypoints<scatter_t<T0, T1>, Us...>> :
    entrypoint_concat<find_entrypoints_impl<T0, T1>, find_entrypoints_impl<Us...>> {};

template <typename T0, typename... Ts, typename... Us>
struct find_entrypoints_impl<graph_t<T0, Ts...>, Us...> :
    UTL_SCOPE concat_elements<UTL_SCOPE tuple<T0&>,
        typename entrypoint_concat<find_entrypoints<Ts...>, find_entrypoints<Us...>>::type> {};
} // namespace details

template <typename... Ts>
struct find_entrypoints<Ts...> : details::find_entrypoints_impl<Ts...> {};

#endif

//     A
//    / \
//   B   C
//  / \ / \
// D   E   F
//  \ / \ / \
//   G   H   I
//    \ /   / \
//     J   K   L

// graph<J, group<G,H>, scatter<E, group<D, graph<group<K,L>, I, F>>>, group<B,C>, A>
// latch<0, 1,                 <2,      <1,      <0,        , 2, 2>>>,      <2,2>  2>

void func(utl::thread_pool<16> pool) {
    // clang-format off
    auto subgraph = utl::job::grapher >>
        utl::job::group([]() { K; }, []() { L; }) >> []() { I; } >> []() { F; };

    auto graph = utl::job::grapher >> []() { J; } >>
        utl::job::group([]() { G; }, []() { H; })
        >> []() { E; } * utl::job::group([]() { D; }, UTL_SCOPE move(subgraph))
        >> utl::job::group([]() { B; }, []() { C; }) >> []() { A; };
    // clang-format on

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

UTL_INLINE_CXX17 constexpr group_t<> grapher = {};

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


*/
