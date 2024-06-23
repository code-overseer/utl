// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_config.h"
#include "utl/type_traits/utl_remove_cv.h"
#include "utl/type_traits/utl_remove_extent.h"

UTL_NAMESPACE_BEGIN

template <typename>
struct indirectly_readable_traits;

template <typename T>
struct indirectly_readable_traits<T const> : indirectly_readable_traits<T> {};

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
struct object_value_type {};

template <object_type T>
struct object_value_type<T> {
    using value_type = UTL_SCOPE remove_cv_t<T>;
};

template <typename T>
concept with_conflict_member_types = with_member_value_type<T> && with_member_element_type<T>;

template <typename T>
concept with_resolvable_conflict = with_conflict_member_types<T> &&
    UTL_SCOPE same_as<UTL_SCOPE remove_cv_t<typename T::value_type>,
        UTL_SCOPE remove_cv_t<typename T::element_type>>;
} // namespace indirectly_readable
} // namespace details

template <typename>
struct indirectly_readable_traits {};

template <typename T>
struct indirectly_readable_traits<T*> : details::indirectly_readable::object_value_type<T> {};

template <array_type T>
struct indirectly_readable_traits<T> {
    using value_type = UTL_SCOPE remove_cv_t<UTL_SCOPE remove_extent_t<T>>;
};

template <with_member_value_type T>
struct indirectly_readable_traits<T> {
    using value_type = typename T::value_type;
};

template <with_member_element_type T>
struct indirectly_readable_traits<T> {
    using value_type = typename T::element_type;
};

template <details::indirectly_readable::with_conflict_member_types T>
struct indirectly_readable_traits<T> {};

template <details::indirectly_readable::with_resolvable_conflict T>
struct indirectly_readable_traits<T> {
    using value_type = typename T::value_type;
};

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
    using value_type = T;
};

template <typename T, typename = void>
struct object_value_type {};

template <typename T>
struct object_value_type<T, UTL_SCOPE enable_if_t<UTL_TRAIT_is_object(T)>> {
    using value_type = UTL_SCOPE remove_cv_t<T>;
};

template <typename T UTL_REQUIRES_CXX11(UTL_TRAIT_is_array(T))>
auto resolve(int) noexcept -> value_identity<UTL_SCOPE remove_cv_t<UTL_SCOPE remove_extent_t<T>>>;
template <typename T UTL_REQUIRES_CXX11(
    UTL_TRAIT_has_member_value_type(T) && !UTL_TRAIT_has_member_element_type(T))>
auto resolve(int) noexcept -> value_identity<typename T::value_type>;
template <typename T UTL_REQUIRES_CXX11(
    !UTL_TRAIT_has_member_value_type(T) && UTL_TRAIT_has_member_element_type(T))>
auto resolve(int) noexcept -> value_identity<typename T::element_type>;
template <typename T UTL_REQUIRES_CXX11(
    UTL_TRAIT_is_same(typename T::value_type, typename T::element_type))>
auto resolve(int) noexcept -> value_identity<typename T::value_type>;
struct fallback_t {};
template <typename T>
auto resolve(float) -> fallback_t;

template <typename T>
using impl = decltype(resolve<T>(0));

} // namespace indirectly_readable
} // namespace details

template <typename T>
struct indirectly_readable_traits<T*> : details::indirectly_readable::object_value_type<T> {};

template <typename T>
struct indirectly_readable_traits : details::indirectly_readable::impl<T> {};

UTL_NAMESPACE_END

#endif // UTL_CXX20
