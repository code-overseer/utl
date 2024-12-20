// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

#if UTL_USE_STD_tuple

#  include <tuple>

UTL_NAMESPACE_BEGIN

using std::get;
using std::make_tuple;
using std::tuple;
using std::tuple_cat;
using std::tuple_element;
using std::tuple_size;

UTL_NAMESPACE_END

#else // UTL_USE_STD_tuple

#  define UTL_TUPLE_PRIVATE_HEADER_GUARD

#  if !UTL_CXX20
#    include "utl_tuple_cpp17.h"
#  else
#    include "utl_tuple_latest.h"
#  endif

#  include "utl/utility/utl_pair.h"
#  include "utl_tuple_cat.h"
#  include "utl_tuple_get.h"
#  include "utl_tuple_get_element.h"

#  undef UTL_TUPLE_PRIVATE_HEADER_GUARD

#endif // UTL_USE_STD_tuple
