// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

#include "utl/memory/utl_addressof.h"
#include "utl/type_traits/utl_enable_if.h"
#include "utl/type_traits/utl_is_array.h"

UTL_NAMESPACE_BEGIN

#if UTL_CXX17

template <typename T>
__UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX20 void destroy_at(T* ptr) noexcept {
    if constexpr (UTL_TRAIT_is_array(T)) {
        for (auto& element : *ptr) {
            (destroy_at)(__UTL addressof(element));
        }
    } else {
        ptr->~T();
    }
}

#else

template <typename T>
__UTL_HIDE_FROM_ABI inline enable_if_t<UTL_TRAIT_is_array(T)> destroy_at(T* ptr) noexcept {
    for (auto& element : *ptr) {
        (destroy_at)(__UTL addressof(element));
    }
}

template <typename T>
__UTL_HIDE_FROM_ABI inline enable_if_t<!UTL_TRAIT_is_array(T)> destroy_at(T* ptr) noexcept {
    ptr->~T();
}

#endif

UTL_NAMESPACE_END
