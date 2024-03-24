
// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/base_preprocessor.h"
#include "utl_is_reference_countable.h"

UTL_NAMESPACE_BEGIN

namespace reference_counting {

/**
 * Destroys the object pointed to by the given pointer when the reference count reaches zero.
 *
 * @tparam T The type of object being managed by reference counting.
 *
 * @param ptr A pointer to the object to be destroyed.
 *
 * @note This function is specifically designed for objects managed by reference counting. It is
 *       expected that the object pointed to by `ptr` inherits from either
 * `atomic_reference_count<T>` or `reference_count<T>`.
 *
 * @note It is the responsibility of the caller to ensure that the object being pointed to is
 *       allocated on the heap and was previously allocated using dynamic memory allocation
 *       (e.g., `new` operator). Calling this function on objects allocated on the stack or
 *       statically allocated objects will result in undefined behavior.
 *
 * @note This function performs a `delete` operation on the object pointed to by `ptr`. Therefore,
 *       the object must have been allocated using the `new` operator or an allocator that matches
 *       with `delete`.
 *
 * @note The behavior of this function is undefined if `ptr` does not point to a valid object
 *       managed by reference counting.
 */
template <typename T>
UTL_CONSTEXPR_CXX20 void destroy(atomic_reference_count<T>* ptr) noexcept {
    static_assert(is_base_of<atomic_reference_count<T>, T>::value, "invalid type");
    delete (T*)ptr;
}

template <typename T>
UTL_CONSTEXPR_CXX20 void destroy(reference_count<T>* ptr) noexcept {
    static_assert(is_base_of<reference_count<T>, T>::value, "invalid type");
    delete (T*)ptr;
}

struct destroy_cpo_t {
    template <UTL_CONCEPT_CXX20(reference_countable)
            T UTL_REQUIRES_CXX11(is_reference_countable<T>::value)>
    UTL_CONSTEXPR_CXX20 void operator()(T* ptr) const noexcept {
        destroy(ptr);
    }
};

namespace details {
UTL_INLINE_CXX17 constexpr destroy_cpo_t destroy = {};
}

} // namespace reference_counting

UTL_NAMESPACE_END
