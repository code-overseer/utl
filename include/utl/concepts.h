// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/base_preprocessor.h"

#ifdef UTL_CXX20

#include <concepts>

UTL_NAMESPACE_BEGIN

using std::derived_from;
using std::convertible_to;
using std::common_reference_with;
using std::common_with;
using std::integral;
using std::signed_integral;
using std::unsigned_integral;
using std::floating_point;
using std::assignable_from;
using std::swappable;
using std::swappable_with;
using std::destructible;
using std::constructible_from;
using std::default_initializable;
using std::move_constructible;
using std::copy_constructible;

using std::equality_comparable;
using std::equality_comparable_with;
using std::totally_ordered;
using std::totally_ordered_with;

using std::movable;
using std::copyable;
using std::semiregular;
using std::regular;

using std::invocable;
using std::regular_invocable;
using std::predicate;
using std::relation;
using std::equivalence_relation;
using std::strict_weak_order;
using std::swappable;
using std::swappable_with;

UTL_NAMESPACE_END

#endif
