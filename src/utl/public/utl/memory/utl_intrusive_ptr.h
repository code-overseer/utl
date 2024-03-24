// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/atomic.h"
#include "utl/base_preprocessor.h"
#include "utl/compare/utl_pointer_comparable.h"
#include "utl/exception.h"
#include "utl/memory/utl_addressof.h"
#include "utl/memory/utl_reference_counter.h"
#include "utl/type_traits/declval.h"
#include "utl/utility/utl_exchange.h"
#include "utl/utility/utl_forward.h"

UTL_NAMESPACE_BEGIN

UTL_INLINE_CXX17 constexpr class retain_object_t {
} retain_object = {};
UTL_INLINE_CXX17 constexpr class adopt_object_t {
} adopt_object = {};

/**
 * Smart pointer class for managing intrusive reference-counted objects.
 *
 * This class represents a smart pointer for managing intrusive reference-counted objects. It is
 * designed to work with objects that provide their own reference counting mechanism, often referred
 * to as "intrusive" reference counting. The class provides ownership semantics, allowing ownership
 * transfer between objects while ensuring proper reference counting.
 *
 * @tparam T The type of the managed object.
 */
template <typename T>
class intrusive_ptr : private pointer_comparable<intrusive_ptr<T>> {
    /**
     * Helper function to conditionally execute a function if the pointer is not null.
     *
     * @tparam F The type of the callable.
     * @param ptr The pointer to check.
     * @param func The callable to execute if the pointer is not null.
     *
     * @return The result of executing the callable.
     *
     * @throws utl::program_exception<void> - if ptr is null and exceptions are enabled
     */
    template <typename F>
    static UTL_CONSTEXPR_CXX14 auto iff_notnull(T* ptr, F&& func) noexcept(!utl::with_exceptions)
        -> decltype(declval<F>()((T*)nullptr)) {
        if (ptr != nullptr) {
            return forward<F>(func)(ptr);
        }
        UTL_THROW(utl::program_exception<void>(
            "[UTL] intrusive_ptr operation failed, Reason=[Unexpected nullptr argument]"));
    }

public:
    /**
     * Default constructor.
     * Constructs an intrusive_ptr with a null pointer.
     */
    constexpr intrusive_ptr() noexcept : resource_(nullptr) {}

    /**
     * Constructor to adopt an object.
     * Constructs an intrusive_ptr by adopting ownership of the provided object.
     *
     * @param adopt_object_t Tag type indicating adoption of ownership.
     * @param ptr The pointer to the object to adopt.
     *
     * @note Adoption transfers ownership without incrementing the reference count.
     *
     * @throws utl::program_exception<void> - if ptr is null and exceptions are enabled
     */
    UTL_CONSTEXPR_CXX14 intrusive_ptr(adopt_object_t, T* ptr) noexcept(!utl::with_exceptions)
        : resource_(iff_notnull(ptr, [](T* ptr) { return ptr; })) {}

    /**
     * Constructor to retain an object.
     * Constructs an intrusive_ptr by retaining ownership of the provided object.
     *
     * @param retain_object_t Tag type indicating retention of ownership.
     * @param ptr The pointer to the object to retain.
     *
     * @note Retention of ownership involves incrementing the reference count of the object.
     * @note The pointer to the object cannot be null
     *
     * @throws utl::program_exception<void> - if ptr is null and exceptions are enabled
     */
    UTL_CONSTEXPR_CXX14 intrusive_ptr(retain_object_t, T* ptr) noexcept(!utl::with_exceptions)
        : resource_(iff_notnull(ptr, [](T* ptr) {
            increment(*ptr);
            return ptr;
        })) {}

    /**
     * Copy constructor
     */
    UTL_CONSTEXPR_CXX14 intrusive_ptr(intrusive_ptr const& other) noexcept
        : resource_(other.resource_) {
        if (resource_ != nullptr) {
            increment(*resource_);
        }
    }

    /**
     * Copy assignment
     */
    UTL_CONSTEXPR_CXX14 intrusive_ptr& operator=(intrusive_ptr const& other) noexcept {
        if (addressof(other) != this) {
            reset();
            if (other) {
                increment(*other.resource_);
                resource_ = other.resource_;
            }
        }

        return *this;
    }

    /**
     * Move constructor
     */
    constexpr intrusive_ptr(intrusive_ptr&& other) noexcept
        : resource_(exchange(other.resource_, nullptr)) {}

    /**
     * Move assignment
     */
    UTL_CONSTEXPR_CXX14 intrusive_ptr& operator=(intrusive_ptr&& other) noexcept {
        if (addressof(other) != this) {
            reset();
            resource_ = exchange(other.resource_, nullptr);
        }
        return *this;
    }

    /**
     * Destructor
     */
    UTL_CONSTEXPR_CXX20 ~intrusive_ptr() noexcept { reset(); }

    /**
     * Dereference operator to access the object pointed to by the intrusive_ptr.
     */
    constexpr T* operator->() const noexcept { return resource_; }

    /**
     * Dereference operator to access the object pointed to by the intrusive_ptr.
     */
    constexpr T& operator*() const noexcept { return *resource_; }

    /**
     * Returns the underlying raw pointer.
     */
    constexpr T* get() const noexcept { return resource_; }

    /**
     * Releases ownership of the managed object.
     * Resets the intrusive_ptr to hold a null pointer and returns the previous raw pointer.
     */
    UTL_CONSTEXPR_CXX14 T* release() noexcept { return exchange(resource_, nullptr); }

    /**
     * Conversion operator to implicitly convert intrusive_ptr to boolean indicating non-nullness.
     */
    constexpr explicit operator bool() const noexcept { return resource_ != nullptr; }

    /**
     * Resets the intrusive_ptr to hold a null pointer, decrementing the reference count of the
     * current object if it's not null.
     */
    UTL_CONSTEXPR_CXX14 void reset() noexcept {
        if (resource_ != nullptr) {
            decrement(*resource_);
            resource_ = nullptr;
        }
    }

    /**
     * Resets the intrusive_ptr to hold the specified pointer, incrementing its reference count if
     * it's not null.
     *
     * @param retain_object_t Tag type indicating retention of ownership.
     * @param ptr The pointer to the object to retain.
     *
     * @throws utl::program_exception<void> - if ptr is null and exceptions are enabled
     */
    UTL_CONSTEXPR_CXX14 void reset(retain_object_t, T* ptr) noexcept(!utl::with_exceptions) {
        reset();
        iff_notnull(ptr, [this](T* ptr) {
            increment(*ptr);
            resource_ = ptr;
        });
    }

    /**
     * Resets the intrusive_ptr to hold the specified pointer without altering its reference count.
     *
     * @param adopt_object_t Tag type indicating adoption of ownership.
     * @param ptr The pointer to the object to adopt.
     *
     * @throws utl::program_exception<void> - if ptr is null and exceptions are enabled
     */
    UTL_CONSTEXPR_CXX14 void reset(adopt_object_t, T* other) noexcept(!utl::with_exceptions) {
        reset();
        iff_notnull(ptr, [this](T* ptr) { resource_ = ptr; });
    }

    /**
     * Swaps the contents of this intrusive_ptr with another intrusive_ptr instance.
     *
     * @param other The intrusive_ptr to swap with.
     */
    UTL_CONSTEXPR_CXX14 void swap(intrusive_ptr& other) noexcept {
        T* tmp = other.resource_;
        other.resource_ = resource_;
        resource_ = tmp;
    }

private:
    /**
     * Pointer to the managed object
     */
    T* resource_;
};

/**
 * Specialization of intrusive_ptr for managing const-qualified objects.
 *
 * This specialization of intrusive_ptr is designed to manage const-qualified objects. It inherits
 * functionality from the base intrusive_ptr<T> class and provides additional const-qualified member
 * functions and constructors.
 *
 * @tparam T The const-qualified type of the managed object.
 *
 * @note This specialization deliberately disallows many const-qualified raw pointers versions of
 *       operations that are present in the non-const implementation. This restriction is due to
 *       the lack of guarantee that the constructed object is not const, and allowing
 *       const-qualified versions of the operations may introduce undefined behaviors as the act of
 *       incrementing/decrementing the reference count modifies the object.
 */
template <typename T>
class intrusive_ptr<T const> :
    private intrusive_ptr<T>,
    private pointer_comparable<intrusive_ptr<T const>> {
    using base_type = intrusive_ptr<T>;

public:
    // Inheriting constructors and assignment operators from the non-const intrusive_ptr
    using base_type::base_type;
    using base_type::operator=;
    using base_type::reset;
    using base_type::operator bool;

    /**
     * Dereference operator to access the const-qualified object pointed to by the intrusive_ptr.
     */
    constexpr T const* operator->() const noexcept { return this->get(); }

    /**
     * Dereference operator to access the const-qualified object pointed to by the intrusive_ptr.
     */
    constexpr T const& operator*() const noexcept { return *(this->get()); }

    /**
     * Returns the underlying raw pointer to the const-qualified object.
     */
    constexpr T const* get() const noexcept { return base_type::get(); }

    /**
     * Releases ownership of the const-qualified managed object.
     * Resets the `intrusive_ptr` to hold a null pointer and returns the previous raw pointer.
     */
    UTL_CONSTEXPR_CXX14 T const* release() noexcept { return base_type::release(); }

    /**
     * Copy constructor
     */
    UTL_CONSTEXPR_CXX14 intrusive_ptr(intrusive_ptr<T const> const& other) noexcept
        : base_type(static_cast<base_type const&>(other)) {}

    /**
     * Copy assignment
     */
    UTL_CONSTEXPR_CXX14 intrusive_ptr& operator=(intrusive_ptr<T const> const& other) noexcept {
        base_type::operator=(static_cast<base_type const&>(other));
        return *this;
    }

    /**
     * Move constructor
     */
    constexpr intrusive_ptr(intrusive_ptr<T const>&& other) noexcept
        : base_type(static_cast<base_type&&>(other)) {}

    /**
     * Copy assignment
     */
    UTL_CONSTEXPR_CXX14 intrusive_ptr& operator=(intrusive_ptr<T const>&& other) noexcept {
        base_type::operator=(static_cast<base_type&&>(other));
        return *this;
    }

    /**
     * Swaps the contents of this intrusive_ptr with another intrusive_ptr instance.
     *
     * @param other The intrusive_ptr to swap with.
     */
    UTL_CONSTEXPR_CXX14 void swap(intrusive_ptr& other) noexcept {
        base_type::swap(static_cast<base_type&>(other));
    }
};

template <typename T, typename... Args>
UTL_CONSTEXPR_CXX20 intrusive_ptr<T> make_intrusive_ptr(Args&&... args) {
    // remove const so the constructed object is not const
    using object_type = remove_const_t<T>;
    return intrusive_ptr<T>(adopt_object, new object_type(forward<Args>(args)...));
}

UTL_NAMESPACE_END
