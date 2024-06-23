// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/iterator/utl_iterator_tags.h"
#include "utl/iterator/utl_iterator_traits_fwd.h"
#include "utl/preprocessor/utl_config.h"

#if UTL_CXX20

#  include "utl/concepts/utl_derived_from.h"

UTL_NAMESPACE_BEGIN

namespace details {
namespace iterator_concept {

template <typename T>
struct trait_type {
    using type = T;
};
template <UTL_SCOPE details::iterator_traits::is_specialized T>
struct trait_type<T> {
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

template <typename T, typename Tag>
concept implements = iterator_tag<Tag> && requires {
    typename tag_type<T>::type;
    requires UTL_SCOPE derived_from<tag_type_t<T>, Tag>;
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
struct tag_type {
    using type = decltype(UTL_SCOPE details::iterator_concept::resolve<T>(0));
};

template <typename T>
using tag_type_t = decltype(UTL_SCOPE details::iterator_concept::resolve<T>(0));

template <typename Tag, typename T, typename = void>
struct implements : UTL_SCOPE false_type {};

template <typename Tag, typename T>
struct implements<Tag, T,
    UTL_SCOPE
        enable_if_t<UTL_TRAIT_is_iterator_tag(Tag) && UTL_TRAIT_is_base_of(Tag, tag_type_t<T>)>> :
    UTL_SCOPE true_type {};

} // namespace iterator_concept
} // namespace details

UTL_NAMESPACE_END

#endif // UTL_CXX20

UTL_NAMESPACE_BEGIN
template <typename T>
using iterator_concept_t = typename UTL_SCOPE details::iterator_concept::tag_type<T>::type;

UTL_NAMESPACE_END
