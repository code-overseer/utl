// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/base_preprocessor.h"

#ifdef UTL_CXX20

#  include <concepts>

UTL_NAMESPACE_BEGIN

using std::assignable_from;
using std::common_reference_with;
using std::common_with;
using std::constructible_from;
using std::convertible_to;
using std::copy_constructible;
using std::default_initializable;
using std::derived_from;
using std::destructible;
using std::floating_point;
using std::integral;
using std::move_constructible;
using std::signed_integral;
using std::swappable;
using std::swappable_with;
using std::unsigned_integral;

using std::equality_comparable;
using std::equality_comparable_with;
using std::totally_ordered;
using std::totally_ordered_with;

using std::copyable;
using std::movable;
using std::regular;
using std::semiregular;

using std::equivalence_relation;
using std::invocable;
using std::predicate;
using std::regular_invocable;
using std::relation;
using std::strict_weak_order;
using std::swappable;
using std::swappable_with;

UTL_NAMESPACE_END

#endif
