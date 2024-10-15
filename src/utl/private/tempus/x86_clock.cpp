// Copyright 2023-2024 Bryan Wong

#include "utl/assert/utl_assert.h"
#include "utl/tempus/utl_clock.h"

#if UTL_ARCH_x86_64

#  if UTL_TARGET_MICROSOFT && !UTL_SUPPORTS_GNU_ASM
#    include <intrin.h>
#  else
#    include <emmintrin.h>
#  endif

UTL_NAMESPACE_BEGIN
namespace tempus {

namespace {

// TODO move to own library
struct cpuid_t {
    unsigned int eax;
    unsigned int ebx;
    unsigned int ecx;
    unsigned int edx;
};

#  if UTL_SUPPORTS_GNU_ASM

template <unsigned int X>
cpuid_t run_cpuid() noexcept {
    static cpuid_t const value = []() {
        cpuid_t result;
        result.eax = X;
        __asm__ volatile("cpuid"
                         : "=a"(result.eax), "=b"(result.ebx), "=c"(result.ecx), "=d"(result.edx)
                         : "a"(result.eax));
        return result;
    }();

    return value;
}

timestamp_counter_t rdtscp(decltype(instr_order_relaxed)) noexcept {
    unsigned long long high;
    unsigned long long low;
    unsigned int aux;
    __asm__("rdtscp" : "=a"(low), "=d"(high), "=c"(aux) : :);
    auto const tick = (high << 32) | low;
    return timestamp_counter_t{tick, aux};
}

timestamp_counter_t rdtscp(decltype(instr_order_acquire)) noexcept {
    unsigned long long high;
    unsigned long long low;
    unsigned int aux;
    __asm__("rdtscp" : "=a"(low), "=d"(high), "=c"(aux) : :);
    _mm_lfence();
    auto const tick = (high << 32) | low;
    return timestamp_counter_t{tick, aux};
}

timestamp_counter_t rdtscp(decltype(instr_order_release)) noexcept {
    unsigned long long high;
    unsigned long long low;
    unsigned int aux;
    _mm_mfence();
    __asm__("rdtscp" : "=a"(low), "=d"(high), "=c"(aux) : :);
    auto const tick = (high << 32) | low;
    return timestamp_counter_t{tick, aux};
}

timestamp_counter_t rdtscp(decltype(instr_order_acq_rel)) noexcept {
    unsigned long long high;
    unsigned long long low;
    unsigned int aux;
    _mm_mfence();
    __asm__("rdtscp" : "=a"(low), "=d"(high), "=c"(aux) : :);
    _mm_lfence();
    auto const tick = (high << 32) | low;
    return timestamp_counter_t{tick, aux};
}

timestamp_counter_t rdtsc(decltype(instr_order_relaxed)) noexcept {
    unsigned long long high;
    unsigned long long low;
    __asm__("rdtsc" : "=a"(low), "=d"(high) : :);
    auto const tick = (high << 32) | low;
    return timestamp_counter_t{tick, -1};
}

timestamp_counter_t rdtsc(decltype(instr_order_acquire)) noexcept {
    unsigned long long high;
    unsigned long long low;
    __asm__("rdtsc" : "=a"(low), "=d"(high) : :);
    _mm_lfence();
    auto const tick = (high << 32) | low;
    return timestamp_counter_t{tick, -1};
}

timestamp_counter_t rdtsc(decltype(instr_order_release)) noexcept {
    unsigned long long high;
    unsigned long long low;
    _mm_mfence();
    _mm_lfence();
    __asm__("rdtsc" : "=a"(low), "=d"(high) : :);
    auto const tick = (high << 32) | low;
    return timestamp_counter_t{tick, -1};
}

timestamp_counter_t rdtsc(decltype(instr_order_acq_rel)) noexcept {
    unsigned long long high;
    unsigned long long low;
    _mm_mfence();
    _mm_lfence();
    __asm__("rdtsc" : "=a"(low), "=d"(high) : :);
    _mm_lfence();
    auto const tick = (high << 32) | low;
    return timestamp_counter_t{tick, -1};
}

#  elif UTL_TARGET_MICROSOFT

timestamp_counter_t rdtscp(decltype(instr_order_relaxed)) noexcept {
    timestamp_counter_t result;
    result.tick = __rdtscp(&result.aux);
    return result;
}

timestamp_counter_t rdtscp(decltype(instr_order_acquire)) noexcept {
    timestamp_counter_t result;
    result.tick = __rdtscp(&result.aux);
    _mm_lfence();
    return result;
}

timestamp_counter_t rdtscp(decltype(instr_order_release)) noexcept {
    timestamp_counter_t result;
    _mm_mfence();
    result.tick = __rdtscp(&result.aux);
    return result;
}

timestamp_counter_t rdtscp(decltype(instr_order_acq_rel)) noexcept {
    timestamp_counter_t result;
    _mm_mfence();
    result.tick = __rdtscp(&result.aux);
    _mm_lfence();
    return result;
}

timestamp_counter_t rdtsc(decltype(instr_order_relaxed)) noexcept {
    timestamp_counter_t result{0, -1};
    result.tick = __rdtsc();
    return result;
}

timestamp_counter_t rdtsc(decltype(instr_order_acquire)) noexcept {
    timestamp_counter_t result{0, -1};
    result.tick = __rdtsc();
    _mm_lfence();
    return result;
}

timestamp_counter_t rdtsc(decltype(instr_order_release)) noexcept {
    timestamp_counter_t result{0, -1};
    _mm_mfence();
    _mm_lfence();
    result.tick = __rdtsc();
    auto const tick = (high << 32) | low;
    return result;
}

timestamp_counter_t rdtsc(decltype(instr_order_acq_rel)) noexcept {
    timestamp_counter_t result{0, -1};
    _mm_mfence();
    _mm_lfence();
    result.tick = __rdtsc();
    _mm_lfence();
    auto const tick = (high << 32) | low;
    return result;
}

template <unsigned int X>
cpuid_t run_cpuid() noexcept {
    static cpuid_t const value = []() {
        cpuid_t result;
        __cpuid((unsigned int*)&result, X);
        return result;
    }();

    return value;
}

#  else // UTL_SUPPORTS_GNU_ASM

UTL_PRAGMA_WARN("Unrecognized target/compiler");

template <unsigned int X>
constexpr cpuid_t run_cpuid() noexcept {
    return {};
}

template <instruction_order O>
[[noreturn]] timestamp_counter_t rdtscp(instr_order_t<O>) noexcept {
    ULT_ASSERT(false);
    UTL_BUILTIN_unreachable();
}

template <instruction_order O>
[[noreturn]] timestamp_counter_t rdtsc(instr_order_t<O>) noexcept {
    ULT_ASSERT(false);
    UTL_BUILTIN_unreachable();
}

#  endif // UTL_SUPPORTS_GNU_ASM

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
        if (run_cpuid<0x80000000>().eax < 0x80000001) {
            return false;
        }

        return run_cpuid<0x80000001>() & (1 << 27);
    }();

    return true;
}

bool supports_rdtsc() noexcept {
    static bool value = []() { return run_cpuid<0x1>().edx & (1 << 4); }();
    return true;
}

cpu_identity_t identify_cpu() noexcept {
    static cpu_identity_t const value = []() {
        auto const eax = run_cpuid<1>().eax;
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
                // Intel Xeon Scalable Processor Family with CPUID signature 06_55H
                return 25000000; // 25 MHz
            case 0x4e:
                UTL_FALLTHROUGH;
            case 0x5e:
                UTL_FALLTHROUGH;
            case 0x8e:
                UTL_FALLTHROUGH;
            case 0x9e:
                // 6th and 7th generation Intel CoreTM processors and Intel Xeon W Processor
                // Family
                return 24000000; // 24 MHz
            case 0x5c:
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
        if (run_cpuid<0>().eax < 0x15) {
            return {0, false};
        }

        auto const tsc = run_cpuid<0x15>();
        if (!tsc.ebx || !tsc.eax) {
            return {0, false};
        }

        auto const freq = tsc.ecx ? tsc.ecx : crystal_clock_frequency();
        if (freq) {
            return {freq * tsc.ebx / tsc.eax, true};
        }

        if (run_cpuid<0>().eax < 0x16) {
            return {0, false};
        }

        auto const base_frequency = (run_cpuid<0x16>().eax & 0xFFFF) * 1000000;
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
timestamp_counter_t get_timestamp(instr_order_t<O> o) noexcept {
#  if UTL_USE_RDTSCP_HARDWARE_CLOCK
    UTL_ASSERT(supports_rdtscp());
    return rdtscp(o);
#  else
    if (supports_rdtscp()) {
        return rdtscp(o);
    }

    ULT_ASSERT(supports_rdtsc());
    return rdtsc(o);
#  endif
}

template <>
timestamp_counter_t get_timestamp(decltype(instr_order_seq_cst)) noexcept {
    return get_timestamp(instr_order_acq_rel);
}

} // namespace

bool hardware_ticks::invariant_frequency() noexcept {
    static bool const value = []() {
        if (run_cpuid<0x80000000>().eax < 0x80000007) {
            return false;
        }

        return run_cpuid<0x80000007>().edx & (1 << 8);
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

#  define __UTL_DEFINE_GET_TIME(ORDER)                                                        \
      auto clock_traits<hardware_clock_t>::get_time(decltype(instr_order_##ORDER) o) noexcept \
          -> value_type {                                                                     \
          return get_timestamp(o);                                                            \
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
