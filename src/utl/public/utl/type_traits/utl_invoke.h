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
invalid_t invoke(Args&&...) noexcept;

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
constexpr auto unwrap(T&& t) noexcept(noexcept(t.get())) -> decltype(t.get()) {
    return t.get();
}

template <typename T UTL_REQUIRES_CXX11(!UTL_TRAIT_is_reference_wrapper(T))>
constexpr T&& unwrap(T&& t) noexcept {
    return UTL_SCOPE forward(t);
}

template <typename T UTL_REQUIRES_CXX11(UTL_TRAIT_is_dereferenceable(T))>
constexpr auto unwrap(T&& t) noexcept(UTL_TRAIT_is_nothrow_dereferenceable(T)) -> decltype(*t) {
    return *t;
}

template <typename F, typename A, typename Class = class_type_t<remove_cvref_t<F>>>
using requires_member_function =
    enable_if_t<UTL_TRAIT_is_member_function_pointer(remove_cvref_t<F>) &&
            UTL_TRAIT_is_base_of(Class, remove_cvref_t<decltype(unwrap(declval<A>()))>),
        int>;
template <typename F, typename A, typename Class = class_type_t<remove_cvref_t<F>>>
using requires_member_function_ptr_arg =
    enable_if_t<UTL_TRAIT_is_member_function_pointer(remove_cvref_t<F>) &&
            !UTL_TRAIT_is_base_of(Class, remove_cvref_t<decltype(unwrap(declval<A>()))>),
        int>;
template <typename F, typename A, typename Class = class_type_t<remove_cvref_t<F>>>
using requires_member_object = enable_if_t<UTL_TRAIT_is_member_object_pointer(remove_cvref_t<F>) &&
        UTL_TRAIT_is_base_of(Class, remove_cvref_t<decltype(unwrap(declval<A>()))>),
    int>;
template <typename F, typename A, typename Class = class_type_t<remove_cvref_t<F>>>
using requires_member_object_ptr_arg =
    enable_if_t<UTL_TRAIT_is_member_object_pointer(remove_cvref_t<F>) &&
            !UTL_TRAIT_is_base_of(Class, remove_cvref_t<decltype(unwrap(declval<A>()))>),
        int>;

template <typename F, typename A, typename... Args, requires_member_function<F, A> = 0>
inline constexpr auto invoke(F&& f, A&& a, Args&&... args) noexcept(
    noexcept((unwrap(declval<A>()).*declval<F>())(declval<Args>()...)))
    -> decltype((unwrap(declval<A>()).*declval<F>())(declval<Args>()...)) {
    return (unwrap(UTL_SCOPE forward<A>(a)).*f)(UTL_SCOPE forward<Args>(args)...);
}

template <typename F, typename A, typename... Args, requires_member_function_ptr_arg<F, A> = 1>
inline constexpr auto invoke(F&& f, A&& a, Args&&... args) noexcept(
    noexcept((unwrap(declval<A>()).*declval<F>())(declval<Args>()...)))
    -> decltype((unwrap(declval<A>()).*declval<F>())(declval<Args>()...)) {
    return (unwrap(UTL_SCOPE forward<A>(a)).*f)(UTL_SCOPE forward<Args>(args)...);
}

template <typename F, typename A, requires_member_object<F, A> = 2>
inline constexpr auto invoke(F&& f, A&& a) noexcept(noexcept(unwrap(declval<A>()).*declval<F>()))
    -> decltype(unwrap(declval<A>()).*declval<F>()) {
    return unwrap(UTL_SCOPE forward<A>(a)).*f;
}

template <typename F, typename A, requires_member_object_ptr_arg<F, A> = 3>
inline constexpr auto invoke(F&& f, A&& a) noexcept(noexcept(unwrap(declval<A>()).*declval<F>()))
    -> decltype(unwrap(declval<A>()).*declval<F>()) {
    return unwrap(UTL_SCOPE forward<A>(a)).*f;
}

template <typename F, typename... Args>
inline constexpr auto invoke(F&& f, Args&&... args) noexcept(
    noexcept(declval<F>()(declval<Args>()...))) -> decltype(declval<F>()(declval<Args>()...)) {
    return UTL_SCOPE forward<F>(f)(UTL_SCOPE forward<Args>(args)...);
}

template <typename Result, typename T>
struct returnable_as;
template <typename T>
struct returnable_as<void, T> : true_type {};
template <typename T>
struct returnable_as<invalid_t, T> : false_type {};
template <typename R, typename T>
auto returnable_as_impl(int) noexcept
    -> decltype(static_cast<UTL_SCOPE true_type (*)(R)>(0)(static_cast<T (*)()>(0)()));
template <typename R, typename T>
auto returnable_as_impl(float) noexcept -> UTL_SCOPE false_type;
template <typename Result, typename T>
struct returnable_as : decltype(returnable_as_impl<Result, T>(0)) {};

template <typename Result, typename T>
struct nothrow_returnable_as;
template <typename T>
struct nothrow_returnable_as<void, T> : is_nothrow_destructible<T> {};
template <typename T>
struct nothrow_returnable_as<invalid_t, T> : false_type {};
template <typename T>
void noexcept_test(T) noexcept;
template <typename Result, typename T>
struct nothrow_returnable_as :
    bool_constant<noexcept(noexcept_test<Result>(static_cast<T (*)()>(0)()))> {};

template <typename R, typename F, typename... Args>
auto resolve(int) noexcept -> returnable_as<R, decltype(invoke(declval<F>(), declval<Args>()...))>;
template <typename R, typename F, typename... Args>
auto resolve(float) noexcept -> false_type;

template <typename R, typename F, typename... Args,
    bool V = noexcept(invoke(declval<F>(), declval<Args>()...))>
auto resolve_nothrow(int) noexcept -> bool_constant<V &&
    nothrow_returnable_as<R, decltype(invoke(declval<F>(), declval<Args>()...))>::value>;
template <typename R, typename F, typename... Args>
auto resolve_nothrow(float) noexcept -> false_type;

template <typename R, typename F, typename... Args>
using impl_t = decltype(resolve<R, F, Args...>(0));
template <typename R, typename F, typename... Args>
using nothrow_t = decltype(resolve_nothrow<R, F, Args...>(0));

} // namespace invocable
} // namespace details

template <typename R, typename F, typename... Args>
struct is_invocable_r : details::invocable::impl_t<R, F, Args...> {};
template <typename F, typename... Args>
struct is_invocable : details::invocable::impl_t<void, F, Args...> {};
template <typename R, typename F, typename... Args>
struct is_nothrow_invocable_r : details::invocable::nothrow_t<R, F, Args...> {};
template <typename F, typename... Args>
struct is_nothrow_invocable : details::invocable::nothrow_t<void, F, Args...> {};

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
#  define UTL_TRAIT_is_invocable_r(...) UTL_SCOPE is_invocable_r_v<__VA_ARGS__>
#  define UTL_TRAIT_is_invocable(...) UTL_SCOPE is_invocable_v<__VA_ARGS__>
#  define UTL_TRAIT_is_nothrow_invocable_r(...) UTL_SCOPE is_nothrow_invocable_r_v<__VA_ARGS__>
#  define UTL_TRAIT_is_nothrow_invocable(...) UTL_SCOPE is_nothrow_invocable_v<__VA_ARGS__>
#else
#  define UTL_TRAIT_is_invocable_r(...) UTL_SCOPE is_invocable_r<__VA_ARGS__>::value
#  define UTL_TRAIT_is_invocable(...) UTL_SCOPE is_invocable<__VA_ARGS__>::value
#  define UTL_TRAIT_is_nothrow_invocable_r(...) UTL_SCOPE is_nothrow_invocable_r<__VA_ARGS__>::value
#  define UTL_TRAIT_is_nothrow_invocable(...) UTL_SCOPE is_nothrow_invocable<__VA_ARGS__>::value
#endif

UTL_NAMESPACE_BEGIN
template <typename F, typename... Args>
struct invoke_result :
    enable_if<UTL_TRAIT_is_invocable(F, Args...),
        decltype(details::invocable::invoke(declval<F>(), declval<Args>()...))> {};

template <typename F, typename... Args>
using invoke_result_t = typename invoke_result<F, Args...>::type;
UTL_NAMESPACE_END
