// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/iterator/utl_weakly_incrementable.h"
#include "utl/preprocessor/utl_config.h"
#include "utl/type_traits/utl_constants.h"

#if UTL_CXX20
#  include "utl/concepts/utl_regular.h"
#  include "utl/concepts/utl_signed_integral.h"

UTL_NAMESPACE_BEGIN

template <typename T>
concept incrementable = UTL_SCOPE regular<T> && UTL_SCOPE weakly_incrementable<T> && requires(T t) {
    { i++ } -> UTL_SCOPE same_as<T>;
};

template <typename T>
struct is_incrementable : UTL_SCOPE bool_constant<incrementable<T>> {};

template <typename T>
inline constexpr bool is_incrementable_v = incrementable<T>;

UTL_NAMESPACE_END

#else

#  include "utl/type_traits/utl_void_t.h"

UTL_NAMESPACE_BEGIN

namespace details {
namespace incrementable {
template <typename T, typename = void>
struct post_incrementable : UTL_SCOPE false_type {};

template <typename T>
struct post_incrementable<T, UTL_SCOPE void_t<decltype(static_cast<T (*)()>(0)()++)>> :
    UTL_SCOPE is_same<decltype(static_cast<T (*)()>(0)()++), T> {};
} // namespace incrementable
} // namespace details

template <typename T>
struct is_incrementable :
    UTL_SCOPE conjunction<UTL_SCOPE is_copyable<T>, UTL_SCOPE is_default_constructible<T>,
        UTL_SCOPE is_equality_comparable<T>, UTL_SCOPE is_weakly_incrementable<T>,
        UTL_SCOPE details::incrementable::post_incrementable<T>> {};

#  if UTL_CXX14
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_incrementable_v = is_incrementable<T>::value;
#  endif

UTL_NAMESPACE_END
#endif // UTL_CXX20
