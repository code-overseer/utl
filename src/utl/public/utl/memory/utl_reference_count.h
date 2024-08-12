// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_config.h"

#include "utl/memory/utl_addressof.h"
#include "utl/memory/utl_reference_counting_destroy.h"

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
class __UTL_PUBLIC_TEMPLATE reference_count {
protected:
    /**
     * Constructs the reference_count object with an initial count of 1.
     */
    __UTL_HIDE_FROM_ABI constexpr reference_count() noexcept : count_(1) {}
    __UTL_HIDE_FROM_ABI UTL_CONSTEXPR_CXX20 ~reference_count() noexcept = default;

    /**
     * Copy and move operations are deleted to enforce non-copyability and non-movability
     */
    reference_count(reference_count const&) = delete;
    reference_count& operator=(reference_count const&) = delete;
    reference_count(reference_count&&) noexcept = delete;
    reference_count& operator=(reference_count&&) noexcept = delete;

private:
    /**
     * ADL function to increment the reference count for objects of type T.
     *
     * @param obj The object of type T to increment the count for.
     */
    __UTL_HIDE_FROM_ABI friend void increment(T& obj) noexcept {
        static_assert(UTL_TRAIT_is_base_of(reference_count, T), "Invalid type relation");
        ++((reference_count&)obj).count_;
    }

    /**
     * ADL function to decrement the reference count for objects of type T.
     *
     * @param obj The object of type T to decrement the count for.
     */
    __UTL_HIDE_FROM_ABI friend void decrement(T& obj) noexcept {
        static_assert(UTL_TRAIT_is_base_of(reference_count, T), "Invalid type relation");
        auto& ref = (reference_count&)obj;
        if (--ref.count_ < 1) {
            UTL_ASSERT(ref.count_ == 0);
            reference_counting::destroy(UTL_SCOPE addressof(obj));
        }
    }

    /**
     * Reference count for the object
     */
    int count_;
};

UTL_NAMESPACE_END
