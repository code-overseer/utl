// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_config.h"

#include "utl/compare/utl_pointer_comparable.h"
#include "utl/concepts.h" // convertible_to
#include "utl/memory/utl_addressof.h"

#if UTL_WITH_EXCEPTIONS
#  include "utl/exception/utl_program_exception.h"
#else
#  include "utl/exception/utl_exception_base.h"
#endif

UTL_NAMESPACE_BEGIN

/**
 * Observer pointer class (non-owning) representing a non-null pointer.
 *
 * This class represents a pointer that ensures its underlying pointer is always non-null.
 * It is intended to be used in scenarios where null pointers are not allowed or indicate an error.
 *
 * @tparam T The type of the pointer's target.
 */
template <typename T>
class UTL_PUBLIC_TEMPLATE nonnull_ptr : private pointer_comparable<nonnull_ptr<T>> {
public:
    /**
     * Deleted default constructor to enforce initialization with a non-null pointer.
     */
    constexpr nonnull_ptr() noexcept = delete;
    constexpr nonnull_ptr(decltype(nullptr)) = delete;

    /**
     * Constructs a nonnull_ptr from a raw pointer.
     *
     * @param ptr A pointer to the object. It must be non-null.
     *
     * @throws utl::invalid_argument If the pointer is null and exceptions are enabled
     */
    UTL_HIDE_FROM_ABI UTL_CONSTEXPR_CXX14 nonnull_ptr(T* ptr) UTL_THROWS : ptr_(ptr) {
        UTL_THROW_IF(ptr == nullptr,
            invalid_argument(UTL_MESSAGE_FORMAT("[UTL] nonnull_ptr construction failed, "
                                                "Reason=[Pointer argument cannot be null]")));
    }

    template <UTL_CONCEPT_CXX20(convertible_to<T*>) U UTL_REQUIRES_CXX11(
        is_convertible<U, T*>::value)>
    UTL_HIDE_FROM_ABI UTL_CONSTEXPR_CXX14 nonnull_ptr(U&& obj) UTL_THROWS : nonnull_ptr((T*)obj) {}

    /**
     * Constructs a nonnull_ptr from a reference.
     *
     * @param ref A reference to the object.
     */
    UTL_HIDE_FROM_ABI constexpr explicit nonnull_ptr(T& ref) noexcept
        : ptr_(UTL_SCOPE addressof(ref)) {}

    /**
     * Conversion operator to implicitly convert nonnull_ptr to the underlying raw pointer.
     */
    UTL_ATTRIBUTES(HIDE_FROM_ABI, FLATTEN, PURE) constexpr operator T*() const {
        return get();
    }

    /**
     * Default copy operation
     */
    constexpr nonnull_ptr(nonnull_ptr const&) noexcept = default;
    UTL_CONSTEXPR_CXX14 nonnull_ptr& operator=(nonnull_ptr const&) noexcept = default;

    /**
     * Dereference operators to access the object pointed to by the nonnull_ptr.
     */
    UTL_ATTRIBUTES(HIDE_FROM_ABI, NODISCARD, PURE) constexpr T* operator->() const noexcept {
        return ptr_;
    }
    UTL_ATTRIBUTES(HIDE_FROM_ABI, NODISCARD, PURE) constexpr T& operator*() const noexcept { return *ptr_; }

    /**
     * Returns the underlying raw pointer.
     */
    UTL_ATTRIBUTES(HIDE_FROM_ABI, ALWAYS_INLINE, PURE) constexpr T* get() const noexcept {
        return UTL_BUILTIN_assume(ptr_ != nullptr), ptr_;
    }

    /**
     * Conversion operator to convert nonnull_ptr to boolean indicating non-nullness.
     */
    UTL_ATTRIBUTES(HIDE_FROM_ABI, ALWAYS_INLINE, CONST) constexpr explicit operator bool() const noexcept {
        return true;
    }

private:
    T* ptr_;
};

#if UTL_CXX17
template <typename T>
explicit nonnull_ptr(T* p) -> nonnull_ptr<T>;
#endif

UTL_NAMESPACE_END
