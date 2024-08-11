// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/tuple/utl_tuple_fwd.h"

#if UTL_USE_STDPAIR

#else // UTL_USE_STD_tuple

UTL_NAMESPACE_BEGIN

template <typename T0, typename T1>
struct pair;

template <typename T>
struct is_pair;

template <typename T>
struct is_pair_like;

UTL_NAMESPACE_END

#endif // UTL_USE_STDPAIR
