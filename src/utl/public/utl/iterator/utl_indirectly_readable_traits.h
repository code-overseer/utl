// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

#include "utl/type_traits/utl_remove_cv.h"
#include "utl/type_traits/utl_remove_extent.h"

UTL_NAMESPACE_BEGIN

template <typename>
struct indirectly_readable_traits;

template <typename T>
struct __UTL_PUBLIC_TEMPLATE indirectly_readable_traits<T const> : indirectly_readable_traits<T> {};

UTL_NAMESPACE_END

#if UTL_CXX20

#  include "utl/concepts/utl_array_type.h"
#  include "utl/concepts/utl_object_type.h"

UTL_NAMESPACE_BEGIN

template <typename T>
concept with_member_value_type = requires { typename T::value_type; };
template <typename T>
concept with_member_element_type = requires { typename T::element_type; };

namespace details {
namespace indirectly_readable {

template <typename T>
concept with_member_object_value_type = with_member_value_type<T> && !with_member_element_type<T> &&
    __UTL object_type<typename T::value_type>;

template <typename T>
concept with_member_object_element_type = with_member_element_type<T> &&
    !with_member_value_type<T> && __UTL object_type<typename T::element_type>;

template <typename T>
concept with_resolvable_conflict = with_member_value_type<T> && with_member_element_type<T> &&
    __UTL same_as<__UTL remove_cv_t<typename T::value_type>,
        __UTL remove_cv_t<typename T::element_type>> &&
    __UTL object_type<typename T::value_type>;

template <typename T>
struct traits {
    using value_type UTL_NODEBUG = __UTL remove_cv_t<T>;
};
} // namespace indirectly_readable
} // namespace details

template <typename>
struct __UTL_PUBLIC_TEMPLATE indirectly_readable_traits {};

template <array_type T>
struct __UTL_PUBLIC_TEMPLATE indirectly_readable_traits<T> :
    details::indirectly_readable::traits<__UTL remove_extent_t<T>> {};

template <object_type T>
struct __UTL_PUBLIC_TEMPLATE indirectly_readable_traits<T*> :
    details::indirectly_readable::traits<T> {};

template <__UTL details::indirectly_readable::with_member_object_value_type T>
struct __UTL_PUBLIC_TEMPLATE indirectly_readable_traits<T> :
    details::indirectly_readable::traits<typename T::value_type> {};

template <__UTL details::indirectly_readable::with_member_object_element_type T>
struct __UTL_PUBLIC_TEMPLATE indirectly_readable_traits<T> :
    details::indirectly_readable::traits<typename T::element_type> {};

template <details::indirectly_readable::with_resolvable_conflict T>
struct __UTL_PUBLIC_TEMPLATE indirectly_readable_traits<T> :
    details::indirectly_readable::traits<typename T::value_type> {};

UTL_NAMESPACE_END

#else // UTL_CXX20

#  include "utl/type_traits/utl_constants.h"
#  include "utl/type_traits/utl_enable_if.h"
#  include "utl/type_traits/utl_has_member_element_type.h"
#  include "utl/type_traits/utl_has_member_value_type.h"
#  include "utl/type_traits/utl_is_array.h"

UTL_NAMESPACE_BEGIN

namespace details {
namespace indirectly_readable {

template <typename T>
struct value_identity {
    using value_type UTL_NODEBUG = T;
};

template <typename T, bool = true, bool = UTL_TRAIT_is_object(T)>
struct object_value_type {};
template <typename T>
struct object_value_type<T, true, true> {
    using value_type UTL_NODEBUG = __UTL remove_cv_t<T>;
};

struct empty_trait {};
template <typename T>
__UTL_HIDE_FROM_ABI auto resolve(int, int) noexcept
    -> __UTL enable_if_t<UTL_TRAIT_is_array(T), object_value_type<__UTL remove_extent_t<T>>>;
template <typename T>
__UTL_HIDE_FROM_ABI auto resolve(int, int) noexcept -> object_value_type<typename T::value_type,
    UTL_TRAIT_is_same(
        __UTL remove_cv_t<typename T::value_type>, __UTL remove_cv_t<typename T::element_type>)>;
template <typename T>
__UTL_HIDE_FROM_ABI auto resolve(int, float) noexcept -> object_value_type<typename T::value_type>;
template <typename T>
__UTL_HIDE_FROM_ABI auto resolve(int, float) noexcept
    -> object_value_type<typename T::element_type>;
template <typename T>
__UTL_HIDE_FROM_ABI auto resolve(...) -> empty_trait;

template <typename T>
using impl UTL_NODEBUG = decltype(resolve<T>(0, 0));

} // namespace indirectly_readable
} // namespace details

template <typename T>
struct __UTL_PUBLIC_TEMPLATE indirectly_readable_traits<T*> :
    details::indirectly_readable::object_value_type<T> {};

template <typename T>
struct __UTL_PUBLIC_TEMPLATE indirectly_readable_traits : details::indirectly_readable::impl<T> {};

UTL_NAMESPACE_END

#endif // UTL_CXX20
