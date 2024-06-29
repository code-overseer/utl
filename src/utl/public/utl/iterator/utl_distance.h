// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_config.h"

#include "utl/iterator/utl_iterator_tags.h"
#include "utl/iterator/utl_iterator_traits.h"
#include "utl/iterator/utl_legacy_input_iterator.h"
#include "utl/string/utl_constant_p.h"
#include "utl/type_traits/utl_declval.h"
#include "utl/type_traits/utl_enable_if.h"
#include "utl/type_traits/utl_is_nothrow_copy_constructible.h"

UTL_NAMESPACE_BEGIN

namespace details {
namespace distance {

template <typename It>
using difference_t = typename UTL_SCOPE iterator_traits<It>::difference_type;
template <typename It>
using category_t = typename UTL_SCOPE iterator_traits<It>::iterator_category;

#if UTL_CXX14

template <typename It>
constexpr difference_t<It> impl(It first, It const last, UTL_SCOPE input_iterator_tag) noexcept(
    noexcept(++first)) {
    difference_t<It> result = 0;
    while (first != last) {
        ++first;
        ++result;
    }
    return result;
}

#else // if UTL_CXX14

namespace compile_time {
template <typename It>
constexpr difference_t<It> impl(It first, It const last, difference_t<It> r = 0) noexcept(
    noexcept((++first, first == last)) && UTL_TRAIT_is_nothrow_copy_constructible(It)) {
    return first == last ? r : impl(++first, last, r + 1);
}
} // namespace compile_time

namespace runtime {
template <typename It>
difference_t<It> impl(It first, It const last) noexcept(noexcept((++first, first != last))) {
    difference_t<It> result = 0;
    while (first != last) {
        ++first;
        ++result;
    }
    return result;
}
} // namespace runtime

template <typename It>
constexpr difference_t<It> impl(It first, It const last, UTL_SCOPE input_iterator_tag) noexcept(
    noexcept(first + 1) && noexcept(first == last) && UTL_TRAIT_is_nothrow_copy_constructible(It)) {
    return UTL_CONSTANT_P(first == last, first, last) ? compile_time::impl(first, last)
                                                      : runtime::impl(first, last);
}

#endif // if UTL_CXX14

template <typename It>
constexpr difference_t<It> impl(It const first, It const last,
    UTL_SCOPE random_access_iterator_tag) noexcept(noexcept(last - first)) {
    return last - first;
}

template <typename It>
auto nothrow_check(int) noexcept
    -> UTL_SCOPE bool_constant<noexcept(UTL_SCOPE details::distance::impl(
        UTL_SCOPE declval<It>(), UTL_SCOPE declval<It>(), category_t<It>{}))>;

template <typename It>
auto nothrow_check(float) noexcept -> UTL_SCOPE false_type;

template <typename It>
using is_nothrow = decltype(UTL_SCOPE details::distance::nothrow_check<It>(0));

} // namespace distance
} // namespace details

template <UTL_CONCEPT_CXX20(input_iterator) It>
constexpr auto distance(It first, It last) noexcept(details::distance::is_nothrow<It>::value)
    -> UTL_ENABLE_IF_CXX11(void, UTL_TRAIT_is_legacy_input_iterator(It)) {
    return details::distance::impl(first, last, UTL_SCOPE details::distance::category_t<It>());
}

UTL_NAMESPACE_END
