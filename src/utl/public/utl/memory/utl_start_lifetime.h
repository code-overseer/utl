// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/concepts/utl_complete.h"
#include "utl/concepts/utl_implicit_lifetime.h"
#include "utl/preprocessor/utl_config.h"
#include "utl/type_traits/utl_enable_if.h"

#include <new>

UTL_NAMESPACE_BEGIN

#define UTL_START_LIFETIME_ATTRIBUTES UTL_ATTRIBUTES(NODISCARD, ALWAYS_INLINE)

namespace details {
namespace lifetime {
template <typename T>
UTL_START_LIFETIME_ATTRIBUTES inline T* start(void* p) noexcept {
    using byte_type = unsigned char;
    auto bytes = ::new (p) byte_type[sizeof(T)];
    auto started = reinterpret_cast<T*>(bytes);
    (void)*started; // collapse the wave-function
    return started;
}

template <typename T>
UTL_START_LIFETIME_ATTRIBUTES inline T* start_array(void* p, size_t n) noexcept {
    if (!n) {
        return reinterpret_cast<T*>(p);
    }

    using byte_type = unsigned char;
    auto bytes = ::new (p) byte_type[sizeof(T) * n];
    if (n == 1) {
        auto started = reinterpret_cast<T(*)[1]>(bytes);
        return *started; // collapse wave-function by treating as array
    }

    auto started = reinterpret_cast<T*>(bytes);
    // pointer arithmetic is only defined if pointer is a pointer to an array element
    (void)(started + n); // collapse wave-function by treating as array
    return started;
}
} // namespace lifetime
} // namespace details

template <UTL_CONCEPT_CXX20(implicit_lifetime) T>
UTL_START_LIFETIME_ATTRIBUTES inline UTL_ENABLE_IF_CXX11(T*, UTL_TRAIT_is_implicit_lifetime(T))
    start_lifetime_as(void* p) noexcept {
    return details::lifetime::start<T>(p);
}

template <UTL_CONCEPT_CXX20(implicit_lifetime) T>
UTL_START_LIFETIME_ATTRIBUTES inline UTL_ENABLE_IF_CXX11(
    T const*, UTL_TRAIT_is_implicit_lifetime(T)) start_lifetime_as(void const* p) noexcept {
    return details::lifetime::start<T const>(const_cast<void*>(p));
}

template <UTL_CONCEPT_CXX20(implicit_lifetime) T>
UTL_START_LIFETIME_ATTRIBUTES inline UTL_ENABLE_IF_CXX11(
    T volatile*, UTL_TRAIT_is_implicit_lifetime(T)) start_lifetime_as(void volatile* p) noexcept {
    return details::lifetime::start<T volatile>(const_cast<void*>(p));
}

template <UTL_CONCEPT_CXX20(implicit_lifetime) T>
UTL_START_LIFETIME_ATTRIBUTES inline UTL_ENABLE_IF_CXX11(T const volatile*,
    UTL_TRAIT_is_implicit_lifetime(T)) start_lifetime_as(void const volatile* p) noexcept {
    return details::lifetime::start<T const volatile>(const_cast<void*>(p));
}

template <UTL_CONCEPT_CXX20(complete) T>
UTL_START_LIFETIME_ATTRIBUTES inline UTL_ENABLE_IF_CXX11(T*, UTL_TRAIT_is_complete(T))
    start_lifetime_as_array(void* p, size_t n) noexcept {
    return details::lifetime::start_array<T>(const_cast<void*>(p), n);
}

template <UTL_CONCEPT_CXX20(complete) T>
UTL_START_LIFETIME_ATTRIBUTES inline UTL_ENABLE_IF_CXX11(T const*, UTL_TRAIT_is_complete(T))
    start_lifetime_as_array(void const* p, size_t n) noexcept {
    return details::lifetime::start_array<T const>(const_cast<void*>(p), n);
}

template <UTL_CONCEPT_CXX20(complete) T>
UTL_START_LIFETIME_ATTRIBUTES inline UTL_ENABLE_IF_CXX11(T volatile*, UTL_TRAIT_is_complete(T))
    start_lifetime_as_array(void volatile* p, size_t n) noexcept {
    return details::lifetime::start_array<T volatile>(const_cast<void*>(p), n);
}

template <UTL_CONCEPT_CXX20(complete) T>
UTL_START_LIFETIME_ATTRIBUTES inline UTL_ENABLE_IF_CXX11(T const volatile*,
    UTL_TRAIT_is_complete(T)) start_lifetime_as_array(void const volatile* p, size_t n) noexcept {
    return details::lifetime::start_array<T const volatile>(const_cast<void*>(p), n);
}

#undef UTL_START_LIFETIME_ATTRIBUTES

UTL_NAMESPACE_END
