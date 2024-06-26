// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/compare/utl_compare_fwd.h"
#include "utl/preprocessor/utl_config.h"
#include "utl/type_traits/utl_constants.h"
#include "utl/type_traits/utl_declval.h"
#include "utl/type_traits/utl_is_convertible.h"

UTL_NAMESPACE_BEGIN

namespace details {
namespace three_way_comparable {

#if UTL_CXX20

template <typename T, typename U>
using result_t = decltype(UTL_SCOPE declval<T>() <=> UTL_SCOPE declval<U>());

template <typename T, typename U UTL_REQUIRES_CXX11(sizeof(result_t<T, U>) > 0)>
UTL_SCOPE true_type possible(int) noexcept;
template <typename T, typename U, typename Cat UTL_REQUIRES_CXX11(
    UTL_TRAIT_is_convertible(result_t<T, U>, Cat))>
UTL_SCOPE true_type impl(int) noexcept;
template <typename T, typename U, typename Cat UTL_REQUIRES_CXX11(
    UTL_TRAIT_is_nothrow_convertible(result_t<T, U>, Cat))>
UTL_SCOPE true_type nothrow_check(int) noexcept;

#endif // UTL_CXX20

template <typename T, typename U>
UTL_SCOPE false_type possible(float) noexcept;

template <typename T, typename U>
UTL_SCOPE false_type impl(float) noexcept;

template <typename T, typename U>
UTL_SCOPE false_type nothrow_check(float) noexcept;

template <typename T, typename U>
using possible_t = decltype(possible<T, U>(0));
template <typename T, typename U, typename Cat>
using impl_t = decltype(impl<T, U, Cat>(0));
template <typename T, typename U, typename Cat>
using nothrow_t = decltype(nothrow_check<T, U, Cat>(0));
} // namespace three_way_comparable
} // namespace details

template <typename T, typename U, typename Cat = UTL_SCOPE partial_ordering>
struct is_three_way_comparable_with : details::three_way_comparable::impl_t<T, U, Cat> {};
template <typename T, typename Cat = UTL_SCOPE partial_ordering>
struct is_three_way_comparable : details::three_way_comparable::impl_t<T, T, Cat> {};

template <typename T, typename U, typename Cat = UTL_SCOPE partial_ordering>
struct is_nothrow_three_way_comparable_with :
    details::three_way_comparable::nothrow_t<T, U, Cat> {};
template <typename T, typename Cat = UTL_SCOPE partial_ordering>
struct is_nothrow_three_way_comparable : details::three_way_comparable::nothrow_t<T, T, Cat> {};

#if UTL_CXX14

template <typename T, typename U>
UTL_INLINE_CXX17 constexpr bool is_three_way_comparable_with_v = is_three_way_comparable_with<T, U>::value;
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_three_way_comparable_v = is_three_way_comparable<T, T>::value;
template <typename T, typename U>
UTL_INLINE_CXX17 constexpr bool is_nothrow_three_way_comparable_with_v =
    is_nothrow_three_way_comparable_with<T, U>::value;
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_nothrow_three_way_comparable_v =
    is_nothrow_three_way_comparable<T, T>::value;

#  define UTL_TRAIT_is_three_way_comparable_with(...) \
      UTL_SCOPE is_three_way_comparable_with_v<__VA_ARGS__>
#  define UTL_TRAIT_is_three_way_comparable(...) UTL_SCOPE is_three_way_comparable_v<__VA_ARGS__>
#  define UTL_TRAIT_is_nothrow_three_way_comparable_with(...) \
      UTL_SCOPE is_nothrow_three_way_comparable_with_v<__VA_ARGS__>
#  define UTL_TRAIT_is_nothrow_three_way_comparable(...) \
      UTL_SCOPE is_nothrow_three_way_comparable_v<__VA_ARGS__>

#else // UTL_CXX14

#  define UTL_TRAIT_is_three_way_comparable_with(...) \
      UTL_SCOPE is_three_way_comparable_with<__VA_ARGS__>::value
#  define UTL_TRAIT_is_three_way_comparable(...) \
      UTL_SCOPE is_three_way_comparable<__VA_ARGS__>::value
#  define UTL_TRAIT_is_nothrow_three_way_comparable_with(...) \
      UTL_SCOPE is_nothrow_three_way_comparable_with<__VA_ARGS__>::value
#  define UTL_TRAIT_is_nothrow_three_way_comparable(...) \
      UTL_SCOPE is_nothrow_three_way_comparable<__VA_ARGS__>::value

#endif // UTL_CXX14

UTL_NAMESPACE_END
