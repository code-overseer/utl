// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_config.h"

#include "utl/atomic.h"
#include "utl/memory/utl_addressof.h"
#include "utl/memory/utl_reference_counting_destroy.h"

UTL_NAMESPACE_BEGIN

/**
 * A thread-safe reference count base class to be inherited by classes that require
 * reference counting functionality with atomic operations.
 *
 * This class provides reference counting mechanisms with atomic operations for managing the
 * lifetime of objects in a thread-safe manner. It is designed to be inherited by classes that need
 * reference counting capabilities with the added safety of atomic operations.
 *
 * @tparam T - The type of the class inheriting atomic_reference_count. T can be forward declared if
 *           the instantiation of the `reference_counting::destroy` function template is deferred
 *           using extern template @see reference_counting::destroy OR if an non-templated overload
 *           is provided in the form void destroy(atomic_reference_count<T>* ptr).
 */
template <typename T>
class UTL_PUBLIC_TEMPLATE atomic_reference_count {
public:
    /**
     * The type of the value managed by atomic_reference_count.
     */
    using value_type = T;

protected:
    /**
     * Constructs the reference_count object with an initial count of 1.
     */
    UTL_HIDE_FROM_ABI constexpr atomic_reference_count() noexcept : count_(1) {}
    UTL_HIDE_FROM_ABI UTL_CONSTEXPR_CXX20 ~atomic_reference_count() noexcept = default;

    /**
     * Copy and move operations are deleted to enforce non-copyability and non-movability
     */
    atomic_reference_count(atomic_reference_count const&) = delete;
    atomic_reference_count& operator=(atomic_reference_count const&) = delete;
    atomic_reference_count(atomic_reference_count&&) noexcept = delete;
    atomic_reference_count& operator=(atomic_reference_count&&) noexcept = delete;

private:
    using this_type = atomic_reference_count<T>;

    /**
     * ADL function to increment the reference count for objects of type T.
     *
     * @param obj The object of type T to increment the count for.
     */
    UTL_HIDE_FROM_ABI friend void increment(T& obj) noexcept {
        static_assert(UTL_TRAIT_is_base_of(atomic_reference_count, T), "Invalid type relation");
        ((atomic_reference_count&)obj).count_.fetch_add(1, UTL_SCOPE memory_order_relaxed);
    }

    /**
     * ADL function to decrement the reference count for objects of type T.
     *
     * @param obj The object of type T to decrement the count for.
     */
    UTL_HIDE_FROM_ABI friend void decrement(T& obj) noexcept {
        static_assert(UTL_TRAIT_is_base_of(atomic_reference_count, T), "Invalid type relation");
        int const result =
            ((atomic_reference_count&)obj).count_.fetch_sub(1, UTL_SCOPE memory_order_acq_rel);
        if (result <= 1) {
            UTL_ASSERT(result > 0);
            reference_counting::destroy(UTL_SCOPE addressof(obj));
        }
    }

    /**
     * Reference count for the object
     */
    atomic<int> count_;
};

UTL_NAMESPACE_END
