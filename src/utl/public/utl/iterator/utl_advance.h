// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

#include "utl/assert/utl_assert.h"
#include "utl/iterator/utl_iterator_traits.h"
#include "utl/iterator/utl_legacy_bidirectional_iterator.h"
#include "utl/iterator/utl_legacy_input_iterator.h"
#include "utl/iterator/utl_legacy_random_access_iterator.h"
#include "utl/type_traits/utl_declval.h"
#include "utl/type_traits/utl_enable_if.h"
#include "utl/type_traits/utl_is_integral.h"

UTL_NAMESPACE_BEGIN

namespace details {
namespace advance {

template <typename It>
using difference_t UTL_NODEBUG = typename __UTL iterator_traits<It>::difference_type;
template <typename It>
using category_t UTL_NODEBUG = typename __UTL iterator_traits<It>::iterator_category;

template <typename It>
UTL_CONSTEXPR_CXX14 void forward(It& it, difference_t<It> n, __UTL input_iterator_tag) noexcept(
    noexcept(--n, ++it)) {
    do {
        --n;
        ++it;
    } while (n > 0);
}

template <class It>
__UTL_HIDE_FROM_ABI UTL_CONSTEXPR_CXX14 void forward(
    It& it, difference_t<It> n, __UTL random_access_iterator_tag) noexcept(noexcept(it += n)) {
    UTL_ASSERT(n >= 0);
    it += n;
}

template <typename It>
__UTL_HIDE_FROM_ABI UTL_CONSTEXPR_CXX14 void backward(
    It& it, difference_t<It> n, __UTL bidirectional_iterator_tag) noexcept(noexcept(++n, --it)) {
    do {
        ++n;
        --it;
    } while (n < 0);
}

template <typename It>
__UTL_HIDE_FROM_ABI UTL_CONSTEXPR_CXX14 void backward(
    It& it, difference_t<It> n, __UTL random_access_iterator_tag) noexcept(noexcept(it += n)) {
    UTL_ASSERT(n <= 0);
    it += n;
}

template <typename It>
__UTL_HIDE_FROM_ABI UTL_CONSTEXPR_CXX14 void impl(It& it, difference_t<It> n,
    __UTL input_iterator_tag t) noexcept(noexcept(__UTL details::advance::forward(it, n, t))) {
    if (n > 0) {
        __UTL details::advance::forward(it, n, t);
    }
}

template <typename It,
    UTL_CONCEPT_CXX20(__UTL derived_from<__UTL bidirectional_iterator_tag>) Tag>
__UTL_HIDE_FROM_ABI UTL_CONSTEXPR_CXX14 auto impl(It& it, difference_t<It> n, Tag t) noexcept(
    noexcept(__UTL details::advance::forward(it, n, t)) && noexcept(
        __UTL details::advance::backward(it, n, t))) -> UTL_ENABLE_IF_CXX11(void, UTL_TRAIT_is_base_of(__UTL bidirectional_iterator_tag, Tag)) {
    if (n > 0) {
        __UTL details::advance::forward(it, n, t);
    } else if (n < 0) {
        __UTL details::advance::backward(it, n, t);
    }
}

template <typename It>
__UTL_HIDE_FROM_ABI auto nothrow_check(int) noexcept
    -> __UTL bool_constant<noexcept(__UTL details::advance::impl(
        __UTL declval<It&>(), __UTL declval<difference_t<It>>(), category_t<It>{}))>;

template <typename It>
__UTL_HIDE_FROM_ABI auto nothrow_check(float) noexcept -> __UTL false_type;

template <typename It>
using is_nothrow UTL_NODEBUG = decltype(__UTL details::advance::nothrow_check<It>(0));

} // namespace advance
} // namespace details

template <UTL_CONCEPT_CXX20(__UTL input_iterator) It,
    UTL_CONCEPT_CXX20(__UTL integral) Distance>
__UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 auto advance(It& it, Distance n) noexcept(
    __UTL details::advance::is_nothrow<It>::value) -> UTL_ENABLE_IF_CXX11(void, UTL_TRAIT_is_legacy_input_iterator(It) && UTL_TRAIT_is_integral(Distance)) {
    __UTL details::advance::impl(
        it, __UTL details::advance::difference_t<It>(n), __UTL details::advance::category_t<It>());
}

UTL_NAMESPACE_END
