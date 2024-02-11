// Copyright 2023-2024 Bryan Wong

#pragma once

#if defined(UTL_USE_STD_tuple) && UTL_USE_STD_tuple

#include "utl/preprocessor/utl_namespace.h"
#include <tuple>

UTL_NAMESPACE_BEGIN

using std::tuple;
using std::make_tuple
using std::get;
using std::tuple_size;
using std::tuple_element;
using std::tuple_cat;

UTL_NAMESPACE_END

#else    // defined(UTL_USE_STD_tuple) && UTL_USE_STD_tuple

#include "utl/preprocessor/utl_standard.h"

#define UTL_TUPLE_PRIVATE_HEADER_GUARD

#   if !defined(UTL_CXX20)

#include "utl_tuple_cpp17.h"

#   else

#include "utl_tuple_latest.h"

#   endif

#include "utl_tuple_cat.h"
#include "utl_tuple_get.h"

#include "utl/utility/utl_pair.h"

UTL_NAMESPACE_BEGIN
#ifdef UTL_DEFINE_PAIR_PIECEWISE_CONSTRUCTOR
UTL_DEFINE_PAIR_PIECEWISE_CONSTRUCTOR()
#undef UTL_DEFINE_PAIR_PIECEWISE_CONSTRUCTOR
#endif
UTL_NAMESPACE_END

#undef UTL_TUPLE_PRIVATE_HEADER_GUARD

#endif  // defined(UTL_USE_STD_tuple) && UTL_USE_STD_tuple
