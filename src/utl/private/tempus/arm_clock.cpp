// Copyright 2023-2024 Bryan Wong

#include "utl/assert/utl_assert.h"
#include "utl/hardware/aarch64/utl_cntfrq.h"
#include "utl/hardware/aarch64/utl_cntpct.h"
#include "utl/hardware/aarch64/utl_pmccntr.h"
#include "utl/tempus/utl_clock.h"
#include "utl/type_traits/utl_is_convertible.h"
#include "utl/type_traits/utl_is_same.h"

#if UTL_ARCH_AARCH64

UTL_NAMESPACE_BEGIN
namespace {

uint32_t clock_frequency() noexcept {
// Provide escape hatch for systems with broken frequencies
#  ifndef UTL_ARM_CNTFRQ
    // TODO Prescaler?
    static uint32_t value = []() {
        return (aarch64::cntfrq(instruction_barrier_none) & 0xffffffffu);
    }();
    return value;
#  else
    static_assert(
        is_convertible<decltype(UTL_ARM_CNTFRQ), uint32_t>::value, "Invalid `UTL_ARM_CNTFRQ`");
    return UTL_ARM_CNTFRQ;
#  endif
}

template <instruction_barrier O>
uint64_t get_timestamp(instruction_barrier_type<O> o) noexcept {
#  ifndef UTL_USE_PMU_HARDWARE_CLOCK
    return aarch64::cntpct(o);
#  else
    // TODO Investigate using PMU counter (which is not invariant)
    return aarch64::pmccntr(o);
#  endif
}
} // namespace

namespace tempus {
bool hardware_ticks::invariant_frequency() noexcept {
#  if !defined(__UTL_NO_INVARIANT_CNT_FREQ) && !defined(UTL_USE_PMU_HARDWARE_CLOCK)
    return clock_frequency() != 0;
#  else
    return false;
#  endif
}

uint64_t hardware_ticks::frequency() noexcept {
    if (invariant_frequency()) {
        return clock_frequency();
    }

    return uint64_t(-1);
}

duration to_duration(hardware_ticks t) noexcept {
    if (!hardware_ticks::invariant_frequency()) {
        return duration::invalid();
    }

    if (t.value() < 0) {
        return duration::invalid();
    }

    static constexpr unsigned int nano = 1000000000;
    return duration{0, t.value() * nano / clock_frequency()};
}

#  define __UTL_DEFINE_GET_TIME(ORDER)                                      \
      auto clock_traits<hardware_clock_t>::get_time(                        \
          decltype(instruction_barrier_##ORDER) o) noexcept -> value_type { \
          return get_timestamp(o);                                          \
      }
__UTL_DEFINE_GET_TIME(none);
__UTL_DEFINE_GET_TIME(after);
__UTL_DEFINE_GET_TIME(before);
__UTL_DEFINE_GET_TIME(enclose);

#  undef __UTL_DEFINE_GET_TIME

} // namespace tempus
UTL_NAMESPACE_END

#endif // UTL_ARCH_AARCH64
