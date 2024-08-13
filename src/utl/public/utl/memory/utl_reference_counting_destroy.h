
// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

#include "utl/memory/utl_is_reference_countable.h"
#include "utl/utility/utl_customization_point.h"

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

namespace details {
template <typename T>
void destroy(T*) noexcept = delete;

struct __UTL_ABI_PUBLIC destroy_cpo_t {
private:
    template <typename T>
    __UTL_HIDE_FROM_ABI static auto has_custom_destroy_impl(int) noexcept
        -> always_true_type<decltype(destroy((T*)0))>;
    template <typename T>
    __UTL_HIDE_FROM_ABI static auto has_custom_destroy_impl(float) noexcept -> false_type;

    template <typename T>
    using has_custom_destroy UTL_NODEBUG = decltype(has_custom_destroy_impl<T>(0));

public:
    template <UTL_CONCEPT_CXX20(reference_countable) T UTL_CONSTRAINT_CXX11(
        is_reference_countable<T>::value && has_custom_destroy<T>::value)>
    UTL_CONSTRAINT_CXX20(requires(T* p) {
        destroy(p); })
    __UTL_HIDE_FROM_ABI UTL_CONSTEXPR_CXX20 void operator()(T* ptr) const noexcept {
        destroy(ptr);
    }

    template <UTL_CONCEPT_CXX20(reference_countable) T UTL_CONSTRAINT_CXX11(
        is_reference_countable<T>::value && !has_custom_destroy<T>::value)>
    __UTL_HIDE_FROM_ABI UTL_CONSTEXPR_CXX20 void operator()(T* ptr) const noexcept {
        delete ptr;
    }
};
} // namespace details

inline namespace cpo {
UTL_DEFINE_CUSTOMIZATION_POINT(__UTL reference_counting::details::destroy_cpo_t, destroy);
}

} // namespace reference_counting

UTL_NAMESPACE_END
