// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_config.h"

#include "utl/preprocessor/utl_assertion.h"

#include "utl/memory/utl_allocator_fwd.h"

#include "utl/exception/utl_exception_base.h"
#include "utl/type_traits/utl_constants.h"
#include "utl/type_traits/utl_is_complete.h"
#include "utl/type_traits/utl_type_identity.h"

#include <cstddef>
#include <new>

UTL_NAMESPACE_BEGIN
template <typename T>
class allocator {
public:
    using value_type = T;
    using size_type = size_t;
    using difference_type = decltype((char*)(0) - (char*)(0));
    using propagate_on_container_move_assignment = true_type;

private:
    using pointer UTL_NODEBUG = value_type*;
    using result_type UTL_NODEBUG = allocation_result<pointer, size_t>;

public:
    UTL_HIDE_FROM_ABI constexpr allocator() noexcept = default;
    UTL_HIDE_FROM_ABI constexpr allocator(allocator const&) noexcept = default;
    UTL_HIDE_FROM_ABI UTL_CONSTEXPR_CXX14 allocator& operator=(allocator const&) noexcept = default;

    template <typename U>
    UTL_HIDE_FROM_ABI constexpr allocator(allocator<U> const&) noexcept {}
    template <typename U>
    UTL_HIDE_FROM_ABI UTL_CONSTEXPR_CXX14 allocator& operator=(allocator<U> const&) noexcept {
        return *this;
    }

    UTL_ATTRIBUTES(NODISCARD, HIDE_FROM_ABI) inline UTL_CONSTEXPR_CXX20 pointer allocate(size_type count) UTL_THROWS;

    UTL_ATTRIBUTES(NODISCARD, HIDE_FROM_ABI) inline UTL_CONSTEXPR_CXX20 result_type allocate_at_least(
        size_type count) UTL_THROWS {
        return {allocate(count), count};
    }

    UTL_HIDE_FROM_ABI UTL_CONSTEXPR_CXX20 void deallocate(pointer pointer, size_type count) noexcept;

    UTL_HIDE_FROM_ABI UTL_CONSTEXPR_CXX20 ~allocator() noexcept = default;
};
UTL_NAMESPACE_END

#if UTL_HAS_BUILTIN(__builtin_operator_new) && UTL_HAS_BUILTIN(__builtin_operator_delete)
#  define __UTL_ALLOC_NEW(...) __builtin_operator_new(__VA_ARGS__)
#  define __UTL_ALLOC_DELETE(...) __builtin_operator_delete(__VA_ARGS__)
#else
#  define __UTL_ALLOC_NEW(...) ::operator new(__VA_ARGS__)
#  define __UTL_ALLOC_DELETE(...) ::operator delete(__VA_ARGS__)
#endif

UTL_NAMESPACE_BEGIN
using ::std::align_val_t;

namespace memory {
template <typename T>
struct max_size : integral_constant<size_t, size_t(-1) / sizeof(T)> {};
template <>
struct max_size<void> : integral_constant<size_t, size_t(-1)> {};
namespace details {
#ifdef UTL_DEFAULT_NEW_ALIGNMENT
UTL_INLINE_CXX17 constexpr size_t default_new_alignment = UTL_DEFAULT_NEW_ALIGNMENT;
#else
UTL_INLINE_CXX17 constexpr size_t default_new_alignment = alignof(max_align_t);
#endif

UTL_ATTRIBUTES(HIDE_FROM_ABI, ALWAYS_INLINE, CONST) inline constexpr bool is_overaligned_for_new(
    size_t alignment) noexcept {
    return alignment > default_new_alignment;
}

UTL_HIDE_FROM_ABI inline void deallocate(
    void* pointer, size_t size, size_t alignment = default_new_alignment) noexcept {
    if (is_overaligned_for_new(alignment)) {
#if UTL_SUPPORTS_ALIGNED_ALLOCATION
        align_val_t const align_val = static_cast<align_val_t>(alignment);
#  if UTL_SUPPORTS_SIZED_DEALLOCATION
        __UTL_ALLOC_DELETE(pointer, size, align_val);
#  else
        __UTL_ALLOC_DELETE(pointer, align_val);
#  endif
#else
        UTL_ASSERT(false, "Allocation alignment is too strict");
        UTL_BUILTIN_unreachable();
#endif
    } else {
        (void)alignment;
#if UTL_SUPPORTS_SIZED_DEALLOCATION
        __UTL_ALLOC_DELETE(pointer, size);
#else
        (void)size;
        __UTL_ALLOC_DELETE(pointer);
#endif
    }
}

UTL_HIDE_FROM_ABI inline void* allocate(size_t size, size_t alignment = default_new_alignment) {
    if (is_overaligned_for_new(alignment)) {
#if UTL_SUPPORTS_ALIGNED_ALLOCATION
        align_val_t const align_val = static_cast<align_val_t>(alignment);
        return __UTL_ALLOC_NEW(size, align_val);
#else
        UTL_ASSERT(false, "Allocation alignment is too strict");
        UTL_BUILTIN_unreachable();
#endif
    }
    (void)alignment;
    return __UTL_ALLOC_NEW(size);
}
} // namespace details

namespace runtime {
template <typename T>
UTL_HIDE_FROM_ABI T* allocate(size_t count) {
    return static_cast<T*>(details::allocate(count * sizeof(T), alignof(T)));
}

template <typename T>
UTL_HIDE_FROM_ABI void deallocate(typename type_identity<T>::type* ptr, size_t count) noexcept {
    details::deallocate(ptr, count * sizeof(T), alignof(T));
}
} // namespace runtime

namespace compile_time {
#if UTL_CXX20
template <typename T>
inline constinit ::std::allocator<T> allocator_v = {};

template <typename T>
UTL_HIDE_FROM_ABI constexpr T* allocate(size_t count) {
    if (UTL_BUILTIN_is_constant_evaluated()) {
        if constexpr (UTL_TRAIT_is_complete(::std::allocator<T>)) {
            return allocator_v<T>.allocate(count);
        } else {
            extern void constant_evaluation_failure();
            constant_evaluation_failure(); // TODO remove when UTL_ASSERT is defined
            UTL_ASSERT(false,
                "Constant evaluated allocation failure, please include "
                "the standard <memory> header");
            UTL_BUILTIN_unreachable();
        }
    }

    return runtime::allocate<T>(count);
}

template <typename T>
UTL_HIDE_FROM_ABI constexpr void deallocate(
    typename type_identity<T>::type* ptr, size_t count) noexcept {
    if (UTL_BUILTIN_is_constant_evaluated()) {
        if constexpr (UTL_TRAIT_is_complete(::std::allocator<T>)) {
            allocator_v<T>.deallocate(ptr, count);
        } else {
            extern void constant_evaluation_failure();
            constant_evaluation_failure(); // TODO remove when UTL_ASSERT is defined
            UTL_ASSERT(false,
                "Constant evaluated allocation failure, please include "
                "the standard <memory> header");
            UTL_BUILTIN_unreachable();
        }
    } else {
        return runtime::deallocate<T>(ptr, count);
    }
}
#endif
} // namespace compile_time
template <typename T>
UTL_HIDE_FROM_ABI UTL_CONSTEXPR_CXX20 void deallocate(
    typename type_identity<T>::type* ptr, size_t count) noexcept {
    static_assert(sizeof(T) > 0, "Incomplete type cannot be deallocated");
#if UTL_CXX20
    compile_time::deallocate<T>(ptr, count);
#else
    runtime::deallocate<T>(ptr, count);
#endif
}

template <typename T>
UTL_ATTRIBUTES(MALLOC, NODISCARD, HIDE_FROM_ABI)
UTL_CONSTEXPR_CXX20 T* allocate(size_t count) {
    static_assert(sizeof(T) > 0, "Incomplete type cannot be allocated");
#if UTL_CXX20
    return compile_time::allocate<T>(count);
#else
    return runtime::allocate<T>(count);
#endif
}
} // namespace memory

UTL_NAMESPACE_END

#undef __UTL_ALLOC_DELETE
#undef __UTL_ALLOC_NEW
