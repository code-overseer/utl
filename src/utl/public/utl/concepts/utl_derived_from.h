// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_config.h"
#include "utl/type_traits/utl_is_base_of.h"
#include "utl/type_traits/utl_is_convertible.h"

#if UTL_CXX20
UTL_NAMESPACE_BEGIN

template <typename Derive, typename Base>
concept derived_from = UTL_TRAIT_is_base_of(Base, Derive) &&
    UTL_TRAIT_is_convertible(Derive const volatile*, Base const volatile*);

UTL_NAMESPACE_END
#endif
