// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/iterator/utl_advance.h"
#include "utl/iterator/utl_iterator_traits.h"
#include "utl/iterator/utl_legacy_bidirectional_iterator.h"
#include "utl/preprocessor/utl_config.h"
#include "utl/type_traits/utl_enable_if.h"

UTL_NAMESPACE_BEGIN

template <UTL_CONCEPT_CXX20(UTL_SCOPE bidirectional_iterator) It>
constexpr auto prev(It it, typename UTL_SCOPE iterator_traits<It>::difference_type n = 1) noexcept(
    noexcept(UTL_SCOPE advance(it))) -> UTL_ENABLE_IF_CXX11(It, UTL_TRAIT_is_legacy_bidirectional_iterator(It)) {
    return UTL_SCOPE advance(it, -n), it;
}

UTL_NAMESPACE_END
