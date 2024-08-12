// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_config.h"

#include "utl/algorithm/utl_find_if.h"
#include "utl/concepts/utl_predicate.h"
#include "utl/iterator/utl_legacy_forward_iterator.h"
#include "utl/type_traits/utl_enable_if.h"
#include "utl/type_traits/utl_invoke.h"
#include "utl/type_traits/utl_is_move_assignable.h"
#include "utl/type_traits/utl_is_predicate.h"
#include "utl/type_traits/utl_logical_traits.h"
#include "utl/type_traits/utl_remove_cvref.h"
#include "utl/utility/utl_move.h"

UTL_NAMESPACE_BEGIN

#if !UTL_CXX20
namespace details {
namespace remove_if {

template <typename It, typename F>
using requirement UTL_NODEBUG = UTL_SCOPE conjunction<UTL_SCOPE is_legacy_forward_iterator<It>,
    UTL_SCOPE is_predicate<F, decltype(*UTL_SCOPE declval<It>())>,
    UTL_SCOPE is_move_assignable<UTL_SCOPE remove_cvref_t<decltype(*UTL_SCOPE declval<It>())>>>;

} // namespace remove_if
} // namespace details
#endif

template <UTL_CONCEPT_CXX20(forward_iterator) It,
    UTL_CONCEPT_CXX20(predicate<decltype(*UTL_SCOPE declval<It>())>) F>
UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD)
UTL_CONSTEXPR_CXX14 auto remove_if(It first, It last, F&& f)
    -> UTL_ENABLE_IF_CXX11(It, details::remove_if::requirement<It, F>::value) {
    first = UTL_SCOPE find_if(first, last, f);
    if (first == last) {
        return first;
    }

    for (It i = first; ++i != last;) {
        if (!f(*i)) {
            *first++ = UTL_SCOPE move(*i);
        }
    }

    return first;
}

// TODO
template <typename ExPolicy, typename It, typename F>
It remove_if(ExPolicy&& policy, It first, It last, F&& f) = delete;

UTL_NAMESPACE_END
