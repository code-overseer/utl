// Copyright 2023-2024 Bryan Wong

#include "utl/numeric/utl_add_sat.h"
#include "utl/numeric/utl_sub_sat.h"

UTL_NAMESPACE_BEGIN
namespace details {
namespace sub_sat {
namespace compile_time {
static_assert(signed_impl<int>(UTL_NUMERIC_maximum(int), -1).result() == UTL_NUMERIC_maximum(int));
static_assert(
    signed_impl<int>(UTL_NUMERIC_maximum(int), 1).result() == UTL_NUMERIC_maximum(int) - 1);
static_assert(signed_impl<int>(UTL_NUMERIC_maximum(int), UTL_NUMERIC_maximum(int)).result() == 0);
static_assert(
    signed_impl<int>(UTL_NUMERIC_minimum(int), -1).result() == UTL_NUMERIC_minimum(int) + 1);
static_assert(signed_impl<int>(UTL_NUMERIC_minimum(int), 1).result() == UTL_NUMERIC_minimum(int));

static_assert(signed_impl<int>(1, -1).result() == 2);
static_assert(signed_impl<int>(1, 1).result() == 0);
} // namespace compile_time
namespace runtime {
template long impl(long, long) noexcept;
template int impl(int, int) noexcept;
template short impl(short, short) noexcept;
template signed char impl(signed char, signed char) noexcept;
} // namespace runtime
} // namespace sub_sat

namespace add_sat {
namespace compile_time {
static_assert(
    signed_impl<int>(UTL_NUMERIC_maximum(int), -1).result() == UTL_NUMERIC_maximum(int) - 1);
static_assert(signed_impl<int>(UTL_NUMERIC_maximum(int), 1).result() == UTL_NUMERIC_maximum(int));
static_assert(signed_impl<int>(UTL_NUMERIC_maximum(int), UTL_NUMERIC_maximum(int)).result() ==
    UTL_NUMERIC_maximum(int));
static_assert(signed_impl<int>(UTL_NUMERIC_minimum(int), UTL_NUMERIC_minimum(int)).result() ==
    UTL_NUMERIC_minimum(int));
static_assert(signed_impl<int>(UTL_NUMERIC_minimum(int), -1).result() == UTL_NUMERIC_minimum(int));
static_assert(
    signed_impl<int>(UTL_NUMERIC_minimum(int), 1).result() == UTL_NUMERIC_minimum(int) + 1);
static_assert(signed_impl<int>(1, -1).result() == 0);
static_assert(signed_impl<int>(1, 1).result() == 2);
} // namespace compile_time
namespace runtime {
template long impl(long, long) noexcept;
template int impl(int, int) noexcept;
template short impl(short, short) noexcept;
template signed char impl(signed char, signed char) noexcept;
} // namespace runtime
} // namespace add_sat
} // namespace details
UTL_NAMESPACE_END
