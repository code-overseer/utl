// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_config.h"
#include "utl/type_traits/utl_declval.h"
#include "utl/type_traits/utl_make_signed.h"

UTL_NAMESPACE_BEGIN

template <typename>
struct incrementable_traits;

using ptrdiff_t = decltype((char*)0 - (char*)0);

UTL_NAMESPACE_END

#if UTL_CXX20

#  include "utl/concepts/utl_integral.h"

UTL_NAMESPACE_BEGIN

template <typename T>
struct incrementable_traits : details::incrementable::traits<T> {};

template <typename T>
requires (UTL_TRAIT_is_object(T))
struct incrementable_traits<T*> {
    using difference_type = ptrdiff_t;
};

template <typename T>
requires requires { typename T::difference_type; }
struct incrementable_traits<T> {
    using difference_type = typename T::difference_type;
};

template <typename T>
requires (!requires { typename T::difference_type; }) && requires(T const& a, T const& b) {
    { a - b } -> integral;
}
struct incrementable_traits<T> {
    using difference_type = make_signed_t<decltype(declval<T const&>() - declval<T const&>())>;
};

template <typename T>
struct incrementable_traits<T const> : incrementable_traits<T> {};

UTL_NAMESPACE_END

#else // if UTL_CXX20

#  include "utl/type_traits/utl_enable_if.h"
#  include "utl/type_traits/utl_has_member_difference_type.h"
#  include "utl/type_traits/utl_void_t.h"

UTL_NAMESPACE_BEGIN

namespace details {
namespace incrementable {

template <typename T>
struct difference_identity {
    using difference_type = T;
};

template <typename T UTL_REQUIRES_CXX11(UTL_TRAIT_is_object(T))>
UTL_REQUIRES_CXX20(UTL_TRAIT_is_object(T))
using object_pointer = difference_identity<ptrdiff_t>;

template <typename T UTL_REQUIRES_CXX11(has_member_difference_type<T>::value)>
auto resolve_trait(int) noexcept -> difference_identity<typename T::difference_type>;

template <typename T UTL_REQUIRES_CXX11(!has_difference_type<T>::value &&
    UTL_TRAIT_is_integral(decltype(declval<T const&>()-declval<T const&>())))>
auto resolve_trait(int) noexcept
    -> difference_identity<make_signed_t<decltype(declval<T const&>() - declval<T const&>())>>;

struct fallback_t {};
template <typename T>
auto resolve_trait(float) noexcept -> fallback_t;

template <typename T>
using traits = decltype(resolve_trait<T>(0));

} // namespace incrementable
} // namespace details

template <typename T>
struct incrementable_traits : details::incrementable::traits<T> {};

template <typename T>
struct incrementable_traits<T*> : details::incrementable::object_pointer<T> {};

template <typename T>
struct incrementable_traits<T const> : incrementable_traits<T> {};

UTL_NAMESPACE_END

#endif // if UTL_CXX20
