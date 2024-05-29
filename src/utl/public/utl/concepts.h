// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_config.h"

#if UTL_CXX20

#  include "utl/concepts/utl_assignable_from.h"
#  include "utl/concepts/utl_common_reference_with.h"
#  include "utl/concepts/utl_constructible_from.h"
#  include "utl/concepts/utl_convertible_to.h"
#  include "utl/concepts/utl_copy_constructible.h"
#  include "utl/concepts/utl_copyable.h"
#  include "utl/concepts/utl_default_initializable.h"
#  include "utl/concepts/utl_destructible.h"
#  include "utl/concepts/utl_equality_comparable.h"
#  include "utl/concepts/utl_floating_point.h"
#  include "utl/concepts/utl_implicit_lifetime.h"
#  include "utl/concepts/utl_integral.h"
#  include "utl/concepts/utl_move_constructible.h"
#  include "utl/concepts/utl_regular.h"
#  include "utl/concepts/utl_same_as.h"
#  include "utl/concepts/utl_semiregular.h"
#  include "utl/concepts/utl_signed_integral.h"
#  include "utl/concepts/utl_unsigned_integral.h"

#  include <concepts>

UTL_NAMESPACE_BEGIN

using std::common_with;
using std::derived_from;

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
