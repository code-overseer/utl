// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

#include "utl/iterator/utl_iter_move.h"
#include "utl/iterator/utl_iter_reference_t.h"
#include "utl/iterator/utl_iter_rvalue_reference_t.h"
#include "utl/iterator/utl_iter_value_t.h"
#include "utl/type_traits/utl_constants.h"
#include "utl/type_traits/utl_enable_if.h"
#include "utl/type_traits/utl_remove_cvref.h"

#if UTL_CXX20
#  include "utl/concepts/utl_common_reference_with.h"
#  include "utl/concepts/utl_same_as.h"

UTL_NAMESPACE_BEGIN

namespace details {
namespace indirectly_readable {
template <typename T>
concept concept_impl =
    requires(T const t) {
        typename __UTL iter_value_t<T>;
        typename __UTL iter_reference_t<T>;
        typename __UTL iter_rvalue_reference_t<T>;
        { *t } -> __UTL same_as<__UTL iter_reference_t<T>>;
        { __UTL ranges::iter_move(t) } -> __UTL same_as<__UTL iter_rvalue_reference_t<T>>;
    } && __UTL common_reference_with<__UTL iter_reference_t<T>&&, __UTL iter_value_t<T>&> &&
    __UTL common_reference_with<__UTL iter_reference_t<T>&&, __UTL iter_rvalue_reference_t<T>&&> &&
    __UTL common_reference_with<__UTL iter_rvalue_reference_t<T>&&, __UTL iter_value_t<T>&>;

} // namespace indirectly_readable
} // namespace details

template <typename T>
concept indirectly_readable = details::indirectly_readable::concept_impl<__UTL remove_cvref_t<T>>;

template <typename T>
struct __UTL_PUBLIC_TEMPLATE is_indirectly_readable :
    __UTL bool_constant<indirectly_readable<T>> {};

template <typename T>
inline constexpr bool is_indirectly_readable_v = indirectly_readable<T>;
#  define UTL_TRAIT_is_indirectly_readable(...) __UTL indirectly_readable<__VA_ARGS__>

UTL_NAMESPACE_END

#else

UTL_NAMESPACE_BEGIN
namespace details {
namespace indirectly_readable {

template <typename T>
__UTL_HIDE_FROM_ABI auto trait_impl(float) noexcept -> __UTL false_type;
template <typename T>
__UTL_HIDE_FROM_ABI auto trait_impl(int) noexcept -> __UTL always_true_type<__UTL iter_value_t<T>,
    __UTL iter_reference_t<T>, __UTL iter_rvalue_reference_t<T>>;

template <typename T>
using trait UTL_NODEBUG = decltype(trait_impl<T>(0));

} // namespace indirectly_readable
} // namespace details

template <typename T>
struct is_indirectly_readable : details::indirectly_readable::trait<__UTL remove_cvref_t<T>> {};

#  if UTL_CXX14
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_indirectly_readable_v = is_indirectly_readable<T>::value;
#    define UTL_TRAIT_is_indirectly_readable(...) __UTL is_indirectly_readable_v<__VA_ARGS__>
#  else
#    define UTL_TRAIT_is_indirectly_readable(...) __UTL is_indirectly_readable<__VA_ARGS__>::value
#  endif

UTL_NAMESPACE_END
#endif // UTL_CXX20
