// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/base_preprocessor.h"
#include "utl/memory/utl_addressof.h"

UTL_NAMESPACE_BEGIN

/**
 * Smart pointer class representing a non-null pointer.
 *
 * This class represents a smart pointer that ensures its underlying pointer is always non-null.
 * It is intended to be used in scenarios where null pointers are not allowed or indicate an error.
 *
 * @tparam T The type of the pointer's target.
 */
template <typename T>
class nonnull_ptr : private pointer_comparable<nonnull_ptr<T>> {
public:
    /**
     * Deleted default constructor to enforce initialization with a non-null pointer.
     */
    constexpr nonnull_ptr() noexcept = delete;

    /**
     * Constructs a nonnull_ptr from a raw pointer.
     *
     * @param ptr A pointer to the object. It must be non-null.
     */
    constexpr nonnull_ptr(T* ptr) noexcept : ptr_(ptr) { UTL_ASSERT(ptr_ != nullptr); }

    /**
     * Constructs a nonnull_ptr from a reference.
     *
     * @param ref A reference to the object.
     */
    constexpr explicit nonnull_ptr(T& ref) noexcept : ptr_(addressof(ref)) {}

    /**
     * Conversion operator to implicitly convert nonnull_ptr to the underlying raw pointer.
     */
    constexpr operator T*() const { return ptr_; }

    /**
     * Default copy and move operations.
     */
    constexpr nonnull_ptr(nonnull_ptr const&) noexcept = default;
    constexpr nonnull_ptr& operator=(nonnull_ptr const&) noexcept = default;
    constexpr nonnull_ptr(nonnull_ptr&&) noexcept = default;
    constexpr nonnull_ptr& operator=(nonnull_ptr&&) noexcept = default;

    /**
     * Dereference operators to access the object pointed to by the nonnull_ptr.
     */
    constexpr T* operator->() const noexcept { return ptr_; }
    constexpr T& operator*() const noexcept { return *ptr_; }

    /**
     * Returns the underlying raw pointer.
     */
    constexpr T* get() const noexcept { return ptr_; }

    /**
     * Conversion operator to convert nonnull_ptr to boolean indicating non-nullness.
     */
    constexpr explicit operator bool() const noexcept { return true; }

private:
    T* ptr_;
};

UTL_NAMESPACE_END
