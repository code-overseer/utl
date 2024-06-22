// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/iterator/utl_indirectly_readable.h"
#include "utl/iterator/utl_input_or_output_iterator.h"
#include "utl/iterator/utl_iterator_tags.h"
#include "utl/iterator/utl_iterator_traits_fwd.h"
#include "utl/preprocessor/utl_config.h"
#include "utl/type_traits/utl_constants.h"

#if UTL_CXX20

#  include "utl/concepts/utl_derived_from.h"

UTL_NAMESPACE_BEGIN

namespace details {
namespace input_iterator {

template <typename T>
struct trait_type {
    using type = T;
};
template <UTL_SCOPE details::iterator_traits::is_specialized T>
struct trait_type {
    using type = UTL_SCOPE iterator_traits<T>;
};

template <typename T>
using trait_type_t = typename trait_type<T>::type;

template <typename T>
struct tag_type {};

template <typename T>
concept with_iterator_concept = requires { typename trait_type_t<T>::iterator_concept; };
template <typename T>
concept with_iterator_category = requires { typename trait_type_t<T>::iterator_category; };

template <with_iterator_concept T>
struct tag_type<T> {
    using type = typename trait_type_t<T>::iterator_concept;
};

template <with_iterator_category T>
requires (!with_iterator_concept<T>)
struct tag_type<T> {
    using type = typename trait_type_t<T>::iterator_category;
};

template <typename T>
requires (!with_iterator_category<T> && !with_iterator_concept<T> &&
    !UTL_SCOPE details::iterator_traits::is_specialized<T>)
struct tag_type<T> {
    using type = UTL_SCOPE random_access_iterator_tag;
};

template <typename T>
using tag_type_t = typename tag_type<T>::type;
} // namespace input_iterator
} // namespace details

template <typename T>
concept input_iterator =
    UTL_SCOPE input_or_output_iterator<T> && UTL_SCOPE indirectly_readable<T> &&
    requires { typename details::input_iterator::tag_type_t<T>; } &&
    UTL_SCOPE derived_from<details::input_iterator::tag_type_t<T>, UTL_SCOPE input_iterator_tag>;

template <typename T>
struct is_input_iterator : UTL_SCOPE bool_constant<input_iterator<T>> {};

template <typename T>
inline constexpr bool is_input_iterator_v = input_iterator<T>;

UTL_NAMESPACE_END
#else // UTL_CXX20

#  include "utl/type_traits/utl_is_base_of.h"

UTL_NAMESPACE_BEGIN
namespace details {
namespace input_iterator {

template <typename T, typename = void>
struct trait_type {
    using type = T;
};
template <typename T>
struct trait_type<T, enable_if_t<UTL_SCOPE details::iterator_traits::is_specialized<T>::value>> {
    using type = UTL_SCOPE iterator_traits<T>;
};

template <typename T>
using trait_type_t = typename trait_type<T>::type;

template <typename T, typename = void>
struct has_member_iterator_concept : UTL_SCOPE false_type {};
template <typename T>
struct has_member_iterator_concept<T,
    UTL_SCOPE void_t<typename trait_type_t<T>::iterator_concept>> : UTL_SCOPE true_type {};
template <typename T, typename = void>
struct has_member_iterator_category : UTL_SCOPE false_type {};
template <typename T>
struct has_member_iterator_category<T,
    UTL_SCOPE void_t<typename trait_type_t<T>::iterator_category>> : UTL_SCOPE true_type {};

template <typename T UTL_REQUIRES_CXX11(has_member_iterator_concept<T>::value)>
auto resolve(int) noexcept -> typename trait_type_t<T>::iterator_concept;

template <typename T UTL_REQUIRES_CXX11(
    !has_member_iterator_concept<T>::value && has_member_iterator_category<T>::value)>
auto resolve(int) noexcept -> typename trait_type_t<T>::iterator_category;

template <typename T UTL_REQUIRES_CXX11(!has_member_iterator_concept<T>::value &&
    !has_member_iterator_category<T>::value &&
    !UTL_SCOPE details::iterator_traits::is_specialized<T>::value)>
auto resolve(int) noexcept -> UTL_SCOPE random_access_iterator_tag;

template <typename T>
using tag_type_t = decltype(UTL_SCOPE details::input_iterator::resolve<T>(0));
template <typename T, typename = void>
struct has_tag : UTL_SCOPE false_type {};

template <typename T>
struct has_tag<T, UTL_SCOPE void_t<tag_type_t<T>>> :
    UTL_SCOPE is_base_of<UTL_SCOPE input_iterator_tag, tag_type_t<T>> {};

} // namespace input_iterator
} // namespace details

template <typename T>
struct is_input_iterator :
    UTL_SCOPE conjunction<UTL_SCOPE is_input_or_output_iterator<T>,
        UTL_SCOPE is_indirectly_readable<T>, UTL_SCOPE details::input_iterator::has_tag<T>> {};

#  if UTL_CXX14
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_input_iterator_v = input_iterator<T>;
#  endif

UTL_NAMESPACE_END

#endif
