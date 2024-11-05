// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

#include "utl/iterator/utl_iterator_traits_fwd.h"

#include "utl/iterator/utl_iterator_tags.h"
#include "utl/iterator/utl_legacy_bidirectional_iterator.h"
#include "utl/iterator/utl_legacy_forward_iterator.h"
#include "utl/iterator/utl_legacy_input_iterator.h"
#include "utl/iterator/utl_legacy_iterator.h"
#include "utl/iterator/utl_legacy_output_iterator.h"
#include "utl/iterator/utl_legacy_random_access_iterator.h"
#include "utl/memory/utl_pointer_traits.h"
#include "utl/type_traits/utl_declval.h"
#include "utl/type_traits/utl_remove_cv.h"

#if UTL_CXX20

#  include "utl/concepts/utl_object_type.h"

UTL_NAMESPACE_BEGIN

namespace details {
namespace iterator_traits {
template <typename T>
concept non_pointer_iter = requires {
    typename T::difference_type;
    typename T::value_type;
    typename T::reference;
    typename T::iterator_category;
};
template <typename T>
concept simple_iter = non_pointer_iter<T> && requires { typename T::pointer; };

template <typename Iter>
struct pointer_type {
    using type UTL_NODEBUG = void;
};

template <typename Iter>
requires requires { typename Iter::pointer; }
struct pointer_type<Iter> {
    using type UTL_NODEBUG = typename Iter::pointer;
};

template <typename Iter>
requires (
    !requires { typename Iter::pointer; } && requires(Iter it) { it.operator->(); })
struct pointer_type<Iter> {
    using type UTL_NODEBUG = decltype(__UTL declval<Iter&>().operator->());
};

template <typename Iter>
struct reference_type {
    using type UTL_NODEBUG = __UTL iter_reference_t<Iter>;
};

template <typename Iter>
requires requires { typename Iter::reference; }
struct reference_type<Iter> {
    using type UTL_NODEBUG = typename Iter::reference;
};

template <typename Iter>
struct category_type;

template <__UTL legacy_random_access_iterator Iter>
struct category_type<Iter> {
    using type UTL_NODEBUG = __UTL random_access_iterator_tag;
};

template <__UTL legacy_bidirectional_iterator Iter>
struct category_type<Iter> {
    using type UTL_NODEBUG = __UTL bidirectional_iterator_tag;
};

template <__UTL legacy_forward_iterator Iter>
struct category_type<Iter> {
    using type UTL_NODEBUG = __UTL forward_iterator_tag;
};

template <__UTL legacy_input_iterator Iter>
struct category_type<Iter> {
    using type UTL_NODEBUG = __UTL input_iterator_tag;
};

template <typename Iter>
struct difference_type {
    using type UTL_NODEBUG = void;
};

template <typename Iter>
requires requires { typename __UTL incrementable_traits<Iter>::difference_type; }
struct difference_type<Iter> {
    using type UTL_NODEBUG = typename __UTL incrementable_traits<Iter>::difference_type;
};

template <typename Iter>
struct legacy_traits : private __UTL details::iterator_traits::impl_tag<Iter> {};

template <__UTL legacy_input_iterator Iter>
struct legacy_traits<Iter> : private __UTL details::iterator_traits::impl_tag<Iter> {
    using difference_type = typename __UTL incrementable_traits<Iter>::difference_type;
    using value_type = typename __UTL indirectly_readable_traits<Iter>::value_type;
    using pointer = typename __UTL details::iterator_traits::pointer_type<Iter>::type;
    using reference = typename __UTL details::iterator_traits::reference_type<Iter>::type;
    using iterator_category = typename __UTL details::iterator_traits::category_type<Iter>::type;
};

template <__UTL legacy_iterator Iter>
struct legacy_traits<Iter> : private __UTL details::iterator_traits::impl_tag<Iter> {
    using difference_type = typename __UTL details::iterator_traits::difference_type<Iter>::type;
    using value_type = void;
    using pointer = void;
    using reference = void;
    using iterator_category = __UTL output_iterator_tag;
};

} // namespace iterator_traits
} // namespace details

template <details::iterator_traits::simple_iter Iter>
struct __UTL_PUBLIC_TEMPLATE iterator_traits<Iter> :
    private details::iterator_traits::impl_tag<Iter> {
    using difference_type = typename Iter::difference_type;
    using value_type = typename Iter::value_type;
    using pointer = typename Iter::pointer;
    using reference = typename Iter::reference;
    using iterator_category = typename Iter::iterator_category;
};

template <details::iterator_traits::non_pointer_iter Iter>
struct __UTL_PUBLIC_TEMPLATE iterator_traits<Iter> :
    private details::iterator_traits::impl_tag<Iter> {
    using difference_type = typename Iter::difference_type;
    using value_type = typename Iter::value_type;
    using pointer = void;
    using reference = typename Iter::reference;
    using iterator_category = typename Iter::iterator_category;
};

template <object_type T>
struct __UTL_PUBLIC_TEMPLATE iterator_traits<T*> : private details::iterator_traits::impl_tag<T*> {
    using difference_type = pointer_traits<T*>::difference_type;
    using value_type = remove_cv_t<T>;
    using pointer = T*;
    using reference = T&;
    using iterator_category = random_access_iterator_tag;
    using iterator_concept = contiguous_iterator_tag;
};

template <typename T>
struct __UTL_PUBLIC_TEMPLATE iterator_traits : details::iterator_traits::legacy_traits<T> {
private:
    using base_type = details::iterator_traits::legacy_traits<T>;
};

UTL_NAMESPACE_END

#else

#  include "utl/type_traits/utl_has_member_difference_type.h"
#  include "utl/type_traits/utl_has_member_pointer.h"
#  include "utl/type_traits/utl_has_member_reference.h"
#  include "utl/type_traits/utl_has_member_value_type.h"
#  include "utl/type_traits/utl_is_object.h"
#  include "utl/type_traits/utl_logical_traits.h"
#  include "utl/type_traits/utl_remove_pointer.h"

UTL_NAMESPACE_BEGIN

namespace details {
namespace iterator_traits {

template <typename T>
using simple_iter UTL_NODEBUG =
    __UTL conjunction<__UTL has_member_difference_type<T>, __UTL has_member_value_type<T>,
        __UTL has_member_reference<T>, __UTL is_iterator_tag<typename T::iterator_category>>;

template <typename T, bool = simple_iter<T>::value, bool = UTL_TRAIT_has_member_pointer(T)>
struct traits;

template <typename Iter>
struct traits<Iter, true, true> {
    using difference_type = typename Iter::difference_type;
    using value_type = typename Iter::value_type;
    using pointer = typename Iter::pointer;
    using reference = typename Iter::reference;
    using iterator_category = typename Iter::iterator_category;
};

template <typename Iter>
struct traits<Iter, true, false> {
    using difference_type = typename Iter::difference_type;
    using value_type = typename Iter::value_type;
    using pointer = void;
    using reference = typename Iter::reference;
    using iterator_category = typename Iter::iterator_category;
};

template <typename Iter>
__UTL_HIDE_FROM_ABI auto pointer_type(int) noexcept -> typename Iter::pointer;
template <typename Iter>
__UTL_HIDE_FROM_ABI auto pointer_type(float) noexcept
    -> decltype(__UTL declval<Iter&>().operator->());
template <typename Iter>
__UTL_HIDE_FROM_ABI auto pointer_type(...) noexcept -> void;

template <typename Iter>
__UTL_HIDE_FROM_ABI auto reference_type(int) noexcept -> typename Iter::reference;
template <typename Iter>
__UTL_HIDE_FROM_ABI auto reference_type(float) noexcept -> __UTL iter_reference_t<Iter>;

template <typename Iter>
__UTL_HIDE_FROM_ABI auto category_type(int, int) noexcept -> __UTL
    enable_if_t<UTL_TRAIT_is_legacy_random_access_iterator(Iter), __UTL random_access_iterator_tag>;
template <typename Iter>
__UTL_HIDE_FROM_ABI auto category_type(int, float) noexcept -> __UTL
    enable_if_t<UTL_TRAIT_is_legacy_bidirectional_iterator(Iter), __UTL bidirectional_iterator_tag>;
template <typename Iter>
__UTL_HIDE_FROM_ABI auto category_type(float, float) noexcept
    -> __UTL enable_if_t<UTL_TRAIT_is_legacy_forward_iterator(Iter), __UTL forward_iterator_tag>;
template <typename Iter>
__UTL_HIDE_FROM_ABI auto category_type(...) noexcept -> __UTL input_iterator_tag;

template <typename Iter>
__UTL_HIDE_FROM_ABI auto difference_type(int) noexcept ->
    typename __UTL incrementable_traits<Iter>::difference_type;
template <typename Iter>
__UTL_HIDE_FROM_ABI auto difference_type(float) noexcept -> void;

template <typename Iter, bool = UTL_TRAIT_is_legacy_input_iterator(Iter),
    bool = UTL_TRAIT_is_legacy_iterator(Iter)>
struct legacy_traits {};

template <typename Iter>
struct legacy_traits<Iter, true, true> {
    using difference_type = typename __UTL incrementable_traits<Iter>::difference_type;
    using value_type = typename __UTL indirectly_readable_traits<Iter>::value_type;
    using pointer = decltype(__UTL details::iterator_traits::pointer_type<Iter>(0));
    using reference = decltype(__UTL details::iterator_traits::reference_type<Iter>(0));
    using iterator_category = decltype(category_type<Iter>(0, 0));
};

template <typename Iter>
struct legacy_traits<Iter, false, true> {
    using difference_type = decltype(difference_type<Iter>(0));
    using value_type = void;
    using pointer = void;
    using reference = void;
    using iterator_category = __UTL output_iterator_tag;
};

template <typename Iter, bool B>
struct traits<Iter, false, B> : legacy_traits<Iter> {};

template <typename T, bool = UTL_TRAIT_is_object(__UTL remove_pointer_t<T>)>
struct pointer_impl {};

template <typename T>
struct pointer_impl<T*, true> {
    using difference_type = typename __UTL pointer_traits<T*>::difference_type;
    using value_type = __UTL remove_cv_t<T>;
    using pointer = T*;
    using reference = T&;
    using iterator_category = __UTL random_access_iterator_tag;
    using iterator_concept = __UTL contiguous_iterator_tag;
};

} // namespace iterator_traits
} // namespace details

template <typename Iter>
struct __UTL_PUBLIC_TEMPLATE iterator_traits :
    details::iterator_traits::traits<Iter>,
    private details::iterator_traits::impl_tag<Iter> {};

template <typename T>
struct __UTL_PUBLIC_TEMPLATE iterator_traits<T*> :
    details::iterator_traits::pointer_impl<T*>,
    private details::iterator_traits::impl_tag<T*> {};

UTL_NAMESPACE_END

#endif
