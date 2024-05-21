// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/concepts/utl_complete.h"
#include "utl/concepts/utl_implicit_lifetime.h"
#include "utl/preprocessor/utl_config.h"
#include "utl/type_traits/utl_enable_if.h"

#include <new>

UTL_NAMESPACE_BEGIN
#if UTL_COMPILER_GCC
#  define UTL_START_LIFETIME_OPTIMIZE __attribute__((optimize("-O3")))
#endif
#ifndef UTL_START_LIFETIME_OPTIMIZE
#  define UTL_START_LIFETIME_OPTIMIZE
#endif

#define UTL_START_LIFETIME_ATTRIBUTES \
    UTL_ATTRIBUTES(NODISCARD, ALWAYS_INLINE) UTL_START_LIFETIME_OPTIMIZE

#if !UTL_COMPILER_CLANG && !UTL_COMPILER_ICC && !UTL_COMPILER_ICX
#  define UTL_STRICT_LIFETIME_IMPLEMENTATION 1
#endif

namespace details {
namespace lifetime {
/**
 * Generates a superposition of all posible implicit lifetime types of size n with alignment
 * given by pointer `p`
 *
 * @param p - pointer to memory location to entangle
 * @param n - size of implicit lifetime object
 */
UTL_START_LIFETIME_ATTRIBUTES inline void* entangle_memory(void* p, size_t n) noexcept {
    using byte_type = unsigned char;
#if UTL_STRICT_LIFETIME_IMPLEMENTATION
    /* UTL_UNDEFINED_BEHAVIOUR */
    // May be undefined behaviour if p points to a const object with automatic, static or
    // thread-local storage
    // This is "more" defined than placement new byte array due to indeterminate value access in
    // the alternative
    return ::memmove(p, p, n);
#else
    /* UTL_UNDEFINED_BEHAVIOUR */
    // technically results in undefined behaviour since bytes have indeterminate value
    return ::new (p) byte_type[n];
#endif
}

/**
 * Entangles the given storage location and collapses the resulting superposition of
 * implicit types to type `T`
 *
 * @tparam T - implicit lifetime type to collapse as
 * @param p - storage location in which to (re-)start the lifetime of type T
 */
template <typename T>
UTL_START_LIFETIME_ATTRIBUTES inline T* collapse_as(void* p) noexcept {
    auto bytes = entangle_memory(p, sizeof(T));
    auto started = reinterpret_cast<T*>(bytes);
    // Dereferecing a pointer is only valid if the pointer actually points to an object within its
    // lifetime
    (void)*started; // collapse wave-function by dereferencing
    return started;
}

/**
 * Entangles the given memory location and collapses the resulting superposition of
 * implicit types to an array of type `T`.
 *
 * @note `T` need not be an implicit lifetime type, but if it is not an implicit lifetime type, the
 * lifetime of the element will not start without an explicit invocation (e.g. placement-new); If
 * `T` is an implicit lifetime type, the lifetime will be started if the array elements are
 * subsequently accessed.
 *
 * @note It is undefined behaviour to pass a nullptr if the array size is greater than zero
 * @note The underlying storage is untouched if the array size is 0
 *
 * @tparam T - element type of the array to collapse as
 * @param p - storage location in which to (re-)start the lifetime of type T[n]
 */
template <typename T>
UTL_START_LIFETIME_ATTRIBUTES inline T* collapse_as_array(void* p, size_t n) noexcept {
    if (!n) {
        return reinterpret_cast<T*>(p);
    }

    auto bytes = entangle_memory(p, sizeof(T) * n);
    if (n == 1) {
        return *reinterpret_cast<T(*)[1]>(bytes); // collapse wave-function by treating as T[1]
    }

    auto started = reinterpret_cast<T*>(bytes);
    // pointer arithmetic is only valid if pointer is a pointer to an array element
    return (started + n), started; // collapse wave-function by treating as array
}
} // namespace lifetime
} // namespace details

#undef UTL_START_LIFETIME_OPTIMIZE
#undef UTL_START_LIFETIME_ATTRIBUTES
#define UTL_START_LIFETIME_ATTRIBUTES UTL_ATTRIBUTES(NODISCARD, ALWAYS_INLINE)

template <UTL_CONCEPT_CXX20(implicit_lifetime) T>
UTL_START_LIFETIME_ATTRIBUTES inline UTL_ENABLE_IF_CXX11(T*, UTL_TRAIT_is_implicit_lifetime(T))
    start_lifetime_as(void* p) noexcept {
    return details::lifetime::collapse_as<T>(p);
}

template <UTL_CONCEPT_CXX20(implicit_lifetime) T>
UTL_START_LIFETIME_ATTRIBUTES inline UTL_ENABLE_IF_CXX11(
    T const*, UTL_TRAIT_is_implicit_lifetime(T)) start_lifetime_as(void const* p) noexcept {
    return details::lifetime::collapse_as<T const>(const_cast<void*>(p));
}

template <UTL_CONCEPT_CXX20(implicit_lifetime) T>
UTL_START_LIFETIME_ATTRIBUTES inline UTL_ENABLE_IF_CXX11(
    T volatile*, UTL_TRAIT_is_implicit_lifetime(T)) start_lifetime_as(void volatile* p) noexcept {
    return details::lifetime::collapse_as<T volatile>(const_cast<void*>(p));
}

template <UTL_CONCEPT_CXX20(implicit_lifetime) T>
UTL_START_LIFETIME_ATTRIBUTES inline UTL_ENABLE_IF_CXX11(T const volatile*,
    UTL_TRAIT_is_implicit_lifetime(T)) start_lifetime_as(void const volatile* p) noexcept {
    return details::lifetime::collapse_as<T const volatile>(const_cast<void*>(p));
}

template <UTL_CONCEPT_CXX20(complete) T>
UTL_START_LIFETIME_ATTRIBUTES inline UTL_ENABLE_IF_CXX11(T*, UTL_TRAIT_is_complete(T))
    start_lifetime_as_array(void* p, size_t n) noexcept {
    return details::lifetime::collapse_as_array<T>(const_cast<void*>(p), n);
}

template <UTL_CONCEPT_CXX20(complete) T>
UTL_START_LIFETIME_ATTRIBUTES inline UTL_ENABLE_IF_CXX11(T const*, UTL_TRAIT_is_complete(T))
    start_lifetime_as_array(void const* p, size_t n) noexcept {
    return details::lifetime::collapse_as_array<T const>(const_cast<void*>(p), n);
}

template <UTL_CONCEPT_CXX20(complete) T>
UTL_START_LIFETIME_ATTRIBUTES inline UTL_ENABLE_IF_CXX11(T volatile*, UTL_TRAIT_is_complete(T))
    start_lifetime_as_array(void volatile* p, size_t n) noexcept {
    return details::lifetime::collapse_as_array<T volatile>(const_cast<void*>(p), n);
}

template <UTL_CONCEPT_CXX20(complete) T>
UTL_START_LIFETIME_ATTRIBUTES inline UTL_ENABLE_IF_CXX11(T const volatile*,
    UTL_TRAIT_is_complete(T)) start_lifetime_as_array(void const volatile* p, size_t n) noexcept {
    return details::lifetime::collapse_as_array<T const volatile>(const_cast<void*>(p), n);
}

#undef UTL_START_LIFETIME_ATTRIBUTES
UTL_NAMESPACE_END
