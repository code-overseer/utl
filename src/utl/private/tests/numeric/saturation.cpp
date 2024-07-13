// Copyright 2023-2024 Bryan Wong

#include "utl/numeric/utl_add_sat.h"
#include "utl/numeric/utl_sub_sat.h"

UTL_NAMESPACE_BEGIN
namespace details {
namespace sub_sat {
namespace compile_time {
static_assert(signed_impl<int>(UTL_NUMERIC_max(int), -1).result() == UTL_NUMERIC_max(int));
static_assert(signed_impl<int>(UTL_NUMERIC_max(int), 1).result() == UTL_NUMERIC_max(int) - 1);
static_assert(signed_impl<int>(UTL_NUMERIC_max(int), UTL_NUMERIC_max(int)).result() == 0);
static_assert(signed_impl<int>(UTL_NUMERIC_min(int), -1).result() == UTL_NUMERIC_min(int) + 1);
static_assert(signed_impl<int>(UTL_NUMERIC_min(int), 1).result() == UTL_NUMERIC_min(int));

static_assert(signed_impl<int>(1, -1).result() == 2);
static_assert(signed_impl<int>(1, 1).result() == 0);

} // namespace compile_time
} // namespace sub_sat

namespace add_sat {
namespace compile_time {
static_assert(signed_impl<int>(UTL_NUMERIC_max(int), -1).result() == UTL_NUMERIC_max(int) - 1);
static_assert(signed_impl<int>(UTL_NUMERIC_max(int), 1).result() == UTL_NUMERIC_max(int));
static_assert(
    signed_impl<int>(UTL_NUMERIC_max(int), UTL_NUMERIC_max(int)).result() == UTL_NUMERIC_max(int));
static_assert(
    signed_impl<int>(UTL_NUMERIC_min(int), UTL_NUMERIC_min(int)).result() == UTL_NUMERIC_min(int));
static_assert(signed_impl<int>(UTL_NUMERIC_min(int), -1).result() == UTL_NUMERIC_min(int));
static_assert(signed_impl<int>(UTL_NUMERIC_min(int), 1).result() == UTL_NUMERIC_min(int) + 1);
static_assert(signed_impl<int>(1, -1).result() == 0);
static_assert(signed_impl<int>(1, 1).result() == 2);
} // namespace compile_time
} // namespace add_sat
} // namespace details
UTL_NAMESPACE_END
