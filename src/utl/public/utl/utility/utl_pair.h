// Copyright 2023-2024 Bryan Wong

#pragma once

/** TODO cleanup this implementation and support allocator_arg **/

#if UTL_USE_STDPAIR

#  include "utl/preprocessor/utl_namespace.h"

#  include <utility>

UTL_NAMESPACE_BEGIN

using std::get;
using std::make_pair;
using std::pair;
using std::piecewise_construct;
using std::piecewise_construct_t;
using std::tuple_element;
using std::tuple_size;

UTL_NAMESPACE_END

#else // UTL_USE_STDPAIR

#  define UTL_PAIR_PRIVATE_HEADER_GUARD

#  if !UTL_CXX20
#    include "utl/utility/utl_pair_cpp17.h"
#  else
#    include "utl/utility/utl_pair_latest.h"
#  endif

#  undef UTL_PAIR_PRIVATE_HEADER_GUARD

#endif // UTL_USE_STDPAIR
