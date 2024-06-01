// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_config.h"

#if UTL_CXX20
#  include "utl/concepts/utl_boolean_testable.h"
#  include "utl/concepts/utl_convertible_to.h"
#  include "utl/concepts/utl_same_as.h"
#  include "utl/type_traits/utl_common_reference.h"
#  include "utl/type_traits/utl_declval.h"
#  include "utl/type_traits/utl_is_lvalue_reference.h"
#  include "utl/type_traits/utl_remove_cvref.h"
#  include "utl/type_traits/utl_remove_reference.h"

UTL_NAMESPACE_BEGIN

namespace details {
namespace equality_comparable {
template <typename T, typename U, typename C = common_reference_t<T const&, U const&>>
concept with_common_type_impl =
    same_as<common_reference_t<T const&, U const&>, common_reference_t<U const&, T const&>> &&
    requires {
        requires convertible_to<T const&, C const&> || convertible_to<T, C const&>;
        requires convertible_to<U const&, C const&> || convertible_to<U, C const&>;
    };

template <typename T, typename U>
concept with_common_type = with_common_type_impl<remove_cvref_t<T>, remove_cvref_t<U>>;

template <typename T, typename U>
concept weak = requires(remove_reference_t<T> const& t, remove_reference_t<U> const& u) {
    { t == u } -> boolean_testable;
    { t != u } -> boolean_testable;
    { u == t } -> boolean_testable;
    { u != t } -> boolean_testable;
};
} // namespace equality_comparable
} // namespace details

template <typename T>
concept equality_comparable = details::equality_comparable::weak<T, T>;

template <typename T, typename U>
concept equality_comparable_with = equality_comparable<T> && equality_comparable<U> &&
    details::equality_comparable::with_common_type<T, U> &&
    equality_comparable<
        common_reference_t<remove_reference_t<T> const&, remove_reference_t<U> const&>> &&
    details::equality_comparable::weak<T, U>;

UTL_NAMESPACE_END

#endif
