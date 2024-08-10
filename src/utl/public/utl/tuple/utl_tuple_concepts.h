// Copyright 2023-2024 Bryan Wong

#pragma once

#if UTL_CXX20

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

template <typename T>
concept tuple_like =
    requires { tuple_size<T>::value; } && UTL_SCOPE tuple_traits::is_all_gettable<T>::value;

UTL_NAMESPACE_END

#endif // if UTL_CXX20
