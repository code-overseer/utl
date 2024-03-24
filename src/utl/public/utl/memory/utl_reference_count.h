// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/atomic.h"
#include "utl/base_preprocessor.h"
#include "utl_reference_countable_destroy.h"

UTL_NAMESPACE_BEGIN
/**
 * A non-threadsafe reference count base class to be inherited by classes that require
 * reference counting functionality.
 *
 * This class provides basic reference counting mechanisms for managing the lifetime
 * of objects. It is designed to be inherited by classes that need reference counting
 * capabilities without introducing thread safety overhead.
 *
 * @tparam T The type of the class inheriting reference_count. T can be forward declared if
 *           the instantiation of the `reference_counting::destroy` function template is deferred
 *           using extern template @see reference_counting::destroy OR if an non-templated overload
 *           is provided in the form void destroy(reference_count<T>* ptr).
 */
template <typename T>
class reference_count {
protected:
    /**
     * Constructs the reference_count object with an initial count of 1.
     */
    constexpr reference_count() noexcept : count_(1) {}
    UTL_CONSTEXPR_CXX20 ~reference_count() noexcept = default;

    /**
     * Copy and move operations are deleted to enforce non-copyability and non-movability
     */
    reference_count(reference_count const&) = delete;
    reference_count& operator=(reference_count const&) = delete;
    reference_count(reference_count&&) noexcept = delete;
    reference_count& operator=(reference_count&&) noexcept = delete;

private:
    /**
     * ADL function to increment the reference count.
     *
     * @param obj The reference_count object to increment the count for.
     */
    friend void increment(reference_count& obj) noexcept { ++obj.count_; }

    /**
     * ADL function to decrement the reference count. Destroys the object when the count reaches
     * zero.
     *
     * @param obj The reference_count object to decrement the count for.
     */
    friend void decrement(reference_count& obj) noexcept {
        if (--obj.count_ < 1) {
            UTL_ASSERT(obj.count_ >= 0);
            reference_counting::details::destroy(addressof(obj));
        }
    }

    /**
     * ADL function to increment the reference count for objects of type T.
     *
     * @param obj The object of type T to increment the count for.
     */
    friend void increment(T& obj) noexcept {
        static_assert(UTL_TRAIT_VALUE(is_base_of, reference_count, T), "Invalid type relation");
        increment((reference_count&)obj);
    }

    /**
     * ADL function to decrement the reference count for objects of type T.
     *
     * @param obj The object of type T to decrement the count for.
     */
    friend void decrement(T& obj) noexcept {
        static_assert(UTL_TRAIT_VALUE(is_base_of, reference_count, T), "Invalid type relation");
        decrement((reference_count&)obj);
    }

    /**
     * Reference count for the object
     */
    int count_;
};

UTL_NAMESPACE_END
