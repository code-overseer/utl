// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_config.h"
#include "utl/tuple/utl_tuple_fwd.h"
#include "utl/type_traits/utl_enable_if.h"
#include "utl/type_traits/utl_is_function.h"
#include "utl/type_traits/utl_rebind_template.h"
#include "utl/type_traits/utl_template_list.h"

UTL_NAMESPACE_BEGIN

template <typename F>
struct function_type;

namespace details {
namespace function_info {
template <typename F, typename = void>
struct function_type;
template <typename F>
struct function_type<F*, enable_if_t<UTL_TRAIT_VALUE(is_function, F)>> :
    UTL_SCOPE function_type<F> {};
template <typename F>
struct function_type<F&, enable_if_t<UTL_TRAIT_VALUE(is_function, F)>> :
    UTL_SCOPE function_type<F> {};
} // namespace function_info
} // namespace details

template <typename F>
struct function_type : details::function_info::function_type<F> {};
template <typename F>
using function_type_t = typename function_type<F>::type;

template <typename R, typename... A>
struct function_type<R(A...)> {
    using type = R(A...);
    using return_type = R;
};

template <typename R, typename... A>
struct function_type<R(A...) const> {
    using type = R(A...) const;
    using return_type = R;
};

template <typename R, typename... A>
struct function_type<R(A...) const volatile> {
    using type = R(A...) const volatile;
    using return_type = R;
};

template <typename R, typename... A>
struct function_type<R(A...) volatile> {
    using type = R(A...) volatile;
    using return_type = R;
};

template <typename R, typename... A>
struct function_type<R(A...)&> {
    using type = R(A...);
    using return_type = R;
};

template <typename R, typename... A>
struct function_type<R(A...) const&> {
    using type = R(A...) const&;
    using return_type = R;
};

template <typename R, typename... A>
struct function_type<R(A...) const volatile&> {
    using type = R(A...) const volatile&;
    using return_type = R;
};

template <typename R, typename... A>
struct function_type<R(A...) volatile&> {
    using type = R(A...) volatile&;
    using return_type = R;
};

template <typename R, typename... A>
struct function_type<R(A...) &&> {
    using type = R(A...);
    using return_type = R;
};

template <typename R, typename... A>
struct function_type<R(A...) const&&> {
    using type = R(A...) const&&;
    using return_type = R;
};

template <typename R, typename... A>
struct function_type<R(A...) const volatile&&> {
    using type = R(A...) const volatile&&;
    using return_type = R;
};

template <typename R, typename... A>
struct function_type<R(A...) volatile&&> {
    using type = R(A...) volatile&&;
    using return_type = R;
};

namespace functional {
template <typename F>
struct is_const;
template <typename R, typename... A>
struct is_const<R(A...)> : bool_constant<false> {};
template <typename R, typename... A>
struct is_const<R(A...) const> : bool_constant<true> {};
template <typename R, typename... A>
struct is_const<R(A...) const volatile> : bool_constant<true> {};
template <typename R, typename... A>
struct is_const<R(A...) volatile> : bool_constant<false> {};
template <typename R, typename... A>
struct is_const<R(A...)&> : bool_constant<false> {};
template <typename R, typename... A>
struct is_const<R(A...) const&> : bool_constant<true> {};
template <typename R, typename... A>
struct is_const<R(A...) const volatile&> : bool_constant<true> {};
template <typename R, typename... A>
struct is_const<R(A...) volatile&> : bool_constant<false> {};
template <typename R, typename... A>
struct is_const<R(A...) &&> : bool_constant<false> {};
template <typename R, typename... A>
struct is_const<R(A...) const&&> : bool_constant<true> {};
template <typename R, typename... A>
struct is_const<R(A...) const volatile&&> : bool_constant<true> {};
template <typename R, typename... A>
struct is_const<R(A...) volatile&&> : bool_constant<false> {};

template <typename F>
struct is_volatile;
template <typename R, typename... A>
struct is_volatile<R(A...)> : bool_constant<false> {};
template <typename R, typename... A>
struct is_volatile<R(A...) const> : bool_constant<false> {};
template <typename R, typename... A>
struct is_volatile<R(A...) const volatile> : bool_constant<true> {};
template <typename R, typename... A>
struct is_volatile<R(A...) volatile> : bool_constant<true> {};
template <typename R, typename... A>
struct is_volatile<R(A...)&> : bool_constant<false> {};
template <typename R, typename... A>
struct is_volatile<R(A...) const&> : bool_constant<false> {};
template <typename R, typename... A>
struct is_volatile<R(A...) const volatile&> : bool_constant<true> {};
template <typename R, typename... A>
struct is_volatile<R(A...) volatile&> : bool_constant<true> {};
template <typename R, typename... A>
struct is_volatile<R(A...) &&> : bool_constant<false> {};
template <typename R, typename... A>
struct is_volatile<R(A...) const&&> : bool_constant<false> {};
template <typename R, typename... A>
struct is_volatile<R(A...) const volatile&&> : bool_constant<true> {};
template <typename R, typename... A>
struct is_volatile<R(A...) volatile&&> : bool_constant<true> {};

template <typename F>
struct is_cv;
template <typename R, typename... A>
struct is_cv<R(A...)> : bool_constant<false> {};
template <typename R, typename... A>
struct is_cv<R(A...) const> : bool_constant<false> {};
template <typename R, typename... A>
struct is_cv<R(A...) const volatile> : bool_constant<true> {};
template <typename R, typename... A>
struct is_cv<R(A...) volatile> : bool_constant<false> {};
template <typename R, typename... A>
struct is_cv<R(A...)&> : bool_constant<false> {};
template <typename R, typename... A>
struct is_cv<R(A...) const&> : bool_constant<false> {};
template <typename R, typename... A>
struct is_cv<R(A...) const volatile&> : bool_constant<true> {};
template <typename R, typename... A>
struct is_cv<R(A...) volatile&> : bool_constant<false> {};
template <typename R, typename... A>
struct is_cv<R(A...) &&> : bool_constant<false> {};
template <typename R, typename... A>
struct is_cv<R(A...) const&&> : bool_constant<false> {};
template <typename R, typename... A>
struct is_cv<R(A...) const volatile&&> : bool_constant<true> {};
template <typename R, typename... A>
struct is_cv<R(A...) volatile&&> : bool_constant<false> {};

template <typename F>
struct is_lvalue;
template <typename R, typename... A>
struct is_lvalue<R(A...)> : bool_constant<false> {};
template <typename R, typename... A>
struct is_lvalue<R(A...) const> : bool_constant<false> {};
template <typename R, typename... A>
struct is_lvalue<R(A...) const volatile> : bool_constant<false> {};
template <typename R, typename... A>
struct is_lvalue<R(A...) volatile> : bool_constant<false> {};
template <typename R, typename... A>
struct is_lvalue<R(A...)&> : bool_constant<true> {};
template <typename R, typename... A>
struct is_lvalue<R(A...) const&> : bool_constant<true> {};
template <typename R, typename... A>
struct is_lvalue<R(A...) const volatile&> : bool_constant<true> {};
template <typename R, typename... A>
struct is_lvalue<R(A...) volatile&> : bool_constant<true> {};
template <typename R, typename... A>
struct is_lvalue<R(A...) &&> : bool_constant<false> {};
template <typename R, typename... A>
struct is_lvalue<R(A...) const&&> : bool_constant<false> {};
template <typename R, typename... A>
struct is_lvalue<R(A...) const volatile&&> : bool_constant<false> {};
template <typename R, typename... A>
struct is_lvalue<R(A...) volatile&&> : bool_constant<false> {};

template <typename F>
struct is_rvalue;
template <typename R, typename... A>
struct is_rvalue<R(A...)> : bool_constant<false> {};
template <typename R, typename... A>
struct is_rvalue<R(A...) const> : bool_constant<false> {};
template <typename R, typename... A>
struct is_rvalue<R(A...) const volatile> : bool_constant<false> {};
template <typename R, typename... A>
struct is_rvalue<R(A...) volatile> : bool_constant<false> {};
template <typename R, typename... A>
struct is_rvalue<R(A...)&> : bool_constant<false> {};
template <typename R, typename... A>
struct is_rvalue<R(A...) const&> : bool_constant<false> {};
template <typename R, typename... A>
struct is_rvalue<R(A...) const volatile&> : bool_constant<false> {};
template <typename R, typename... A>
struct is_rvalue<R(A...) volatile&> : bool_constant<false> {};
template <typename R, typename... A>
struct is_rvalue<R(A...) &&> : bool_constant<true> {};
template <typename R, typename... A>
struct is_rvalue<R(A...) const&&> : bool_constant<true> {};
template <typename R, typename... A>
struct is_rvalue<R(A...) const volatile&&> : bool_constant<true> {};
template <typename R, typename... A>
struct is_rvalue<R(A...) volatile&&> : bool_constant<true> {};

template <typename F>
struct is_noexcept;
template <typename R, typename... A>
struct is_noexcept<R(A...)> : bool_constant<false> {};
template <typename R, typename... A>
struct is_noexcept<R(A...) const> : bool_constant<false> {};
template <typename R, typename... A>
struct is_noexcept<R(A...) const volatile> : bool_constant<false> {};
template <typename R, typename... A>
struct is_noexcept<R(A...) volatile> : bool_constant<false> {};
template <typename R, typename... A>
struct is_noexcept<R(A...)&> : bool_constant<false> {};
template <typename R, typename... A>
struct is_noexcept<R(A...) const&> : bool_constant<false> {};
template <typename R, typename... A>
struct is_noexcept<R(A...) const volatile&> : bool_constant<false> {};
template <typename R, typename... A>
struct is_noexcept<R(A...) volatile&> : bool_constant<false> {};
template <typename R, typename... A>
struct is_noexcept<R(A...) &&> : bool_constant<false> {};
template <typename R, typename... A>
struct is_noexcept<R(A...) const&&> : bool_constant<false> {};
template <typename R, typename... A>
struct is_noexcept<R(A...) const volatile&&> : bool_constant<false> {};
template <typename R, typename... A>
struct is_noexcept<R(A...) volatile&&> : bool_constant<false> {};

template <typename F>
struct argument_list;
template <typename F>
using argument_list_t = typename argument_list<F>::type;
template <typename R, typename... A>
struct argument_list<R(A...)> : type_identity<type_list<A...>> {};
template <typename R, typename... A>
struct argument_list<R(A...) const> : type_identity<type_list<A...>> {};
template <typename R, typename... A>
struct argument_list<R(A...) const volatile> : type_identity<type_list<A...>> {};
template <typename R, typename... A>
struct argument_list<R(A...) volatile> : type_identity<type_list<A...>> {};
template <typename R, typename... A>
struct argument_list<R(A...)&> : type_identity<type_list<A...>> {};
template <typename R, typename... A>
struct argument_list<R(A...) const&> : type_identity<type_list<A...>> {};
template <typename R, typename... A>
struct argument_list<R(A...) const volatile&> : type_identity<type_list<A...>> {};
template <typename R, typename... A>
struct argument_list<R(A...) volatile&> : type_identity<type_list<A...>> {};
template <typename R, typename... A>
struct argument_list<R(A...) &&> : type_identity<type_list<A...>> {};
template <typename R, typename... A>
struct argument_list<R(A...) const&&> : type_identity<type_list<A...>> {};
template <typename R, typename... A>
struct argument_list<R(A...) const volatile&&> : type_identity<type_list<A...>> {};
template <typename R, typename... A>
struct argument_list<R(A...) volatile&&> : type_identity<type_list<A...>> {};

#if UTL_CXX17

template <typename R, typename... A>
struct function_type<R(A...) noexcept> {
    using type = R(A...) noexcept;
    using return_type = R;
};

template <typename R, typename... A>
struct function_type<R(A...) const noexcept> {
    using type = R(A...) const noexcept;
    using return_type = R;
};

template <typename R, typename... A>
struct function_type<R(A...) const volatile noexcept> {
    using type = R(A...) const volatile noexcept;
    using return_type = R;
};

template <typename R, typename... A>
struct function_type<R(A...) volatile noexcept> {
    using type = R(A...) volatile noexcept;
    using return_type = R;
};

template <typename R, typename... A>
struct function_type<R(A...) & noexcept> {
    using type = R(A...);
    using return_type = R;
};

template <typename R, typename... A>
struct function_type<R(A...) const & noexcept> {
    using type = R(A...) const&;
    using return_type = R;
};

template <typename R, typename... A>
struct function_type<R(A...) const volatile & noexcept> {
    using type = R(A...) const volatile&;
    using return_type = R;
};

template <typename R, typename... A>
struct function_type<R(A...) volatile & noexcept> {
    using type = R(A...) volatile&;
    using return_type = R;
};

template <typename R, typename... A>
struct function_type<R(A...) && noexcept> {
    using type = R(A...);
    using return_type = R;
};

template <typename R, typename... A>
struct function_type<R(A...) const && noexcept> {
    using type = R(A...) const&&;
    using return_type = R;
};

template <typename R, typename... A>
struct function_type<R(A...) const volatile && noexcept> {
    using type = R(A...) const volatile&&;
    using return_type = R;
};

template <typename R, typename... A>
struct function_type<R(A...) volatile && noexcept> {
    using type = R(A...) volatile&&;
    using return_type = R;
};

template <typename R, typename... A>
struct is_const<R(A...) noexcept> : bool_constant<false> {};
template <typename R, typename... A>
struct is_const<R(A...) const noexcept> : bool_constant<true> {};
template <typename R, typename... A>
struct is_const<R(A...) const volatile noexcept> : bool_constant<true> {};
template <typename R, typename... A>
struct is_const<R(A...) volatile noexcept> : bool_constant<false> {};
template <typename R, typename... A>
struct is_const<R(A...) & noexcept> : bool_constant<false> {};
template <typename R, typename... A>
struct is_const<R(A...) const & noexcept> : bool_constant<true> {};
template <typename R, typename... A>
struct is_const<R(A...) const volatile & noexcept> : bool_constant<true> {};
template <typename R, typename... A>
struct is_const<R(A...) volatile & noexcept> : bool_constant<false> {};
template <typename R, typename... A>
struct is_const<R(A...) && noexcept> : bool_constant<false> {};
template <typename R, typename... A>
struct is_const<R(A...) const && noexcept> : bool_constant<true> {};
template <typename R, typename... A>
struct is_const<R(A...) const volatile && noexcept> : bool_constant<true> {};
template <typename R, typename... A>
struct is_const<R(A...) volatile && noexcept> : bool_constant<false> {};

template <typename R, typename... A>
struct is_volatile<R(A...) noexcept> : bool_constant<false> {};
template <typename R, typename... A>
struct is_volatile<R(A...) const noexcept> : bool_constant<false> {};
template <typename R, typename... A>
struct is_volatile<R(A...) const volatile noexcept> : bool_constant<true> {};
template <typename R, typename... A>
struct is_volatile<R(A...) volatile noexcept> : bool_constant<true> {};
template <typename R, typename... A>
struct is_volatile<R(A...) & noexcept> : bool_constant<false> {};
template <typename R, typename... A>
struct is_volatile<R(A...) const & noexcept> : bool_constant<false> {};
template <typename R, typename... A>
struct is_volatile<R(A...) const volatile & noexcept> : bool_constant<true> {};
template <typename R, typename... A>
struct is_volatile<R(A...) volatile & noexcept> : bool_constant<true> {};
template <typename R, typename... A>
struct is_volatile<R(A...) && noexcept> : bool_constant<false> {};
template <typename R, typename... A>
struct is_volatile<R(A...) const && noexcept> : bool_constant<false> {};
template <typename R, typename... A>
struct is_volatile<R(A...) const volatile && noexcept> : bool_constant<true> {};
template <typename R, typename... A>
struct is_volatile<R(A...) volatile && noexcept> : bool_constant<true> {};

template <typename R, typename... A>
struct is_cv<R(A...) noexcept> : bool_constant<false> {};
template <typename R, typename... A>
struct is_cv<R(A...) const noexcept> : bool_constant<false> {};
template <typename R, typename... A>
struct is_cv<R(A...) const volatile noexcept> : bool_constant<true> {};
template <typename R, typename... A>
struct is_cv<R(A...) volatile noexcept> : bool_constant<false> {};
template <typename R, typename... A>
struct is_cv<R(A...) & noexcept> : bool_constant<false> {};
template <typename R, typename... A>
struct is_cv<R(A...) const & noexcept> : bool_constant<false> {};
template <typename R, typename... A>
struct is_cv<R(A...) const volatile & noexcept> : bool_constant<true> {};
template <typename R, typename... A>
struct is_cv<R(A...) volatile & noexcept> : bool_constant<false> {};
template <typename R, typename... A>
struct is_cv<R(A...) && noexcept> : bool_constant<false> {};
template <typename R, typename... A>
struct is_cv<R(A...) const && noexcept> : bool_constant<false> {};
template <typename R, typename... A>
struct is_cv<R(A...) const volatile && noexcept> : bool_constant<true> {};
template <typename R, typename... A>
struct is_cv<R(A...) volatile && noexcept> : bool_constant<false> {};

template <typename R, typename... A>
struct is_lvalue<R(A...) noexcept> : bool_constant<false> {};
template <typename R, typename... A>
struct is_lvalue<R(A...) const noexcept> : bool_constant<false> {};
template <typename R, typename... A>
struct is_lvalue<R(A...) const volatile noexcept> : bool_constant<false> {};
template <typename R, typename... A>
struct is_lvalue<R(A...) volatile noexcept> : bool_constant<false> {};
template <typename R, typename... A>
struct is_lvalue<R(A...) & noexcept> : bool_constant<true> {};
template <typename R, typename... A>
struct is_lvalue<R(A...) const & noexcept> : bool_constant<true> {};
template <typename R, typename... A>
struct is_lvalue<R(A...) const volatile & noexcept> : bool_constant<true> {};
template <typename R, typename... A>
struct is_lvalue<R(A...) volatile & noexcept> : bool_constant<true> {};
template <typename R, typename... A>
struct is_lvalue<R(A...) && noexcept> : bool_constant<false> {};
template <typename R, typename... A>
struct is_lvalue<R(A...) const && noexcept> : bool_constant<false> {};
template <typename R, typename... A>
struct is_lvalue<R(A...) const volatile && noexcept> : bool_constant<false> {};
template <typename R, typename... A>
struct is_lvalue<R(A...) volatile && noexcept> : bool_constant<false> {};

template <typename R, typename... A>
struct is_rvalue<R(A...) noexcept> : bool_constant<false> {};
template <typename R, typename... A>
struct is_rvalue<R(A...) const noexcept> : bool_constant<false> {};
template <typename R, typename... A>
struct is_rvalue<R(A...) const volatile noexcept> : bool_constant<false> {};
template <typename R, typename... A>
struct is_rvalue<R(A...) volatile noexcept> : bool_constant<false> {};
template <typename R, typename... A>
struct is_rvalue<R(A...) & noexcept> : bool_constant<false> {};
template <typename R, typename... A>
struct is_rvalue<R(A...) const & noexcept> : bool_constant<false> {};
template <typename R, typename... A>
struct is_rvalue<R(A...) const volatile & noexcept> : bool_constant<false> {};
template <typename R, typename... A>
struct is_rvalue<R(A...) volatile & noexcept> : bool_constant<false> {};
template <typename R, typename... A>
struct is_rvalue<R(A...) && noexcept> : bool_constant<true> {};
template <typename R, typename... A>
struct is_rvalue<R(A...) const && noexcept> : bool_constant<true> {};
template <typename R, typename... A>
struct is_rvalue<R(A...) const volatile && noexcept> : bool_constant<true> {};
template <typename R, typename... A>
struct is_rvalue<R(A...) volatile && noexcept> : bool_constant<true> {};

template <typename R, typename... A>
struct is_noexcept<R(A...) noexcept> : bool_constant<true> {};
template <typename R, typename... A>
struct is_noexcept<R(A...) const noexcept> : bool_constant<true> {};
template <typename R, typename... A>
struct is_noexcept<R(A...) const volatile noexcept> : bool_constant<true> {};
template <typename R, typename... A>
struct is_noexcept<R(A...) volatile noexcept> : bool_constant<true> {};
template <typename R, typename... A>
struct is_noexcept<R(A...) & noexcept> : bool_constant<true> {};
template <typename R, typename... A>
struct is_noexcept<R(A...) const & noexcept> : bool_constant<true> {};
template <typename R, typename... A>
struct is_noexcept<R(A...) const volatile & noexcept> : bool_constant<true> {};
template <typename R, typename... A>
struct is_noexcept<R(A...) volatile & noexcept> : bool_constant<true> {};
template <typename R, typename... A>
struct is_noexcept<R(A...) && noexcept> : bool_constant<true> {};
template <typename R, typename... A>
struct is_noexcept<R(A...) const && noexcept> : bool_constant<true> {};
template <typename R, typename... A>
struct is_noexcept<R(A...) const volatile && noexcept> : bool_constant<true> {};
template <typename R, typename... A>
struct is_noexcept<R(A...) volatile && noexcept> : bool_constant<true> {};

template <typename R, typename... A>
struct argument_list<R(A...) noexcept> : type_identity<type_list<A...>> {};
template <typename R, typename... A>
struct argument_list<R(A...) const noexcept> : type_identity<type_list<A...>> {};
template <typename R, typename... A>
struct argument_list<R(A...) const volatile noexcept> : type_identity<type_list<A...>> {};
template <typename R, typename... A>
struct argument_list<R(A...) volatile noexcept> : type_identity<type_list<A...>> {};
template <typename R, typename... A>
struct argument_list<R(A...) & noexcept> : type_identity<type_list<A...>> {};
template <typename R, typename... A>
struct argument_list<R(A...) const & noexcept> : type_identity<type_list<A...>> {};
template <typename R, typename... A>
struct argument_list<R(A...) const volatile & noexcept> : type_identity<type_list<A...>> {};
template <typename R, typename... A>
struct argument_list<R(A...) volatile & noexcept> : type_identity<type_list<A...>> {};
template <typename R, typename... A>
struct argument_list<R(A...) && noexcept> : type_identity<type_list<A...>> {};
template <typename R, typename... A>
struct argument_list<R(A...) const && noexcept> : type_identity<type_list<A...>> {};
template <typename R, typename... A>
struct argument_list<R(A...) const volatile && noexcept> : type_identity<type_list<A...>> {};
template <typename R, typename... A>
struct argument_list<R(A...) volatile && noexcept> : type_identity<type_list<A...>> {};

#endif // if UTL_CXX17

} // namespace functional

template <typename F>
struct function_traits : function_type<F> {
    using function_type::return_type;
    using function_type::type;
    using argument_list = functional::argument_list_t<F>;
    static_assert(is_same<F, type>::value,
        "Function pointers and references are not allowed as a template parameter");

    static constexpr bool is_const = functional::is_const<F>::value;
    static constexpr bool is_volatile = functional::is_volatile<F>::value;
    static constexpr bool is_cv = functional::is_cv<F>::value;
    static constexpr bool is_lvalue = functional::is_lvalue<F>::value;
    static constexpr bool is_rvalue = functional::is_rvalue<F>::value;
    static constexpr bool is_noexcept = functional::is_noexcept<F>::value;
};

template <size_t I, typename F>
struct template_element<I, function_traits<F>> :
    template_element<I, functional::argument_list_t<F>> {};
template <size_t I, typename F>
struct template_element<I, function_type<F>> : template_element<I, function_traits<F>> {};

template <typename F>
struct template_size<function_traits<F>> : template_size<functional::argument_list_t<F>> {};
template <typename F>
struct template_size<function_type<F>> : template_size<function_traits<F>> {};

template <typename T, typename F>
struct template_count<T, function_traits<F>> : template_count<T, functional::argument_list_t<F>> {};
template <typename T, typename F>
struct template_count<T, function_type<F>> : template_count<T, function_traits<F>> {};

UTL_NAMESPACE_END

namespace std {
template <typename F>
struct tuple_size<UTL_SCOPE function_type<F>> :
    UTL_SCOPE template_size<UTL_SCOPE function_type<F>> {};
template <typename F>
struct tuple_size<UTL_SCOPE function_traits<F>> :
    UTL_SCOPE template_size<UTL_SCOPE function_traits<F>> {};
template <size_t I, typename F>
struct tuple_element<I, UTL_SCOPE function_type<F>> :
    UTL_SCOPE template_element<I, UTL_SCOPE function_type<F>> {};
template <size_t I, typename F>
struct tuple_element<I, UTL_SCOPE function_traits<F>> :
    UTL_SCOPE template_element<I, UTL_SCOPE function_traits<F>> {};
} // namespace std
