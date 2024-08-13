

// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

#include "utl/memory/utl_pointer_traits.h"

#if UTL_CXX20

UTL_NAMESPACE_BEGIN

namespace details {
namespace pointer_traits {

template <typename Ptr>
concept has_to_address = requires(Ptr const& p) { __UTL pointer_traits<Ptr>::to_address(p); };

template <typename Ptr>
concept has_arrow_operator = requires(Ptr const& p) { p.operator->(); };

} // namespace pointer_traits
} // namespace details

UTL_NAMESPACE_END

#else

#  include "utl/type_traits/utl_constants.h"
#  include "utl/type_traits/utl_declval.h"

UTL_NAMESPACE_BEGIN

namespace details {
namespace pointer_traits {

template <typename Ptr>
__UTL_HIDE_FROM_ABI auto has_to_address_impl(float) noexcept -> __UTL false_type;
template <typename Ptr>
__UTL_HIDE_FROM_ABI auto has_to_address_impl(int) noexcept -> __UTL
    always_true_type<decltype(__UTL pointer_traits<Ptr>::to_address(__UTL declval<Ptr const&>()))>;

template <typename Ptr>
using has_to_address UTL_NODEBUG = decltype(has_to_address_impl<Ptr>(0));

template <typename Ptr>
__UTL_HIDE_FROM_ABI auto has_arrow_operator_impl(float) noexcept -> __UTL false_type;
template <typename Ptr>
__UTL_HIDE_FROM_ABI auto has_arrow_operator_impl(int) noexcept
    -> __UTL always_true_type<decltype(__UTL declval<Ptr const&>().operator->())>;

template <typename Ptr>
using has_arrow_operator UTL_NODEBUG = decltype(has_arrow_operator_impl<Ptr>(0));

} // namespace pointer_traits
} // namespace details

UTL_NAMESPACE_END

#endif

UTL_NAMESPACE_BEGIN

namespace details {
namespace pointer_traits {
struct to_address_t {
    template <typename T>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD, CONST, ALWAYS_INLINE) inline constexpr T* operator()(
        T* ptr) const noexcept {
        static_assert(!UTL_TRAIT_is_function(T), "T cannot be a function");
        return ptr;
    }

    template <UTL_CONCEPT_CXX20(has_to_address) T UTL_CONSTRAINT_CXX11(
        !__UTL is_pointer<T>::value && has_to_address<T>::value)>
    UTL_CONSTRAINT_CXX20(!__UTL is_pointer<T>::value)
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD, ALWAYS_INLINE) inline constexpr auto operator()(
        T const& ptr) const noexcept -> decltype(__UTL pointer_traits<T>::to_address(ptr)) {
        return __UTL pointer_traits<T>::to_address(ptr);
    }

    template <UTL_CONCEPT_CXX20(has_arrow_operator) T UTL_CONSTRAINT_CXX11(
        !__UTL is_pointer<T>::value && !has_to_address<T>::value && has_arrow_operator<T>::value)>
    UTL_CONSTRAINT_CXX20(!__UTL is_pointer<T>::value && !has_to_address<T>)
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD, ALWAYS_INLINE) inline constexpr auto operator()(
        T const& ptr) const noexcept -> decltype(this->operator()(ptr.operator->())) {
        return this->operator()(ptr.operator->());
    }
};

} // namespace pointer_traits
} // namespace details

UTL_DEFINE_CUSTOMIZATION_POINT(details::pointer_traits::to_address_t, to_address);

UTL_NAMESPACE_END
