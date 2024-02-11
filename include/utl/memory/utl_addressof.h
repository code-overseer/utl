// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/base_preprocessor.h"
#include "utl/type_traits/utl_std_traits.h"

UTL_NAMESPACE_BEGIN

template<typename T >
constexpr T* addressof(T&&) = delete;

#ifdef UTL_BUILTIN_addressof

template<typename T>
UTL_ATTRIBUTES(NODISCARD, CONST)
constexpr enable_if_t<is_object<remove_reference_t<T>>::value, T*> addressof(UTL_ATTRIBUTE_LIFETIMEBOUND T& arg) noexcept {
    return UTL_BUILTIN_addressof(arg);
}

template<typename T>
UTL_ATTRIBUTES(NODISCARD, CONST)
constexpr enable_if_t<!is_object<remove_reference_t<T>>::value, T*> addressof(UTL_ATTRIBUTE_LIFETIMEBOUND T& arg) noexcept {
    return &arg;
}

template<typename T>
struct is_addressof_constexpr : true_type {};

#else  // ifdef UTL_BUILTIN_addressof

namespace details {
namespace addressof {
template<typename T, typename = void>
struct has_adl_overload : false_type {};
template<typename T>
struct has_adl_overload<T, void_t<decltype(operator&(declval<add_lvalue_reference_t<T>>()))>> : true_type {};

template<typename T, typename = void>
struct has_member_overload : false_type {};
template<typename T>
struct has_member_overload<T, void_t<decltype(declval<add_lvalue_reference_t<T>>().operator&())>> : true_type {};

template<typename T, typename = void>
struct natively_invocable : false_type {};
template<typename T>
struct natively_invocable<T, void_t<decltype(&declval<add_lvalue_reference_t<T>>())>> : true_type {};

template<typename T>
struct has_overload : disjunction<
    negation<natively_invocable<T>>,
    has_adl_overload<T>,
    has_member_overload<T>
> {};
}   // namespace addressof
}   // namespace details

template<typename T>
UTL_ATTRIBUTES(NODISCARD, CONST, INTRINSIC)
inline enable_if_t<details::addressof::has_overload<T>::value, T*> addressof(T& arg UTL_ATTRIBUTE(LIFETIMEBOUND)) noexcept {
    return reinterpret_cast<T*>(&reinterpret_cast<copy_cvref_t<T&, char>>(arg));
}

template<typename T>
UTL_ATTRIBUTES(NODISCARD, CONST)
constexpr enable_if_t<!details::addressof::has_overload<T>::value, T*> addressof(T& arg UTL_ATTRIBUTE(LIFETIMEBOUND)) noexcept {
    return &arg;
}

template<typename T>
struct is_addressof_constexpr : negation<details::addressof::has_overload<T>> {};

#endif  // ifndef UTL_BUILTIN_addressof

UTL_NAMESPACE_END
