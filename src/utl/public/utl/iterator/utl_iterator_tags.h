// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_config.h"
#include "utl/type_traits/utl_constants.h"

UTL_NAMESPACE_BEGIN

struct input_iterator_tag {};
struct output_iterator_tag {};
struct forward_iterator_tag : input_iterator_tag {};
struct bidirectional_iterator_tag : forward_iterator_tag {};
struct random_access_iterator_tag : bidirectional_iterator_tag {};
struct contiguous_iterator_tag : random_access_iterator_tag {};

template <typename T>
struct is_iterator_tag : UTL_SCOPE false_type {};
template <>
struct is_iterator_tag<input_iterator_tag> : UTL_SCOPE true_type {};
template <>
struct is_iterator_tag<output_iterator_tag> : UTL_SCOPE true_type {};
template <>
struct is_iterator_tag<forward_iterator_tag> : UTL_SCOPE true_type {};
template <>
struct is_iterator_tag<bidirectional_iterator_tag> : UTL_SCOPE true_type {};
template <>
struct is_iterator_tag<random_access_iterator_tag> : UTL_SCOPE true_type {};
template <>
struct is_iterator_tag<contiguous_iterator_tag> : UTL_SCOPE true_type {};

#if UTL_CXX14
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_iterator_tag_v = false;
template <>
UTL_INLINE_CXX17 constexpr bool is_iterator_tag_v<input_iterator_tag> = true;
template <>
UTL_INLINE_CXX17 constexpr bool is_iterator_tag_v<output_iterator_tag> = true;
template <>
UTL_INLINE_CXX17 constexpr bool is_iterator_tag_v<forward_iterator_tag> = true;
template <>
UTL_INLINE_CXX17 constexpr bool is_iterator_tag_v<bidirectional_iterator_tag> = true;
template <>
UTL_INLINE_CXX17 constexpr bool is_iterator_tag_v<random_access_iterator_tag> = true;
template <>
UTL_INLINE_CXX17 constexpr bool is_iterator_tag_v<contiguous_iterator_tag> = true;
#endif

#if UTL_CXX20
template <typename T>
concept iterator_tag = is_iterator_tag_v<T>;
#endif

UTL_NAMESPACE_END

#if UTL_CXX14
#  define UTL_TRAIT_is_iterator_tag(...) UTL_SCOPE is_iterator_tag_v<__VA_ARGS__>
#else
#  define UTL_TRAIT_is_iterator_tag(...) UTL_SCOPE is_iterator_tag<__VA_ARGS__>::value
#endif
