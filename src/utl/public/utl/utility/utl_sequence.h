// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_config.h"

#if UTL_CXX14 && UTL_USE_STD_sequence_utility

#  include <utility>

UTL_NAMESPACE_BEGIN

using std::index_sequence;
using std::index_sequence_for;
using std::integer_sequence;
using std::make_index_sequence;

UTL_NAMESPACE_END

#else // UTL_CXX14 && UTL_USE_STD_sequence_utility

#  if UTL_USE_STD_sequence_utility
UTL_PRAGMA_WARN("The current standard does not implement integer_sequence, "
                "`UTL_USE_STD_sequence_utility` ignored")
#  endif // UTL_USE_STD_sequence_utility

UTL_NAMESPACE_BEGIN

using size_t = decltype(sizeof(0));

template <typename T, T... Is>
struct integer_sequence {};

namespace details {
namespace sequence {

template <typename, typename>
struct combine;

template <typename T, T... Is, T... Js>
struct combine<integer_sequence<T, Is...>, integer_sequence<T, Js...>> {
    using type = integer_sequence<T, Is..., (sizeof...(Is) + Js)...>;
};

template <typename T>
struct combine<integer_sequence<T>, integer_sequence<T>> {
    using type = integer_sequence<T>;
};

template <typename T, size_t N>
struct generate;

template <typename T>
struct generate<T, 0> {
    using type = integer_sequence<T>;
};

template <typename T>
struct generate<T, 1> {
    using type = integer_sequence<T, 0>;
};

template <typename T, size_t N>
struct generate :
    combine<typename generate<T, N / 2>::type, typename generate<T, N - N / 2>::type> {};

template <typename T, T N, bool = (N >= 0)>
struct safe_generate;

template <typename T, T N>
struct safe_generate<T, N, true> : generate<T, (size_t)N> {};

} // namespace sequence
} // namespace details

template <typename T, T N>
using make_integer_sequence = typename details::sequence::safe_generate<T, N>::type;

template <size_t... Is>
using index_sequence = integer_sequence<size_t, Is...>;

template <size_t N>
using make_index_sequence = make_integer_sequence<size_t, N>;

template <typename... Ts>
using index_sequence_for = make_index_sequence<sizeof...(Ts)>;

UTL_NAMESPACE_END

#endif // UTL_CXX14 && defined(UTL_USE_STD_sequence_utility) &&
       // UTL_USE_STD_sequence_utility
