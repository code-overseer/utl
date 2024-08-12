// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/type_traits/utl_common.h"

#if UTL_USE_STD_TYPE_TRAITS && UTL_CXX17

#  include <type_traits>

UTL_NAMESPACE_BEGIN

using std::is_invocable;
using std::is_invocable_r;
using std::is_invocable_r_v;
using std::is_invocable_v;

UTL_NAMESPACE_END

#else // if UTL_USE_STD_TYPE_TRAITS && UTL_CXX17

#  include "utl/type_traits/utl_declval.h"
#  include "utl/type_traits/utl_enable_if.h"
#  include "utl/type_traits/utl_is_base_of.h"
#  include "utl/type_traits/utl_is_dereferenceable.h"
#  include "utl/type_traits/utl_is_member_function_pointer.h"
#  include "utl/type_traits/utl_is_member_object_pointer.h"
#  include "utl/type_traits/utl_is_nothrow_destructible.h"
#  include "utl/type_traits/utl_is_reference_wrapper.h"
#  include "utl/type_traits/utl_remove_cvref.h"
#  include "utl/utility/utl_forward.h"

UTL_NAMESPACE_BEGIN

namespace details {
namespace invocable {
class invalid_t {
    invalid_t() = default;
};
template <typename... Args>
__UTL_HIDE_FROM_ABI invalid_t invoke(Args&&...) noexcept;

template <typename>
struct class_type {};
template <typename T>
using class_type_t = typename class_type<T>::type;
template <typename C, typename R>
struct class_type<R C::*> {
    using type = C;
};

template <typename T UTL_REQUIRES_CXX11(UTL_TRAIT_is_reference_wrapper(T))>
UTL_REQUIRES_CXX20(UTL_TRAIT_is_reference_wrapper(T))
__UTL_HIDE_FROM_ABI constexpr auto unwrap(T&& t) noexcept(noexcept(t.get())) -> decltype(t.get()) {
    return t.get();
}

template <typename T UTL_REQUIRES_CXX11(!UTL_TRAIT_is_reference_wrapper(T))>
__UTL_HIDE_FROM_ABI constexpr T&& unwrap(T&& t) noexcept {
    return __UTL forward(t);
}

template <typename T UTL_REQUIRES_CXX11(UTL_TRAIT_is_dereferenceable(T))>
__UTL_HIDE_FROM_ABI constexpr auto unwrap(T&& t) noexcept(UTL_TRAIT_is_nothrow_dereferenceable(T))
    -> decltype(*t) {
    return *t;
}

template <typename T>
using unwrap_t UTL_NODEBUG = decltype(__UTL details::invocable::unwrap(__UTL declval<T>()));

template <typename F, typename A, typename Class = class_type_t<remove_cvref_t<F>>>
using requires_member_function UTL_NODEBUG =
    enable_if_t<UTL_TRAIT_is_member_function_pointer(remove_cvref_t<F>) &&
            UTL_TRAIT_is_base_of(Class, remove_cvref_t<unwrap_t<A>>),
        int>;
template <typename F, typename A, typename Class = class_type_t<remove_cvref_t<F>>>
using requires_member_function_ptr_arg UTL_NODEBUG =
    enable_if_t<UTL_TRAIT_is_member_function_pointer(remove_cvref_t<F>) &&
            !UTL_TRAIT_is_base_of(Class, remove_cvref_t<unwrap_t<A>>),
        int>;
template <typename F, typename A, typename Class = class_type_t<remove_cvref_t<F>>>
using requires_member_object UTL_NODEBUG =
    enable_if_t<UTL_TRAIT_is_member_object_pointer(remove_cvref_t<F>) &&
            UTL_TRAIT_is_base_of(Class, remove_cvref_t<unwrap_t<A>>),
        int>;
template <typename F, typename A, typename Class = class_type_t<remove_cvref_t<F>>>
using requires_member_object_ptr_arg UTL_NODEBUG =
    enable_if_t<UTL_TRAIT_is_member_object_pointer(remove_cvref_t<F>) &&
            !UTL_TRAIT_is_base_of(Class, remove_cvref_t<unwrap_t<A>>),
        int>;

template <typename F, typename A, typename... Args, requires_member_function<F, A> = 0>
UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline constexpr auto invoke(F&& f, A&& a,
    Args&&... args) noexcept(noexcept((unwrap(declval<A>()).*declval<F>())(declval<Args>()...)))
    -> decltype((unwrap(declval<A>()).*declval<F>())(declval<Args>()...)) {
    return (unwrap(__UTL forward<A>(a)).*f)(__UTL forward<Args>(args)...);
}

template <typename F, typename A, typename... Args, requires_member_function_ptr_arg<F, A> = 1>
UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline constexpr auto invoke(F&& f, A&& a,
    Args&&... args) noexcept(noexcept((unwrap(declval<A>()).*declval<F>())(declval<Args>()...)))
    -> decltype((unwrap(declval<A>()).*declval<F>())(declval<Args>()...)) {
    return (unwrap(__UTL forward<A>(a)).*f)(__UTL forward<Args>(args)...);
}

template <typename F, typename A, requires_member_object<F, A> = 2>
UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline constexpr auto invoke(F&& f, A&& a) noexcept(
    noexcept(unwrap(declval<A>()).*declval<F>())) -> decltype(unwrap(declval<A>()).*declval<F>()) {
    return unwrap(__UTL forward<A>(a)).*f;
}

template <typename F, typename A, requires_member_object_ptr_arg<F, A> = 3>
UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline constexpr auto invoke(F&& f, A&& a) noexcept(
    noexcept(unwrap(declval<A>()).*declval<F>())) -> decltype(unwrap(declval<A>()).*declval<F>()) {
    return unwrap(__UTL forward<A>(a)).*f;
}

template <typename F, typename... Args>
UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline constexpr auto invoke(F&& f, Args&&... args) noexcept(
    noexcept(declval<F>()(declval<Args>()...))) -> decltype(declval<F>()(declval<Args>()...)) {
    return __UTL forward<F>(f)(__UTL forward<Args>(args)...);
}

template <typename Result, typename T>
struct returnable_as;
template <typename T>
struct returnable_as<void, T> : true_type {};
template <typename T>
struct returnable_as<invalid_t, T> : false_type {};
template <typename R, typename T>
__UTL_HIDE_FROM_ABI auto returnable_as_impl(int) noexcept
    -> decltype(static_cast<__UTL true_type (*)(R)>(0)(static_cast<T (*)()>(0)()));
template <typename R, typename T>
__UTL_HIDE_FROM_ABI auto returnable_as_impl(float) noexcept -> __UTL false_type;
template <typename Result, typename T>
struct returnable_as : decltype(returnable_as_impl<Result, T>(0)) {};

template <typename Result, typename T>
struct nothrow_returnable_as;
template <typename T>
struct nothrow_returnable_as<void, T> : is_nothrow_destructible<T> {};
template <typename T>
struct nothrow_returnable_as<invalid_t, T> : false_type {};
template <typename T>
__UTL_HIDE_FROM_ABI void noexcept_test(T) noexcept;
template <typename Result, typename T>
struct nothrow_returnable_as :
    bool_constant<noexcept(noexcept_test<Result>(static_cast<T (*)()>(0)()))> {};

template <typename F, typename... Args>
using resolve_result_t UTL_NODEBUG =
    decltype(__UTL details::invocable::invoke(__UTL declval<F>(), __UTL declval<Args>()...));

template <typename R, typename F, typename... Args>
__UTL_HIDE_FROM_ABI auto resolve(int) noexcept -> returnable_as<R, resolve_result_t<F, Args...>>;
template <typename R, typename F, typename... Args>
__UTL_HIDE_FROM_ABI auto resolve(float) noexcept -> false_type;

template <typename R, typename F, typename... Args,
    bool V = noexcept(
        __UTL details::invocable::invoke(__UTL declval<F>(), __UTL declval<Args>()...))>
__UTL_HIDE_FROM_ABI auto resolve_nothrow(int) noexcept
    -> bool_constant<V && nothrow_returnable_as<R, resolve_result_t<F, Args...>>::value>;
template <typename R, typename F, typename... Args>
__UTL_HIDE_FROM_ABI auto resolve_nothrow(float) noexcept -> false_type;

template <typename R, typename F, typename... Args>
using impl_t UTL_NODEBUG = decltype(resolve<R, F, Args...>(0));
template <typename R, typename F, typename... Args>
using nothrow_t UTL_NODEBUG = decltype(resolve_nothrow<R, F, Args...>(0));

} // namespace invocable
} // namespace details

template <typename R, typename F, typename... Args>
struct __UTL_PUBLIC_TEMPLATE is_invocable_r : details::invocable::impl_t<R, F, Args...> {};
template <typename F, typename... Args>
struct __UTL_PUBLIC_TEMPLATE is_invocable : details::invocable::impl_t<void, F, Args...> {};
template <typename R, typename F, typename... Args>
struct __UTL_PUBLIC_TEMPLATE is_nothrow_invocable_r :
    details::invocable::nothrow_t<R, F, Args...> {};
template <typename F, typename... Args>
struct __UTL_PUBLIC_TEMPLATE is_nothrow_invocable :
    details::invocable::nothrow_t<void, F, Args...> {};

#  if UTL_CXX14
template <typename R, typename F, typename... Args>
UTL_INLINE_CXX17 constexpr bool is_invocable_r_v = details::invocable::impl_t<R, F, Args...>::value;
template <typename F, typename... Args>
UTL_INLINE_CXX17 constexpr bool is_invocable_v = details::invocable::impl_t<void, F, Args...>::value;
template <typename R, typename F, typename... Args>
UTL_INLINE_CXX17 constexpr bool is_nothrow_invocable_r_v =
    details::invocable::nothrow_t<R, F, Args...>::value;
template <typename F, typename... Args>
UTL_INLINE_CXX17 constexpr bool is_nothrow_invocable_v =
    details::invocable::nothrow_t<void, F, Args...>::value;
#  endif

UTL_NAMESPACE_END

#endif // if UTL_USE_STD_TYPE_TRAITS && UTL_CXX17

#define UTL_TRAIT_SUPPORTED_is_invocable 1
#define UTL_TRAIT_SUPPORTED_is_invocable_r 1

#if UTL_CXX14
#  define UTL_TRAIT_is_invocable_r(...) __UTL is_invocable_r_v<__VA_ARGS__>
#  define UTL_TRAIT_is_invocable(...) __UTL is_invocable_v<__VA_ARGS__>
#  define UTL_TRAIT_is_nothrow_invocable_r(...) __UTL is_nothrow_invocable_r_v<__VA_ARGS__>
#  define UTL_TRAIT_is_nothrow_invocable(...) __UTL is_nothrow_invocable_v<__VA_ARGS__>
#else
#  define UTL_TRAIT_is_invocable_r(...) __UTL is_invocable_r<__VA_ARGS__>::value
#  define UTL_TRAIT_is_invocable(...) __UTL is_invocable<__VA_ARGS__>::value
#  define UTL_TRAIT_is_nothrow_invocable_r(...) __UTL is_nothrow_invocable_r<__VA_ARGS__>::value
#  define UTL_TRAIT_is_nothrow_invocable(...) __UTL is_nothrow_invocable<__VA_ARGS__>::value
#endif

UTL_NAMESPACE_BEGIN
template <typename F, typename... Args>
struct __UTL_PUBLIC_TEMPLATE invoke_result :
    enable_if<UTL_TRAIT_is_invocable(F, Args...),
        decltype(details::invocable::invoke(__UTL declval<F>(), __UTL declval<Args>()...))> {};

template <typename F, typename... Args>
using invoke_result_t = typename invoke_result<F, Args...>::type;
UTL_NAMESPACE_END
