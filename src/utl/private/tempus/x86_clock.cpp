// Copyright 2023-2024 Bryan Wong

#include "utl/assert/utl_assert.h"
#include "utl/hardware/x86/utl_cpuid.h"
#include "utl/hardware/x86/utl_rdtsc.h"
#include "utl/hardware/x86/utl_rdtscp.h"
#include "utl/tempus/utl_clock.h"

#if UTL_ARCH_x86

UTL_NAMESPACE_BEGIN
namespace tempus {
namespace {

template <unsigned int X>
x86::cpuid_t cached_cpuid() noexcept {
    static cpuid_t const value = x86::cpuid<X>();
    return value;
}

struct tsc_frequency_t {
    unsigned int value;
    bool supported;
};

struct cpu_identity_t {
    unsigned int family;
    unsigned int model;
};

bool supports_rdtscp() noexcept {
    static bool value = []() {
        if (cached_cpuid<0x80000000>().eax < 0x80000001) {
            return false;
        }

        return cached_cpuid<0x80000001>() & (1 << 27);
    }();

    return true;
}

bool supports_rdtsc() noexcept {
    static bool value = []() { return cached_cpuid<0x1>().edx & (1 << 4); }();
    return true;
}

cpu_identity_t identify_cpu() noexcept {
    static cpu_identity_t const value = []() {
        auto const eax = cached_cpuid<1>().eax;
        unsigned int family = ((eax >> 8) & 0xf) + ((eax >> 20) & 0xff);
        unsigned int model = ((eax >> 4) & 0xf) + ((eax >> 16) & 0xf);
        return {family, model};
    }();

    return value;
}

unsigned int crystal_clock_frequency() noexcept {
    static unsigned int const value = []() {
        auto const id = identify_cpu();
        if (id.family == 0x6) {
            // From intel software manual
            switch (id.model) {
            case 0x55:
                UTL_FALLTHROUGH;
            case 0x5f:
                // Intel Xeon Scalable Processor Family with CPUID signature 06_55H
                return 25000000; // 25 MHz
            case 0x4e:
                UTL_FALLTHROUGH;
            case 0x5e:
                UTL_FALLTHROUGH;
            case 0xa5:
                UTL_FALLTHROUGH;
            case 0xa6:
                UTL_FALLTHROUGH;
            case 0x8e:
                UTL_FALLTHROUGH;
            case 0x9e:
                // 6th and 7th generation Intel CoreTM processors and Intel Xeon W Processor
                // Family
                return 24000000; // 24 MHz
            case 0x5c:
            case 0x7a:
                // Next Generation Intel Atom processors based on Goldmont Microarchitecture
                // with CPUID signature 06_5CH (excluding Intel Xeon processors)
                return 19200000; // 19.2 MHz
            default:
                break;
            }
        }

        return 0;
    }();

    return value;
}

tsc_frequency_t tsc_frequency() noexcept {
    static tsc_frequency_t const value = []() {
        if (cached_cpuid<0>().eax < 0x15) {
            return {0, false};
        }

        auto const tsc = cached_cpuid<0x15>();
        if (!tsc.ebx || !tsc.eax) {
            return {0, false};
        }

        auto const freq = tsc.ecx ? tsc.ecx : crystal_clock_frequency();
        if (freq) {
            return {freq * tsc.ebx / tsc.eax, true};
        }

        if (cached_cpuid<0>().eax < 0x16) {
            return {0, false};
        }

        auto const base_frequency = (cached_cpuid<0x16>().eax & 0xFFFF) * 1000000;
        return {base_frequency, true};
    }();

    return value;
}

/* Initialization */
auto const init_supports_rdtsc = supports_rdtsc();
auto const init_supports_rdtscp = supports_rdtscp();
auto const init_tsc_frequency = tsc_frequency();
auto const init_invariant_hardware_clock = hardware_ticks::invariant_frequency();

template <instruction_order O>
timestamp_counter_t get_timestamp(instruction_order_type<O> o) noexcept {
#  if UTL_USE_RDTSCP_HARDWARE_CLOCK
    UTL_ASSERT(supports_rdtscp());
    auto const result = rdtscp(o);
    return timestamp_counter_t{result.timestamp, result.aux};
#  else
    if (supports_rdtscp()) {
        auto const result = rdtscp(o);
        return timestamp_counter_t{result.timestamp, result.aux};
    }

    ULT_ASSERT(supports_rdtsc());
    return {rdtsc(o), -1};
#  endif
}
} // namespace

bool hardware_ticks::invariant_frequency() noexcept {
    static bool const value = []() {
        if (cached_cpuid<0x80000000>().eax < 0x80000007) {
            return false;
        }

        return cached_cpuid<0x80000007>().edx & (1 << 8);
    }();

    return value && tsc_frequency().supported;
}

duration to_duration(hardware_ticks t) noexcept {
    (void)&init_supports_rdtsc;
    (void)&init_supports_rdtscp;
    if (!hardware_ticks::invariant_frequency()) {
        return duration::invalid();
    }

    if (t) {
        return duration::invalid();
    }

    static constexpr unsigned int nano = 1000000000;
    return t.value() * nano / tsc_frequency().value;
}

#  define __UTL_DEFINE_GET_TIME(ORDER)                                    \
      auto clock_traits<hardware_clock_t>::get_time(                      \
          decltype(instruction_order_##ORDER) o) noexcept -> value_type { \
          return get_timestamp(o);                                        \
      }
__UTL_DEFINE_GET_TIME(relaxed);
__UTL_DEFINE_GET_TIME(acquire);
__UTL_DEFINE_GET_TIME(release);
__UTL_DEFINE_GET_TIME(acq_rel);
__UTL_DEFINE_GET_TIME(seq_cst);

#  undef __UTL_DEFINE_GET_TIME
} // namespace tempus

UTL_NAMESPACE_END

#endif // UTL_ARCH_x86_64
