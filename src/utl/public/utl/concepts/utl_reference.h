// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

#include "utl/concepts/utl_lvalue_reference.h"
#include "utl/concepts/utl_rvalue_reference.h"
#include "utl/type_traits/utl_is_reference.h"

#if UTL_CXX20
UTL_NAMESPACE_BEGIN

template <typename T>
concept reference = lvalue_reference<T> || rvalue_reference<T> || UTL_TRAIT_is_reference(T);

UTL_NAMESPACE_END
#endif
