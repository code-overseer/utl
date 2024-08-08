// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_config.h"

#include "utl/iterator/utl_iterator_traits_fwd.h"

#include "utl/algorithm/utl_remove_if.h"
#include "utl/functional/utl_equal_to.h"
#include "utl/iterator/utl_forward_iterator.h"
#include "utl/type_traits/utl_invoke.h"
#include "utl/utility/utl_forward.h"

UTL_NAMESPACE_BEGIN

template <
    UTL_CONCEPT_CXX20(forward_iterator) It, typename T = typename iterator_traits<It>::value_type>
UTL_ATTRIBUTES(HIDE_FROM_ABI, NODISCARD) constexpr It remove(It first, It last, T const& val) {
    static_assert(UTL_TRAIT_is_invocable(equal_to<void>, decltype(*first), T const&),
        "Arguments must be comparable");
    return UTL_SCOPE remove_if(first, last, [equals = equal_to<void>{}, &val](auto&& v) {
        return equals(UTL_SCOPE forward<decltype(v)>(v), val);
    });
}

// TODO
template <typename ExPolicy, typename It, typename T = typename iterator_traits<It>::value_type>
It remove(ExPolicy&& policy, It first, It last, T const& val) = delete;

UTL_NAMESPACE_END
