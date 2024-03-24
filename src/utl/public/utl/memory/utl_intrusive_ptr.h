// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/atomic.h"
#include "utl/base_preprocessor.h"
#include "utl/compare/utl_pointer_comparable.h"
#include "utl/memory/utl_addressof.h"
#include "utl/type_traits/declval.h"
#include "utl/utility/utl_exchange.h"
#include "utl/utility/utl_forward.h"
#include "utl_reference_counter.h"

UTL_NAMESPACE_BEGIN

UTL_INLINE_CXX17 constexpr class retain_object_t {
} retain_object = {};
UTL_INLINE_CXX17 constexpr class adopt_object_t {
} adopt_object = {};

template <typename T>
class intrusive_ptr : private pointer_comparable<intrusive_ptr<T>> {
    template <typename F>
    static UTL_CONSTEXPR_CXX14 auto iff_notnull(T* ptr, F&& func) noexcept
        -> decltype(declval<F>()((T*)nullptr)) {
        UTL_ASSERT(ptr != nullptr);
        return forward<F>(func)(ptr);
    }

public:
    constexpr intrusive_ptr() noexcept : resource_(nullptr) {}
    UTL_CONSTEXPR_CXX14 intrusive_ptr(adopt_object_t, T* ptr) noexcept
        : resource_(iff_notnull(ptr, [](T* ptr) { return ptr; })) {}
    UTL_CONSTEXPR_CXX14 intrusive_ptr(retain_object_t, T* ptr) noexcept
        : resource_(iff_notnull(ptr, [](T* ptr) {
            increment(*ptr);
            return ptr;
        })) {}

    UTL_CONSTEXPR_CXX14 intrusive_ptr(intrusive_ptr const& other) noexcept
        : intrusive_ptr(retain_object, other.resource_) {}

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

    constexpr intrusive_ptr(intrusive_ptr&& other) noexcept
        : resource_(exchange(other.resource_, nullptr)) {}

    UTL_CONSTEXPR_CXX14 intrusive_ptr& operator=(intrusive_ptr&& other) noexcept {
        if (addressof(other) != this) {
            reset();
            resource_ = exchange(other.resource_, nullptr);
        }
        return *this;
    }

    constexpr T* operator->() const noexcept { return resource_; }
    constexpr T& operator*() const noexcept { return *resource_; }
    constexpr T* get() const noexcept { return resource_; }
    constexpr T* release() noexcept { return exchange(resource_, nullptr); }
    constexpr explicit operator bool() const noexcept { return resource_ != nullptr; }

    UTL_CONSTEXPR_CXX14 void reset() noexcept {
        if (resource_ != nullptr) {
            decrement(*resource_);
            resource_ = nullptr;
        }
    }

    UTL_CONSTEXPR_CXX14 void reset(retain_object_t, T* ptr) noexcept {
        reset();
        iff_notnull(ptr, [this](T* ptr) {
            increment(*ptr);
            resource_ = ptr;
        });
    }

    UTL_CONSTEXPR_CXX14 void reset(adopt_object_t, T* other) noexcept {
        reset();
        iff_notnull(ptr, [this](T* ptr) { resource_ = ptr; });
    }

    UTL_CONSTEXPR_CXX14 void swap(intrusive_ptr& other) noexcept {
        T* tmp = other.resource_;
        other.resource_ = resource_;
        resource_ = tmp;
    }

    UTL_CONSTEXPR_CXX20 ~intrusive_ptr() noexcept { reset(); }

private:
    T* resource_;
};

template <typename T>
class intrusive_ptr<T const> :
    private intrusive_ptr<T>,
    private pointer_comparable<intrusive_ptr<T const>> {
    using base_type = intrusive_ptr<T>;

public:
    using intrusive_ptr::intrusive_ptr;
    using intrusive_ptr::operator=;
    using intrusive_ptr::reset;
    using intrusive_ptr::operator bool;

    constexpr T const* operator->() const noexcept { return this->get(); }
    constexpr T const& operator*() const noexcept { return *(this->get()); }
    constexpr T const* get() const noexcept { return base_type::get(); }
    constexpr T const* release() noexcept { return base_type::release(); }

    UTL_CONSTEXPR_CXX14 intrusive_ptr(intrusive_ptr<T const> const& other) noexcept
        : base_type(static_cast<base_type const&>(other)) {}

    UTL_CONSTEXPR_CXX14 intrusive_ptr& operator=(intrusive_ptr<T const> const& other) noexcept {
        base_type::operator=(static_cast<base_type const&>(other));
        return *this;
    }

    constexpr intrusive_ptr(intrusive_ptr<T const>&& other) noexcept
        : base_type(static_cast<base_type&&>(other)) {}

    UTL_CONSTEXPR_CXX14 intrusive_ptr& operator=(intrusive_ptr<T const>&& other) noexcept {
        base_type::operator=(static_cast<base_type&&>(other));
        return *this;
    }
};

template <typename T, typename... Args>
UTL_CONSTEXPR_CXX20 intrusive_ptr<T> make_intrusive_ptr(Args&&... args) {
    using object_type = remove_const_t<T>;
    return intrusive_ptr<object_type>(adopt_object, new object_type(forward<Args>(args)...));
}

UTL_NAMESPACE_END
