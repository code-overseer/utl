// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_config.h"
#include "utl/type_traits/utl_constants.h"
#include "utl/type_traits/utl_declval.h"
#include "utl/type_traits/utl_is_boolean_testable.h"

UTL_NAMESPACE_BEGIN

namespace details {
namespace equality_comparable {
template <typename T, typename U>
using result_t = decltype(UTL_SCOPE declval<T>() == UTL_SCOPE declval<U>());

template <typename T, typename U UTL_REQUIRES_CXX11(UTL_TRAIT_is_boolean_testable(result_t<T, U>))>
UTL_REQUIRES_CXX20(UTL_TRAIT_is_boolean_testable(result_t<T, U>))
UTL_SCOPE true_type impl(int) noexcept;
template <typename T, typename U>
UTL_SCOPE false_type impl(float) noexcept;
template <typename T, typename U UTL_REQUIRES_CXX11(
    UTL_TRAIT_is_nothrow_boolean_testable(result_t<T, U>))>
UTL_REQUIRES_CXX20(UTL_TRAIT_is_nothrow_boolean_testable(result_t<T, U>))
UTL_SCOPE true_type nothrow_check(int) noexcept;
template <typename T, typename U>
UTL_SCOPE false_type nothrow_check(float) noexcept;

template <typename T, typename U>
using impl_t = decltype(impl<T, U>(0));
template <typename T, typename U>
using nothrow_t = decltype(nothrow_check<T, U>(0));
} // namespace equality_comparable
} // namespace details

template <typename T, typename U>
struct is_equality_comparable_with : details::equality_comparable::impl_t<T, U> {};
template <typename T>
struct is_equality_comparable : details::equality_comparable::impl_t<T, T> {};

template <typename T, typename U>
struct is_nothrow_equality_comparable_with : details::equality_comparable::nothrow_t<T, U> {};
template <typename T>
struct is_nothrow_equality_comparable : details::equality_comparable::nothrow_t<T, T> {};

#if UTL_CXX14

template <typename T, typename U>
UTL_INLINE_CXX17 constexpr bool is_equality_comparable_with_v = is_equality_comparable_with<T, U>::value;
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_equality_comparable_v = is_equality_comparable<T>::value;
template <typename T, typename U>
UTL_INLINE_CXX17 constexpr bool is_nothrow_equality_comparable_with_v =
    is_nothrow_equality_comparable_with<T, U>::value;
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_nothrow_equality_comparable_v =
    is_nothrow_equality_comparable<T>::value;

#  define UTL_TRAIT_is_equality_comparable_with(...) \
      UTL_SCOPE is_equality_comparable_with_v<__VA_ARGS__>
#  define UTL_TRAIT_is_equality_comparable(...) UTL_SCOPE is_equality_comparable_v<__VA_ARGS__>
#  define UTL_TRAIT_is_nothrow_equality_comparable_with(...) \
      UTL_SCOPE is_nothrow_equality_comparable_with_v<__VA_ARGS__>
#  define UTL_TRAIT_is_nothrow_equality_comparable(...) \
      UTL_SCOPE is_nothrow_equality_comparable_v<__VA_ARGS__>

#else // UTL_CXX14

#  define UTL_TRAIT_is_equality_comparable_with(...) \
      UTL_SCOPE is_equality_comparable_with<__VA_ARGS__>::value
#  define UTL_TRAIT_is_equality_comparable(...) UTL_SCOPE is_equality_comparable<__VA_ARGS__>::value
#  define UTL_TRAIT_is_nothrow_equality_comparable_with(...) \
      UTL_SCOPE is_nothrow_equality_comparable_with<__VA_ARGS__>::value
#  define UTL_TRAIT_is_nothrow_equality_comparable(...) \
      UTL_SCOPE is_nothrow_equality_comparable<__VA_ARGS__>::value

#endif // UTL_CXX14

UTL_NAMESPACE_END
