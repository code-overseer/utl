// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

#include "utl/type_traits/utl_constants.h"

UTL_NAMESPACE_BEGIN

struct __UTL_ABI_PUBLIC input_iterator_tag {};
struct __UTL_ABI_PUBLIC output_iterator_tag {};
struct __UTL_ABI_PUBLIC forward_iterator_tag : input_iterator_tag {};
struct __UTL_ABI_PUBLIC bidirectional_iterator_tag : forward_iterator_tag {};
struct __UTL_ABI_PUBLIC random_access_iterator_tag : bidirectional_iterator_tag {};
struct __UTL_ABI_PUBLIC contiguous_iterator_tag : random_access_iterator_tag {};

template <typename T>
struct __UTL_PUBLIC_TEMPLATE is_iterator_tag : __UTL false_type {};
template <>
struct __UTL_PUBLIC_TEMPLATE is_iterator_tag<input_iterator_tag> : __UTL true_type {};
template <>
struct __UTL_PUBLIC_TEMPLATE is_iterator_tag<output_iterator_tag> : __UTL true_type {};
template <>
struct __UTL_PUBLIC_TEMPLATE is_iterator_tag<forward_iterator_tag> : __UTL true_type {};
template <>
struct __UTL_PUBLIC_TEMPLATE is_iterator_tag<bidirectional_iterator_tag> : __UTL true_type {};
template <>
struct __UTL_PUBLIC_TEMPLATE is_iterator_tag<random_access_iterator_tag> : __UTL true_type {};
template <>
struct __UTL_PUBLIC_TEMPLATE is_iterator_tag<contiguous_iterator_tag> : __UTL true_type {};

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
#  define UTL_TRAIT_is_iterator_tag(...) __UTL is_iterator_tag_v<__VA_ARGS__>
#else
#  define UTL_TRAIT_is_iterator_tag(...) __UTL is_iterator_tag<__VA_ARGS__>::value
#endif
