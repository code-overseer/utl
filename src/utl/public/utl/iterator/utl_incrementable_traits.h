// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

#include "utl/type_traits/utl_declval.h"
#include "utl/type_traits/utl_make_signed.h"

UTL_NAMESPACE_BEGIN

template <typename>
struct incrementable_traits;
template <typename T>
struct __UTL_PUBLIC_TEMPLATE incrementable_traits<T const> : incrementable_traits<T> {};

using ptrdiff_t = decltype((char*)0 - (char*)0);

namespace details {
namespace incrementable {
template <typename T>
using subtract_result_t UTL_NODEBUG =
    __UTL make_signed_t<decltype(__UTL declval<T const&>() - __UTL declval<T const&>())>;
}
} // namespace details

UTL_NAMESPACE_END

#if UTL_CXX20

#  include "utl/concepts/utl_integral.h"
#  include "utl/concepts/utl_object_type.h"

UTL_NAMESPACE_BEGIN

namespace details {
namespace incrementable {
template <typename T>
concept with_difference_type = requires { typename T::difference_type; };
template <typename T>
concept subtractable = !with_difference_type<T> && requires(T const& a, T const& b) {
    { a - b } -> integral;
};
} // namespace incrementable
} // namespace details

template <typename>
struct __UTL_PUBLIC_TEMPLATE incrementable_traits {};

template <object_type T>
struct __UTL_PUBLIC_TEMPLATE incrementable_traits<T*> {
    using difference_type = __UTL ptrdiff_t;
};

template <details::incrementable::with_difference_type T>
struct __UTL_PUBLIC_TEMPLATE incrementable_traits<T> {
    using difference_type = typename T::difference_type;
};

template <details::incrementable::subtractable T>
struct __UTL_PUBLIC_TEMPLATE incrementable_traits<T> :
    details::incrementable::subtract_result_t<T> {};

UTL_NAMESPACE_END

#else // if UTL_CXX20

#  include "utl/type_traits/utl_enable_if.h"
#  include "utl/type_traits/utl_has_member_difference_type.h"
#  include "utl/type_traits/utl_is_object.h"

UTL_NAMESPACE_BEGIN

namespace details {
namespace incrementable {
template <typename T>
struct difference_identity {
    using difference_type UTL_NODEBUG = T;
};
template <typename T UTL_REQUIRES_CXX11(UTL_TRAIT_is_object(T))>
using object_pointer UTL_NODEBUG = difference_identity<ptrdiff_t>;
template <typename T UTL_REQUIRES_CXX11(UTL_TRAIT_has_member_difference_type(T))>
__UTL_HIDE_FROM_ABI auto resolve_trait(int) noexcept
    -> difference_identity<typename T::difference_type>;
template <typename T UTL_REQUIRES_CXX11(!UTL_TRAIT_has_member_difference_type(T))>
__UTL_HIDE_FROM_ABI auto resolve_trait(int) noexcept -> difference_identity<subtract_result_t<T>>;

struct fallback_t {};
template <typename T>
__UTL_HIDE_FROM_ABI auto resolve_trait(float) noexcept -> fallback_t;
template <typename T>
using traits UTL_NODEBUG = decltype(resolve_trait<T>(0));

} // namespace incrementable
} // namespace details

template <typename T>
struct __UTL_PUBLIC_TEMPLATE incrementable_traits : details::incrementable::traits<T> {};

template <typename T>
struct __UTL_PUBLIC_TEMPLATE incrementable_traits<T*> :
    details::incrementable::object_pointer<T> {};

UTL_NAMESPACE_END

#endif // if UTL_CXX20
