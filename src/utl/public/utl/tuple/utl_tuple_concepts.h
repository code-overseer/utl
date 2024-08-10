// Copyright 2023-2024 Bryan Wong

#pragma once

#if UTL_CXX20

#  include "utl/concepts/utl_reference.h"
#  include "utl/tuple/utl_tuple_traits.h"

UTL_NAMESPACE_BEGIN

namespace tuple_traits {
template <typename T, size_t I>
concept gettable = requires {
    tuple_size<T>::value;
    typename tuple_element<I, T>::type;
    requires (I < tuple_size<T>::value);
    UTL_SCOPE tuple_traits::get<I>(declval<T>());
};
} // namespace tuple_traits

namespace details {
namespace tuple {
template <typename T>
concept has_size = requires { tuple_size<T>::value; };
template <typename T, size_t I>
concept has_element = requires { typename tuple_element<I, T>::type; };
} // namespace tuple
} // namespace details

template <typename T>
concept tuple_like =
    (!reference<T> && details::tuple::has_size<T> && []<size_t... Is>(index_sequence<Is...>) {
        return (... && details::tuple::has_element<T, Is>);
    }(make_index_sequence<tuple_size_v<T>>{}));

UTL_NAMESPACE_END

#endif // if UTL_CXX20
