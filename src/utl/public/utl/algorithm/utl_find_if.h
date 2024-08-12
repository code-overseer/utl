// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

#include "utl/concepts/utl_predicate.h"
#include "utl/iterator/utl_legacy_forward_iterator.h"
#include "utl/type_traits/utl_declval.h"
#include "utl/utility/utl_move.h"

#if !UTL_CXX20

#  include "utl/type_traits/utl_enable_if.h"
#  include "utl/type_traits/utl_is_predicate.h"
#  include "utl/type_traits/utl_logical_traits.h"

UTL_NAMESPACE_BEGIN
namespace details {
namespace find_if {

template <typename It, typename F>
using requirement UTL_NODEBUG = __UTL conjunction<__UTL is_legacy_forward_iterator<It>,
    __UTL is_predicate<F, decltype(*__UTL declval<It>())>>;

} // namespace find_if
} // namespace details
UTL_NAMESPACE_END
#endif

UTL_NAMESPACE_BEGIN

template <UTL_CONCEPT_CXX20(forward_iterator) It,
    UTL_CONCEPT_CXX20(predicate<decltype(*__UTL declval<It>())>) F>
__UTL_HIDE_FROM_ABI UTL_CONSTEXPR_CXX14 auto find_if(It first, It last, F&& f)
    -> UTL_ENABLE_IF_CXX11(It, details::find_if::requirement<It, F>::value) {
    for (; first != last; ++first) {
        if (f(*first)) {
            return first;
        }
    }

    return last;
}

// TODO
template <typename ExPolicy, typename It, typename F>
It find_if(ExPolicy&& policy, It first, It last, F&& f) = delete;

UTL_NAMESPACE_END
