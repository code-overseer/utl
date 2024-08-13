// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

#include "utl/iterator/utl_iter_reference_t.h"
#include "utl/type_traits/utl_constants.h"
#include "utl/utility/utl_forward.h"

#if UTL_CXX20

UTL_NAMESPACE_BEGIN

template <typename Out, typename T>
concept indirectly_writable = requires(Out&& o, T&& t) {
    *o = __UTL forward<T>(t);
    *__UTL forward<Out>(o) = __UTL forward<T>(t);
    const_cast<const __UTL iter_reference_t<Out>&&>(*o) = __UTL forward<T>(t);
    const_cast<const __UTL iter_reference_t<Out>&&>(*__UTL forward<Out>(o)) = __UTL forward<T>(t);
};

template <typename OutIt, typename ValueType>
struct __UTL_PUBLIC_TEMPLATE is_indirectly_writable :
    bool_constant<indirectly_writable<OutIt, ValueType>> {};

template <typename OutIt, typename ValueType>
inline constexpr bool is_indirectly_writable_v = indirectly_writable<OutIt, ValueType>;

UTL_NAMESPACE_END

#else

#  include "utl/type_traits/utl_declval.h"
#  include "utl/type_traits/utl_is_assignable.h"
#  include "utl/type_traits/utl_logical_traits.h"

UTL_NAMESPACE_BEGIN

namespace details {
namespace indirectly_writable {
template <typename Out, typename T>
__UTL_HIDE_FROM_ABI auto check(float) -> __UTL false_type;

template <typename Out, typename T>
__UTL_HIDE_FROM_ABI auto check(int)
    -> __UTL conjunction<__UTL is_assignable<decltype(*__UTL declval<Out&>()), T>,
        __UTL is_assignable<decltype(*__UTL declval<Out>()), T>,
        decltype((const_cast<const __UTL iter_reference_t<Out>&&>(*__UTL declval<Out&>()),
            __UTL true_type())),
        decltype((const_cast<const __UTL iter_reference_t<Out>&&>(*__UTL declval<Out>()),
            __UTL true_type())),
        __UTL is_assignable<const __UTL iter_reference_t<Out>&&, T>>;

template <typename Out, typename T>
using implemented UTL_NODEBUG = decltype(__UTL details::indirectly_writable::check<Out, T>(0));

} // namespace indirectly_writable
} // namespace details

template <typename OutIt, typename ValueType>
struct __UTL_PUBLIC_TEMPLATE is_indirectly_writable :
    __UTL details::indirectly_writable::implemented<OutIt, ValueType> {};

#  if UTL_CXX14
template <typename OutIt, typename ValueType>
UTL_INLINE_CXX17 constexpr bool is_indirectly_writable_v =
    __UTL is_indirectly_writable<OutIt, ValueType>::value;
#  endif

UTL_NAMESPACE_END

#endif
