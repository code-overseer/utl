// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

#include "utl/iterator/utl_iterator_traits_fwd.h"

#include "utl/iterator/utl_iterator_tags.h"

#if UTL_CXX20

#  include "utl/concepts/utl_derived_from.h"

UTL_NAMESPACE_BEGIN

namespace details {
namespace iterator_concept {

template <typename T>
struct trait_type {
    using type UTL_NODEBUG = T;
};
template <__UTL details::iterator_traits::is_specialized T>
struct trait_type<T> {
    using type UTL_NODEBUG = __UTL iterator_traits<T>;
};

template <typename T>
using trait_type_t UTL_NODEBUG = typename trait_type<T>::type;

template <typename T>
struct tag_type {};

template <typename T>
concept with_iterator_concept = requires { typename trait_type_t<T>::iterator_concept; };
template <typename T>
concept with_iterator_category = requires { typename trait_type_t<T>::iterator_category; };

template <with_iterator_concept T>
struct tag_type<T> {
    using type UTL_NODEBUG = typename trait_type_t<T>::iterator_concept;
};

template <with_iterator_category T>
requires (!with_iterator_concept<T>)
struct tag_type<T> {
    using type UTL_NODEBUG = typename trait_type_t<T>::iterator_category;
};

template <typename T>
requires (!with_iterator_category<T> && !with_iterator_concept<T> &&
    !__UTL details::iterator_traits::is_specialized<T>)
struct tag_type<T> {
    using type UTL_NODEBUG = __UTL random_access_iterator_tag;
};

template <typename T>
using tag_type_t UTL_NODEBUG = typename tag_type<T>::type;

template <typename T, typename Tag>
concept implements = iterator_tag<Tag> && requires {
    typename tag_type<T>::type;
    requires __UTL derived_from<tag_type_t<T>, Tag>;
};

} // namespace iterator_concept
} // namespace details

UTL_NAMESPACE_END

#else // UTL_CXX20

#  include "utl/type_traits/utl_constants.h"
#  include "utl/type_traits/utl_is_base_of.h"

UTL_NAMESPACE_BEGIN
namespace details {
namespace iterator_concept {

template <typename T, typename = void>
struct trait_type {
    using type UTL_NODEBUG = T;
};
template <typename T>
struct trait_type<T, enable_if_t<__UTL details::iterator_traits::is_specialized<T>::value>> {
    using type UTL_NODEBUG = __UTL iterator_traits<T>;
};

template <typename T>
using trait_type_t UTL_NODEBUG = typename trait_type<T>::type;

template <typename T, typename = void>
struct has_member_iterator_concept : __UTL false_type {};
template <typename T>
struct has_member_iterator_concept<T, __UTL void_t<typename trait_type_t<T>::iterator_concept>> :
    __UTL true_type {};
template <typename T, typename = void>
struct has_member_iterator_category : __UTL false_type {};
template <typename T>
struct has_member_iterator_category<T, __UTL void_t<typename trait_type_t<T>::iterator_category>> :
    __UTL true_type {};

template <typename T UTL_REQUIRES_CXX11(has_member_iterator_concept<T>::value)>
__UTL_HIDE_FROM_ABI auto resolve(int) noexcept -> typename trait_type_t<T>::iterator_concept;

template <typename T UTL_REQUIRES_CXX11(
    !has_member_iterator_concept<T>::value && has_member_iterator_category<T>::value)>
__UTL_HIDE_FROM_ABI auto resolve(int) noexcept -> typename trait_type_t<T>::iterator_category;

template <typename T UTL_REQUIRES_CXX11(!has_member_iterator_concept<T>::value &&
    !has_member_iterator_category<T>::value &&
    !__UTL details::iterator_traits::is_specialized<T>::value)>
__UTL_HIDE_FROM_ABI auto resolve(int) noexcept -> __UTL random_access_iterator_tag;

template <typename T>
struct tag_type {
    using type UTL_NODEBUG = decltype(__UTL details::iterator_concept::resolve<T>(0));
};

template <typename T>
using tag_type_t UTL_NODEBUG = decltype(__UTL details::iterator_concept::resolve<T>(0));

template <typename Tag, typename T, typename = void>
struct implements : __UTL false_type {};

template <typename Tag, typename T>
struct implements<Tag, T,
    __UTL enable_if_t<UTL_TRAIT_is_iterator_tag(Tag) && UTL_TRAIT_is_base_of(Tag, tag_type_t<T>)>> :
    __UTL true_type {};

} // namespace iterator_concept
} // namespace details

UTL_NAMESPACE_END

#endif // UTL_CXX20

UTL_NAMESPACE_BEGIN
template <typename T>
using iterator_concept_t = typename __UTL details::iterator_concept::tag_type<T>::type;

UTL_NAMESPACE_END
