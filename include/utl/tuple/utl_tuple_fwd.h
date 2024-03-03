// Copyright 2023-2024 Bryan Wong

#pragma once

#if defined(UTL_USE_STD_tuple) && UTL_USE_STD_tuple

#else    // defined(UTL_USE_STD_tuple) && UTL_USE_STD_tuple


UTL_STD_NAMESPACE_BEGIN
/* UTL_UNDEFINED_BEHAVIOUR */
/** 
 * @note: Might have to bite the bullet on this UB, <tuple> or any other header is too massive
 * Can't define a utl version as the std version interacts with the core language
 * for e.g. structured bindings
 * RANT: Things like these that interact with core language SHOULD provide forward declarations
 */
template<typename>
struct tuple_size;
template<decltype(sizeof(0)), typename>
struct tuple_element;
UTL_STD_NAMESPACE_END

UTL_NAMESPACE_BEGIN

using size_t = decltype(sizeof(0));

template<typename>
struct tuple_size;
template<size_t, typename>
struct tuple_element;
template<typename T>
struct is_tuple;

#ifdef UTL_CXX14

template<typename T>
UTL_INLINE_CXX17 constexpr size_t tuple_size_v = tuple_size<T>::value;
template<typename T>
UTL_INLINE_CXX17 constexpr bool is_tuple_v = is_tuple<T>::value;

#endif

template<size_t I, typename T>
using tuple_element_t = typename tuple_element<I, T>::type;

template<typename...>
class tuple;

UTL_NAMESPACE_END

#endif  // defined(UTL_USE_STD_tuple) && UTL_USE_STD_tuple
