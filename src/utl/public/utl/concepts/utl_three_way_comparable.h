// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

#include "utl/compare/utl_compare_fwd.h"

#include "utl/concepts/utl_boolean_testable.h"
#include "utl/concepts/utl_common_reference_with.h"
#include "utl/concepts/utl_equality_comparable.h"
#include "utl/concepts/utl_partially_ordered_with.h"
#include "utl/concepts/utl_same_as.h"
#include "utl/type_traits/utl_common_comparison_category.h"
#include "utl/type_traits/utl_remove_reference.h"

#if UTL_CXX20
UTL_NAMESPACE_BEGIN

namespace details {
namespace three_way_comparable {
template <typename T, typename Cat>
concept compare_as = same_as<common_comparison_category_t<T, Cat>, Cat>;
} // namespace three_way_comparable
} // namespace details

template <typename T, typename Cat = ::std::partial_ordering>
concept three_way_comparable =
    details::equality_comparable::weak<T, T> && partially_ordered_with<T, T> &&
    requires(remove_reference_t<T> const& l, remove_reference_t<T> const& r) {
        { l <=> r } -> details::three_way_comparable::compare_as<Cat>;
    };

template <typename T, typename U, typename Cat = ::std::partial_ordering>
concept three_way_comparable_with = details::equality_comparable::weak<T, U> &&
    partially_ordered_with<T, U> && three_way_comparable<T, Cat> && three_way_comparable<U, Cat> &&
    common_reference_with<remove_reference_t<T> const&, remove_reference_t<U> const&> &&
    three_way_comparable<
        common_reference_t<remove_reference_t<T> const&, remove_reference_t<U> const&>, Cat> &&
    requires(remove_reference_t<T> const& t, remove_reference_t<U> const& u) {
        { t <=> u } -> details::three_way_comparable::compare_as<Cat>;
        { u <=> t } -> details::three_way_comparable::compare_as<Cat>;
    };

UTL_NAMESPACE_END
#endif
