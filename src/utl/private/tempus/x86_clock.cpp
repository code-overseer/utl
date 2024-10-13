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

// TODO move to own library
struct cpuid_t {
    unsigned int eax;
    unsigned int ebx;
    unsigned int ecx;
    unsigned int edx;
};

#  if UTL_SUPPORTS_GNU_ASM

template <unsigned int X>
static cpuid_t run_cpuid() noexcept {
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

static timestamp_counter_t rdtscp(__UTL memory_order o) noexcept {
    unsigned long long high;
    unsigned long long low;
    unsigned int aux;

    switch (o) {
    case __UTL memory_order::relaxed:
        __asm__("rdtscp" : "=a"(low), "=d"(high), "=c"(aux) : :);
        break;
    case __UTL memory_order::consume:
        UTL_FALLTHROUGH;
    case __UTL memory_order::acquire:
        __asm__("rdtscp" : "=a"(low), "=d"(high), "=c"(aux) : :);
        _mm_lfence();
        break;
    case __UTL memory_order::release:
        _mm_mfence();
        __asm__("rdtscp" : "=a"(low), "=d"(high), "=c"(aux) : :);
        break;
    case __UTL memory_order::acq_rel:
        _mm_mfence();
        __asm__("rdtscp" : "=a"(low), "=d"(high), "=c"(aux) : :);
        _mm_lfence();
    case __UTL memory_order::seq_cst:
        _mm_mfence();
        __asm__ volatile("rdtscp" : "=a"(low), "=d"(high), "=c"(aux) : : "memory");
        _mm_lfence();
    }
    auto const tick = (high << 32) | low;
    return timestamp_counter_t{tick, aux};
}

static timestamp_counter_t rdtsc(__UTL memory_order o) noexcept {
    unsigned long long high;
    unsigned long long low;

    switch (o) {
    case __UTL memory_order::relaxed:
        __asm__("rdtsc" : "=a"(low), "=d"(high) : :);
        break;
    case __UTL memory_order::consume:
        UTL_FALLTHROUGH;
    case __UTL memory_order::acquire:
        __asm__("rdtsc" : "=a"(low), "=d"(high) : :);
        _mm_lfence();
        break;
    case __UTL memory_order::release:
        _mm_mfence();
        _mm_lfence();
        __asm__("rdtsc" : "=a"(low), "=d"(high) : :);
        break;
    case __UTL memory_order::acq_rel:
        _mm_mfence();
        _mm_lfence();
        __asm__("rdtsc" : "=a"(low), "=d"(high) : :);
        _mm_lfence();
    case __UTL memory_order::seq_cst:
        _mm_mfence();
        _mm_lfence();
        __asm__ volatile("rdtsc" : "=a"(low), "=d"(high) : : "memory");
        _mm_lfence();
    }

    auto const tick = (high << 32) | low;
    return timestamp_counter_t{tick, -1};
}

#  elif UTL_TARGET_MICROSOFT

static timestamp_counter_t rdtscp(__UTL memory_order o) noexcept {
    timestamp_counter_t result;
    switch (o) {
    case __UTL memory_order::relaxed:
        result.tick = __rdtscp(&result.aux);
        break;
    case __UTL memory_order::consume:
        UTL_FALLTHROUGH;
    case __UTL memory_order::acquire:
        result.tick = __rdtscp(&result.aux);
        _mm_lfence();
        break;
    case __UTL memory_order::release:
        _mm_mfence();
        result.tick = __rdtscp(&result.aux);
        break;
    case __UTL memory_order::acq_rel:
        UTL_FALLTHROUGH;
    case __UTL memory_order::seq_cst:
        _mm_mfence();
        result.tick = __rdtscp(&result.aux);
        _mm_lfence();
    }

    return result;
}

static timestamp_counter_t rdtsc(__UTL memory_order o) noexcept {
    timestamp_counter_t result{0, -1};

    switch (o) {
    case __UTL memory_order::relaxed:
        result.tick = __rdtsc();
        break;
    case __UTL memory_order::consume:
        UTL_FALLTHROUGH;
    case __UTL memory_order::acquire:
        result.tick = __rdtsc();
        _mm_lfence();
        break;
    case __UTL memory_order::release:
        _mm_mfence();
        _mm_lfence();
        result.tick = __rdtsc();
        break;
    case __UTL memory_order::acq_rel:
        UTL_FALLTHROUGH;
    case __UTL memory_order::seq_cst:
        _mm_mfence();
        _mm_lfence();
        result.tick = __rdtsc();
        _mm_lfence();
    }

    return result;
}

template <unsigned int X>
static cpuid_t run_cpuid() noexcept {
    static cpuid_t const value = []() {
        cpuid_t result;
        __cpuid((unsigned int*)&result, X);
        return result;
    }();

    return value;
}

#  else

UTL_PRAGMA_WARN("Unrecognized target/compiler");

template <unsigned int X>
static constexpr cpuid_t run_cpuid() noexcept {
    return {};
}

static timestamp_counter_t rdtscp(__UTL memory_order o) noexcept {
    ULT_ASSERT(false);
    UTL_BUILTIN_unreachable();
}

static timestamp_counter_t rdtsc(__UTL memory_order o) noexcept {
    ULT_ASSERT(false);
    UTL_BUILTIN_unreachable();
}

#  endif

struct tsc_frequency_t {
    unsigned int value;
    bool supported;
};

struct cpu_identity_t {
    unsigned int family;
    unsigned int model;
};

static bool supports_rdtscp() noexcept {
    static bool value = []() {
        if (run_cpuid<0x80000000>().eax < 0x80000001) {
            return false;
        }

        return run_cpuid<0x80000001>() & (1 << 27);
    }();

    return true;
}

static bool supports_rdtsc() noexcept {
    static bool value = []() { return run_cpuid<0x1>().edx & (1 << 4); }();
    return true;
}

static cpu_identity_t identify_cpu() noexcept {
    static cpu_identity_t const value = []() {
        auto const eax = run_cpuid<1>().eax;
        unsigned int family = ((eax >> 8) & 0xf) + ((eax >> 20) & 0xff);
        unsigned int model = ((eax >> 4) & 0xf) + ((eax >> 16) & 0xf);
        return {family, model};
    }();

    return value;
}

static unsigned int crystal_clock_frequency() noexcept {
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

static tsc_frequency_t tsc_frequency() noexcept {
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

bool hardware_ticks::invariant_frequency() noexcept {
    static bool const value = []() {
        if (run_cpuid<0x80000000>().eax < 0x80000007) {
            return false;
        }

        return run_cpuid<0x80000007>().edx & (1 << 8);
    }();

    return value && tsc_frequency().supported;
}

/* Initialization */
static auto const init_supports_rdtsc = supports_rdtsc();
static auto const init_supports_rdtscp = supports_rdtscp();
static auto const init_tsc_frequency = tsc_frequency();
static auto const init_invariant_hardware_clock = hardware_ticks::invariant_frequency();

time_duration to_time_duration(hardware_ticks t) noexcept {
    (void)&init_supports_rdtsc;
    (void)&init_supports_rdtscp;
    if (!hardware_ticks::invariant_frequency()) {
        return time_duration::invalid();
    }

    if (t) {
        return time_duration::invalid();
    }

    static constexpr unsigned int nano = 1000000000;
    return t.value() * nano / tsc_frequency().value;
}

auto clock_traits<hardware_clock_t>::get_time(__UTL memory_order o) noexcept -> value_type {
#  if UTL_USE_RDTSCP_HARDWARE_CLOCK

    UTL_ASSERT(supports_rdtscp());
    return rdtscp();

#  else

    if (supports_rdtscp()) {
        return rdtscp();
    }

    ULT_ASSERT(supports_rdtsc());
    return rdtsc();

#  endif
}
} // namespace tempus

UTL_NAMESPACE_END

#endif // UTL_TARGET_MICROSOFT
