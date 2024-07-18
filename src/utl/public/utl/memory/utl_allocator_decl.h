// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_config.h"

#include "utl/exception/utl_exception_base.h"
#include "utl/memory/utl_allocator_fwd.h"
#include "utl/type_traits/utl_constants.h"

UTL_NAMESPACE_BEGIN
template <typename T>
class allocator {
public:
    using value_type = T;
    using size_type = size_t;
    using difference_type = decltype((char*)(0) - (char*)(0));
    using propagate_on_container_move_assignment = true_type;

private:
    using pointer = value_type*;
    using result_type = allocation_result<pointer, size_t>;

public:
    constexpr allocator() noexcept = default;
    constexpr allocator(allocator const&) noexcept = default;
    UTL_CONSTEXPR_CXX14 allocator& operator=(allocator const&) noexcept = default;

    template <typename U>
    constexpr allocator(allocator<U> const&) noexcept {}
    template <typename U>
    UTL_CONSTEXPR_CXX14 allocator& operator=(allocator<U> const&) noexcept {
        return *this;
    }

    UTL_NODISCARD UTL_CONSTEXPR_CXX20 pointer allocate(size_type count) UTL_THROWS;

    UTL_NODISCARD UTL_CONSTEXPR_CXX20 result_type allocate_at_least(size_type count) UTL_THROWS {
        return {allocate(count), count};
    }

    UTL_CONSTEXPR_CXX20 void deallocate(pointer pointer, size_type count) noexcept;

    UTL_CONSTEXPR_CXX20 ~allocator() noexcept = default;
};
UTL_NAMESPACE_END
