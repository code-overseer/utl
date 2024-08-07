

// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_config.h"

#include "utl/memory/utl_pointer_traits.h"

#if UTL_CXX20

UTL_NAMESPACE_BEGIN

namespace details {
namespace pointer_traits {

template <typename Ptr>
concept has_to_address = requires(Ptr const& p) { UTL_SCOPE pointer_traits<Ptr>::to_address(p); };

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
UTL_HIDE_FROM_ABI auto has_to_address_impl(float) noexcept -> UTL_SCOPE false_type;
template <typename Ptr>
UTL_HIDE_FROM_ABI auto has_to_address_impl(int) noexcept
    -> UTL_SCOPE always_true_type<decltype(UTL_SCOPE pointer_traits<Ptr>::to_address(
        UTL_SCOPE declval<Ptr const&>()))>;

template <typename Ptr>
using has_to_address UTL_NODEBUG = decltype(has_to_address_impl<Ptr>(0));

template <typename Ptr>
UTL_HIDE_FROM_ABI auto has_arrow_operator_impl(float) noexcept -> UTL_SCOPE false_type;
template <typename Ptr>
UTL_HIDE_FROM_ABI auto has_arrow_operator_impl(int) noexcept
    -> UTL_SCOPE always_true_type<decltype(UTL_SCOPE declval<Ptr const&>().operator->())>;

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
    UTL_ATTRIBUTES(HIDE_FROM_ABI, NODISCARD, CONST, ALWAYS_INLINE) inline constexpr T* operator()(
        T* ptr) const noexcept {
        static_assert(!UTL_TRAIT_is_function(T), "T cannot be a function");
        return ptr;
    }

    template <UTL_CONCEPT_CXX20(has_to_address) T UTL_REQUIRES_CXX11(
        !UTL_SCOPE is_pointer<T>::value && has_to_address<T>::value)>
    UTL_REQUIRES_CXX20(!UTL_SCOPE is_pointer<T>::value)
    UTL_ATTRIBUTES(HIDE_FROM_ABI, NODISCARD, ALWAYS_INLINE) inline constexpr auto operator()(
        T const& ptr) const noexcept -> decltype(UTL_SCOPE pointer_traits<T>::to_address(ptr)) {
        return UTL_SCOPE pointer_traits<T>::to_address(ptr);
    }

    template <UTL_CONCEPT_CXX20(has_arrow_operator) T UTL_REQUIRES_CXX11(
        !UTL_SCOPE is_pointer<T>::value && !has_to_address<T>::value &&
        has_arrow_operator<T>::value)>
    UTL_REQUIRES_CXX20(!UTL_SCOPE is_pointer<T>::value && !has_to_address<T>)
    UTL_ATTRIBUTES(HIDE_FROM_ABI, NODISCARD, ALWAYS_INLINE) inline constexpr auto operator()(
        T const& ptr) const noexcept -> decltype(this->operator()(ptr.operator->())) {
        return this->operator()(ptr.operator->());
    }
};

} // namespace pointer_traits
} // namespace details

UTL_DEFINE_CUSTOMIZATION_POINT(details::pointer_traits::to_address_t, to_address);

UTL_NAMESPACE_END
