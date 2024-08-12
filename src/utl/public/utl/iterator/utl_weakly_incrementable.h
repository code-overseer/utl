// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_config.h"

#include "utl/iterator/utl_iter_difference_t.h"
#include "utl/type_traits/utl_constants.h"

#if UTL_CXX20
#  include "utl/concepts/utl_movable.h"
#  include "utl/concepts/utl_same_as.h"
#  include "utl/concepts/utl_signed_integral.h"

UTL_NAMESPACE_BEGIN

namespace details {
namespace weakly_incrementable {
template <typename T>
concept pre_incrementable = requires(T t) {
    { ++t } -> same_as<T&>;
};
}
} // namespace details

template <typename T>
concept weakly_incrementable = movable<T> && requires(T t) {
    typename iter_difference_t<T>;
    requires signed_integral<iter_difference_t<T>>;
    requires details::weakly_incrementable::pre_incrementable<T>;
    t++;
};

template <typename T>
struct __UTL_PUBLIC_TEMPLATE is_weakly_incrementable : bool_constant<weakly_incrementable<T>> {};

template <typename T>
inline constexpr bool is_weakly_incrementable_v = weakly_incrementable<T>;

UTL_NAMESPACE_END

#else // UTL_CXX20

#  include "utl/type_traits/utl_enable_if.h"
#  include "utl/type_traits/utl_is_integral.h"
#  include "utl/type_traits/utl_is_same.h"
#  include "utl/type_traits/utl_is_signed.h"
#  include "utl/type_traits/utl_logical_traits.h"

UTL_NAMESPACE_BEGIN

namespace details {
namespace weakly_incrementable {

template <typename T>
__UTL_HIDE_FROM_ABI auto pre_incrementable_impl(int) noexcept
    -> UTL_SCOPE is_same<decltype(++UTL_SCOPE declval<T&>()), T&>;
template <typename T>
__UTL_HIDE_FROM_ABI auto pre_incrementable_impl(float) noexcept -> UTL_SCOPE false_type;

template <typename T>
using pre_incrementable UTL_NODEBUG =
    decltype(UTL_SCOPE details::weakly_incrementable::pre_incrementable_impl<T>(0));

template <typename T>
__UTL_HIDE_FROM_ABI auto trait_impl(int) noexcept
    -> UTL_SCOPE conjunction<UTL_SCOPE is_signed<UTL_SCOPE iter_difference_t<T>>,
        UTL_SCOPE is_integral<UTL_SCOPE iter_difference_t<T>>, pre_incrementable<T>,
        UTL_SCOPE always_true_type<decltype(UTL_SCOPE declval<T&>()++)>>;

template <typename T>
__UTL_HIDE_FROM_ABI auto trait_impl(float) noexcept -> UTL_SCOPE false_type;

template <typename T>
using trait UTL_NODEBUG = decltype(UTL_SCOPE details::weakly_incrementable::trait_impl<T>(0));

} // namespace weakly_incrementable
} // namespace details

template <typename T>
struct __UTL_PUBLIC_TEMPLATE is_weakly_incrementable : details::weakly_incrementable::trait<T> {};

#  if UTL_CXX14
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_weakly_incrementable_v = details::weakly_incrementable::trait<T>::value;
#  endif

UTL_NAMESPACE_END

#endif // UTL_CXX20
