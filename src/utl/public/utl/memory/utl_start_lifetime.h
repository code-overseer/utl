// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/concepts/utl_complete.h"
#include "utl/concepts/utl_implicit_lifetime.h"
#include "utl/preprocessor/utl_config.h"
#include "utl/type_traits/utl_enable_if.h"

#include <new>

UTL_NAMESPACE_BEGIN

#define UTL_START_LIFETIME_ATTRIBUTES UTL_ATTRIBUTES(NODISCARD, ALWAYS_INLINE)

template <UTL_CONCEPT_CXX20(implicit_lifetime) T>
UTL_START_LIFETIME_ATTRIBUTES UTL_ENABLE_IF_CXX11(T*, UTL_TRAIT_is_implicit_lifetime(T))
    start_lifetime_as(void* p) noexcept {
    using byte_type = unsigned char;
    auto bytes = ::new (p) byte_type[sizeof(T)];
    auto started = reinterpret_cast<T*>(bytes);
    (void)*started;
    return started;
}

template <UTL_CONCEPT_CXX20(implicit_lifetime) T>
UTL_START_LIFETIME_ATTRIBUTES UTL_ENABLE_IF_CXX11(T const*, UTL_TRAIT_is_implicit_lifetime(T))
    start_lifetime_as(void const* p) noexcept {
    return start_lifetime_as(const_cast<void*>(p));
}

template <UTL_CONCEPT_CXX20(implicit_lifetime) T>
UTL_START_LIFETIME_ATTRIBUTES UTL_ENABLE_IF_CXX11(T volatile*, UTL_TRAIT_is_implicit_lifetime(T))
    start_lifetime_as(void volatile* p) noexcept {
    return start_lifetime_as(const_cast<void*>(p));
}

template <UTL_CONCEPT_CXX20(implicit_lifetime) T>
UTL_ATTRIBUTES(ALWAYS_INLINE)
UTL_ENABLE_IF_CXX11(T const volatile*, UTL_TRAIT_is_implicit_lifetime(T))
    start_lifetime_as(void const volatile* p) noexcept {
    return start_lifetime_as(const_cast<void*>(p));
}

template <UTL_CONCEPT_CXX20(complete) T>
UTL_START_LIFETIME_ATTRIBUTES UTL_ENABLE_IF_CXX11(T*, UTL_TRAIT_is_complete(T))
    start_lifetime_as_array(void* p, size_t n) noexcept {
    if (!n) {
        return reinterpret_cast<T*>(p);
    }

    using byte_type = unsigned char;
    auto bytes = ::new (p) byte_type[sizeof(T) * n];
    auto started = reinterpret_cast<T*>(bytes);
    (void)*started;
    return started;
}

template <UTL_CONCEPT_CXX20(complete) T>
UTL_START_LIFETIME_ATTRIBUTES UTL_ENABLE_IF_CXX11(T const*, UTL_TRAIT_is_complete(T))
    start_lifetime_as_array(void const* p, size_t n) noexcept {
    return start_lifetime_as_array(const_cast<void*>(p));
}

template <UTL_CONCEPT_CXX20(complete) T>
UTL_START_LIFETIME_ATTRIBUTES UTL_ENABLE_IF_CXX11(T volatile*, UTL_TRAIT_is_complete(T))
    start_lifetime_as_array(void volatile* p, size_t n) noexcept {
    return start_lifetime_as_array(const_cast<void*>(p));
}

template <UTL_CONCEPT_CXX20(complete) T>
UTL_START_LIFETIME_ATTRIBUTES UTL_ENABLE_IF_CXX11(T const volatile*, UTL_TRAIT_is_complete(T))
    start_lifetime_as_array(void const volatile* p, size_t n) noexcept {
    return start_lifetime_as_array(const_cast<void*>(p));
}

#undef UTL_START_LIFETIME_ATTRIBUTES

UTL_NAMESPACE_END
