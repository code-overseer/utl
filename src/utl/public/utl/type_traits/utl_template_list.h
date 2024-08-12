// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

#include "utl/type_traits/utl_common.h"

UTL_NAMESPACE_BEGIN

template <size_t I, typename List>
struct template_element {};

template <typename List>
struct template_size {};

template <size_t I, typename List>
using template_element_t = typename template_element<I, List>::type;

template <template <typename...> class List, typename Head, typename... Tail>
struct __UTL_PUBLIC_TEMPLATE template_element<0, List<Head, Tail...>> {
    using type UTL_NODEBUG = Head;
};

template <typename... T>
struct type_list {};

template <size_t I, template <typename...> class List, typename Head, typename... Tail>
struct __UTL_PUBLIC_TEMPLATE template_element<I, List<Head, Tail...>> :
    template_element<I - 1, type_list<Tail...>> {};

template <template <typename...> class List, typename... Args>
struct __UTL_PUBLIC_TEMPLATE template_size<List<Args...>> : size_constant<sizeof...(Args)> {};

template <typename T, typename List>
struct template_count {};

template <typename T, template <typename...> class List>
struct __UTL_PUBLIC_TEMPLATE template_count<T, List<>> : size_constant<0> {};

template <typename T, template <typename...> class List, typename... Args>
struct __UTL_PUBLIC_TEMPLATE template_count<T, List<T, Args...>> :
    integral_constant<size_t, 1 + template_count<T, type_list<Args...>>::value> {};

template <typename T, typename U, template <typename...> class List, typename... Args>
struct __UTL_PUBLIC_TEMPLATE template_count<T, List<U, Args...>> :
    integral_constant<size_t, template_count<T, type_list<Args...>>::value> {};

template <typename T, typename TList>
struct __UTL_PUBLIC_TEMPLATE template_index : integral_constant<size_t, ((size_t)-1)> {};

template <typename T, template <typename...> class TList, typename... Tail>
struct __UTL_PUBLIC_TEMPLATE template_index<T, TList<T, Tail...>> : integral_constant<size_t, 0> {};

template <typename T, template <typename...> class TList, typename Head, typename... Tail>
struct __UTL_PUBLIC_TEMPLATE template_index<T, TList<Head, Tail...>> :
    integral_constant<size_t, 1 + template_index<T, TList<Tail...>>::value> {};

template <typename T, typename U>
struct template_concat {};
template <template <typename...> class List, typename... Ts, typename... Us>
struct __UTL_PUBLIC_TEMPLATE template_concat<List<Ts...>, List<Us...>> {
    using type UTL_NODEBUG = List<Ts..., Us...>;
};
template <typename T, typename U>
using template_concat_t = typename template_concat<T, U>::type;

#if UTL_CXX17

template <auto... V>
struct auto_list;

template <template <auto...> class List, auto Head, auto... Tail>
struct __UTL_PUBLIC_TEMPLATE template_element<0, List<Head, Tail...>> {
    static constexpr auto value = Head;
};

template <size_t I, template <auto...> class List, auto Head, auto... Tail>
struct __UTL_PUBLIC_TEMPLATE template_element<I, List<Head, Tail...>> :
    template_element<I - 1, auto_list<Tail...>> {};

template <template <auto...> class List, auto... Tail>
struct __UTL_PUBLIC_TEMPLATE template_size<List<Tail...>> {
    static constexpr size_t value = sizeof...(Tail);
};

template <typename T, T Value, template <auto...> class List>
struct __UTL_PUBLIC_TEMPLATE template_count<value_constant<T, Value>, List<>> :
    integral_constant<size_t, 0> {};

template <typename T, T Value, template <auto...> class List, auto... Tail>
struct __UTL_PUBLIC_TEMPLATE template_count<value_constant<T, Value>, List<Value, Tail...>> :
    integral_constant<size_t,
        1 + template_count<value_constant<T, Value>, auto_list<Tail...>>::value> {};

template <auto V>
using auto_constant = value_constant<decltype(V), V>;

#endif // if UTL_CXX17

UTL_NAMESPACE_END