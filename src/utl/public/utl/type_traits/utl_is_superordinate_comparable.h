// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_config.h"

#include "utl/type_traits/utl_constants.h"
#include "utl/type_traits/utl_declval.h"
#include "utl/type_traits/utl_is_boolean_testable.h"

UTL_NAMESPACE_BEGIN

namespace details {
namespace superordinate_comparable {
template <typename T, typename U>
using strict_result_t = decltype(UTL_SCOPE declval<T>() > UTL_SCOPE declval<U>());

template <typename T, typename U UTL_REQUIRES_CXX11(
    UTL_TRAIT_is_boolean_testable(strict_result_t<T, U>))>
UTL_REQUIRES_CXX20(UTL_TRAIT_is_boolean_testable(strict_result_t<T, U>))
UTL_HIDE_FROM_ABI UTL_HIDE_FROM_ABI UTL_SCOPE true_type strict_impl(int) noexcept;
template <typename T, typename U>
UTL_HIDE_FROM_ABI UTL_SCOPE false_type strict_impl(float) noexcept;
template <typename T, typename U UTL_REQUIRES_CXX11(
    UTL_TRAIT_is_nothrow_boolean_testable(strict_result_t<T, U>))>
UTL_REQUIRES_CXX20(UTL_TRAIT_is_nothrow_boolean_testable(strict_result_t<T, U>))
UTL_HIDE_FROM_ABI UTL_SCOPE true_type strict_nothrow_check(int) noexcept;
template <typename T, typename U>
UTL_HIDE_FROM_ABI UTL_SCOPE false_type strict_nothrow_check(float) noexcept;

template <typename T, typename U>
using strict_impl_t UTL_NODEBUG = decltype(strict_impl<T, U>(0));
template <typename T, typename U>
using strict_nothrow_t UTL_NODEBUG = decltype(strict_nothrow_check<T, U>(0));

template <typename T, typename U>
using result_t UTL_NODEBUG = decltype(UTL_SCOPE declval<T>() >= UTL_SCOPE declval<U>());

template <typename T, typename U UTL_REQUIRES_CXX11(UTL_TRAIT_is_boolean_testable(result_t<T, U>))>
UTL_REQUIRES_CXX20(UTL_TRAIT_is_boolean_testable(result_t<T, U>))
UTL_HIDE_FROM_ABI UTL_SCOPE true_type impl(int) noexcept;
template <typename T, typename U>
UTL_HIDE_FROM_ABI UTL_SCOPE false_type impl(float) noexcept;

template <typename T, typename U UTL_REQUIRES_CXX11(
    UTL_TRAIT_is_nothrow_boolean_testable(result_t<T, U>))>
UTL_REQUIRES_CXX20(UTL_TRAIT_is_nothrow_boolean_testable(result_t<T, U>))
UTL_HIDE_FROM_ABI UTL_SCOPE true_type nothrow_check(int) noexcept;
template <typename T, typename U>
UTL_HIDE_FROM_ABI UTL_SCOPE false_type nothrow_check(float) noexcept;

template <typename T, typename U>
using impl_t UTL_NODEBUG = decltype(impl<T, U>(0));
template <typename T, typename U>
using nothrow_t UTL_NODEBUG = decltype(nothrow_check<T, U>(0));
} // namespace superordinate_comparable
} // namespace details

template <typename T, typename U>
struct UTL_PUBLIC_TEMPLATE is_strict_superordinate_comparable_with :
    details::superordinate_comparable::strict_impl_t<T, U> {};
template <typename T>
struct UTL_PUBLIC_TEMPLATE is_strict_superordinate_comparable :
    details::superordinate_comparable::strict_impl_t<T, T> {};

template <typename T, typename U>
struct UTL_PUBLIC_TEMPLATE is_nothrow_strict_superordinate_comparable_with :
    details::superordinate_comparable::strict_nothrow_t<T, U> {};
template <typename T>
struct UTL_PUBLIC_TEMPLATE is_nothrow_strict_superordinate_comparable :
    details::superordinate_comparable::strict_nothrow_t<T, T> {};

#if UTL_CXX14

template <typename T, typename U>
UTL_INLINE_CXX17 constexpr bool is_strict_superordinate_comparable_with_v =
    is_strict_superordinate_comparable_with<T, U>::value;
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_strict_superordinate_comparable_v =
    is_strict_superordinate_comparable<T>::value;
template <typename T, typename U>
UTL_INLINE_CXX17 constexpr bool is_nothrow_strict_superordinate_comparable_with_v =
    is_nothrow_strict_superordinate_comparable_with<T, U>::value;
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_nothrow_strict_superordinate_comparable_v =
    is_nothrow_strict_superordinate_comparable<T>::value;

#  define UTL_TRAIT_is_strict_superordinate_comparable_with(...) \
      UTL_SCOPE is_strict_superordinate_comparable_with_v<__VA_ARGS__>
#  define UTL_TRAIT_is_strict_superordinate_comparable(...) \
      UTL_SCOPE is_strict_superordinate_comparable_v<__VA_ARGS__>
#  define UTL_TRAIT_is_nothrow_strict_superordinate_comparable_with(...) \
      UTL_SCOPE is_nothrow_strict_superordinate_comparable_with_v<__VA_ARGS__>
#  define UTL_TRAIT_is_nothrow_strict_superordinate_comparable(...) \
      UTL_SCOPE is_nothrow_strict_superordinate_comparable_v<__VA_ARGS__>

#else // UTL_CXX14

#  define UTL_TRAIT_is_strict_superordinate_comparable_with(...) \
      UTL_SCOPE is_strict_superordinate_comparable_with<__VA_ARGS__>::value
#  define UTL_TRAIT_is_strict_superordinate_comparable(...) \
      UTL_SCOPE is_strict_superordinate_comparable<__VA_ARGS__>::value
#  define UTL_TRAIT_is_nothrow_strict_superordinate_comparable_with(...) \
      UTL_SCOPE is_nothrow_strict_superordinate_comparable_with<__VA_ARGS__>::value
#  define UTL_TRAIT_is_nothrow_strict_superordinate_comparable(...) \
      UTL_SCOPE is_nothrow_strict_superordinate_comparable<__VA_ARGS__>::value

#endif // UTL_CXX14

template <typename T, typename U>
struct UTL_PUBLIC_TEMPLATE is_superordinate_comparable_with :
    details::superordinate_comparable::impl_t<T, U> {};
template <typename T>
struct UTL_PUBLIC_TEMPLATE is_superordinate_comparable :
    details::superordinate_comparable::impl_t<T, T> {};

template <typename T, typename U>
struct UTL_PUBLIC_TEMPLATE is_nothrow_superordinate_comparable_with :
    details::superordinate_comparable::nothrow_t<T, U> {};
template <typename T>
struct UTL_PUBLIC_TEMPLATE is_nothrow_superordinate_comparable :
    details::superordinate_comparable::nothrow_t<T, T> {};

#if UTL_CXX14

template <typename T, typename U>
UTL_INLINE_CXX17 constexpr bool is_superordinate_comparable_with_v =
    is_superordinate_comparable_with<T, U>::value;
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_superordinate_comparable_v = is_superordinate_comparable<T>::value;
template <typename T, typename U>
UTL_INLINE_CXX17 constexpr bool is_nothrow_superordinate_comparable_with_v =
    is_nothrow_superordinate_comparable_with<T, U>::value;
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_nothrow_superordinate_comparable_v =
    is_nothrow_superordinate_comparable<T>::value;

#  define UTL_TRAIT_is_superordinate_comparable_with(...) \
      UTL_SCOPE is_superordinate_comparable_with_v<__VA_ARGS__>
#  define UTL_TRAIT_is_superordinate_comparable(...) \
      UTL_SCOPE is_superordinate_comparable_v<__VA_ARGS__>
#  define UTL_TRAIT_is_nothrow_superordinate_comparable_with(...) \
      UTL_SCOPE is_nothrow_superordinate_comparable_with_v<__VA_ARGS__>
#  define UTL_TRAIT_is_nothrow_superordinate_comparable(...) \
      UTL_SCOPE is_nothrow_superordinate_comparable_v<__VA_ARGS__>

#else // UTL_CXX14

#  define UTL_TRAIT_is_superordinate_comparable_with(...) \
      UTL_SCOPE is_superordinate_comparable_with<__VA_ARGS__>::value
#  define UTL_TRAIT_is_superordinate_comparable(...) \
      UTL_SCOPE is_superordinate_comparable<__VA_ARGS__>::value
#  define UTL_TRAIT_is_nothrow_superordinate_comparable_with(...) \
      UTL_SCOPE is_nothrow_superordinate_comparable_with<__VA_ARGS__>::value
#  define UTL_TRAIT_is_nothrow_superordinate_comparable(...) \
      UTL_SCOPE is_nothrow_superordinate_comparable<__VA_ARGS__>::value

#endif // UTL_CXX14

UTL_NAMESPACE_END
