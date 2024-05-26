// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_config.h"

#if UTL_CXX20

#  include <concepts>

UTL_NAMESPACE_BEGIN

using std::common_with;
using std::derived_from;
using std::floating_point;
using std::signed_integral;
using std::unsigned_integral;

using std::totally_ordered;
using std::totally_ordered_with;

using std::equivalence_relation;
using std::invocable;
using std::predicate;
using std::regular_invocable;
using std::relation;
using std::strict_weak_order;

UTL_NAMESPACE_END

#endif
