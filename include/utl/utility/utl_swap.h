// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_attributes.h"
#include "utl/preprocessor/utl_namespace.h"
#include "utl/preprocessor/utl_standard.h"
#include "utl/preprocessor/utl_pragma.h"

#if defined(UTL_CXX20) && defined(UTL_USE_STD_swap) && UTL_USE_STD_swap

#include <utility>
#include <type_traits>

UTL_NAMESPACE_BEGIN

using std::swap;
using std::is_swappable;
using std::is_nothrow_swappable;

UTL_NAMESPACE_END

#else   // defined(UTL_CXX20) && defined(UTL_USE_STD_swap) && UTL_USE_STD_swap

#if defined(UTL_USE_STD_swap) && UTL_USE_STD_swap
UTL_PRAGMA_WARN("C++ < 20 does not implement a constexpr swap, `UTL_USE_STD_swap` ignored")
#undef UTL_USE_STD_swap
#endif  // defined(UTL_USE_STD_swap) && UTL_USE_STD_swap

#include "utl/type_traits/utl_std_traits.h"

UTL_NAMESPACE_BEGIN

namespace utility {
namespace details {
template<typename T>
using noop_swap = is_empty<T>;
template<typename T>
using can_swap_by_move = conjunction<is_move_constructible<T>, is_move_assignable<T>>;
template<typename T>
using can_swap_by_copy = conjunction<is_copy_constructible<T>, is_copy_assignable<T>>;

template<typename T>
using noop_swap_t = enable_if_t<noop_swap<T>::value>;
template<typename T>
using only_swap_by_move_t = enable_if_t<!noop_swap<T>::value && can_swap_by_move<T>::value>;
template<typename T>
using only_swap_by_copy_t = enable_if_t<!noop_swap<T>::value && !can_swap_by_move<T>::value && can_swap_by_copy<T>::value>;
}   // namespace details
}   // namespace utility

/**
 * No reason to swap an empty class
 * Overload if such a swap is needed
 */
template<typename T>
inline UTL_CONSTEXPR_CXX14 utility::details::noop_swap_t<T> swap(T& a, T& b) noexcept {}

template<typename T>
inline UTL_CONSTEXPR_CXX14 utility::details::only_swap_by_move_t<T> swap(T& a, T& b) noexcept(
    conjunction<
        is_nothrow_move_constructible<T>,
        is_nothrow_move_assignable<T>
    >::value
) {

    T tmp(move(a));
    a = move(b);
    b = move(tmp);
}

template<typename T>
inline UTL_CONSTEXPR_CXX14 utility::details::only_swap_by_copy_t<T> swap(T& a, T& b) noexcept(
    conjunction<
        is_nothrow_copy_constructible<T>,
        is_nothrow_copy_assignable<T>
    >::value
) {

    T tmp(a);
    a = b;
    b = tmp;
}

template<typename T, size_t N>
inline UTL_CONSTEXPR_CXX14 auto swap(T (&a)[N], T (&b)[N]) noexcept(noexcept(swap(*a, *b))) -> void_t<decltype(swap(*a, *b))> {
    for (T *i = a, *j = b, *end = a + N - 1;;) {
        swap(*i, *j);
        if (++i == end) return;
        ++j;
    }
}

namespace utility {
namespace details {
using UTL_SCOPE swap;
struct swap_cpo_t {
    template<typename T, typename U UTL_REQUIRES_CXX11(decltype(swap(declval<T&>(), declval<U&>()), true_type{})::value)>
    UTL_REQUIRES_CXX20(requires { swap(declval<T&>(), declval<U&>()); })
    UTL_ATTRIBUTES(FLATTEN)
    inline UTL_CONSTEXPR_CXX14 void operator()(T& l, U& r) const noexcept(noexcept(swap(declval<T&>(), declval<U&>()))) {
        swap(l, r);
    }
};
}   // namespace details

UTL_INLINE_CXX17 constexpr details::swap_cpo_t swap = {};
}   // namespace utility

namespace utility {
namespace details {

template<typename T, typename U, typename R = decltype(
    utility::swap(declval<T>(), declval<U>()), utility::swap(declval<U>(), declval<T>()), true_type{})>
auto swap_test(int) -> R;
template<typename, typename>
auto swap_test(float) -> false_type;

template<typename T, typename U, bool R = 
    noexcept(swap(declval<T>(), declval<U>())) && noexcept(swap(declval<U>(), declval<T>()))>
auto nothrow_swap_test(int) -> bool_constant<R>;
template<typename, typename>
auto nothrow_swap_test(float) -> false_type;

template<typename T, typename U>
using swap_result = decltype(swap_test<T,U>(0));
template<typename T, typename U>
using nothrow_swap_result = decltype(nothrow_swap_test<T, U>(0));

}   // namespace details
}   // namespace utility

template<typename T, typename U>
struct is_swappable_with : utility::details::swap_result<T, U> {};
template<typename T, typename U>
struct is_nothrow_swappable_with : utility::details::nothrow_swap_result<T, U> {};

template<typename T>
struct is_swappable : is_swappable_with<T&, T&> {};
template<typename T>
struct is_nothrow_swappable : is_nothrow_swappable_with<T&, T&> {};

#define UTL_TRAIT_SUPPORTED_is_swappable 1
#define UTL_TRAIT_SUPPORTED_is_swappable_with 1
#define UTL_TRAIT_SUPPORTED_is_nothrow_swappable 1
#define UTL_TRAIT_SUPPORTED_is_nothrow_swappable_with 1

UTL_NAMESPACE_END

#endif   // defined(UTL_CXX20) && defined(UTL_USE_STD_swap)
