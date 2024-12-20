// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

#include "utl/concepts/utl_lvalue_reference.h"
#include "utl/concepts/utl_rvalue_reference.h"
#include "utl/type_traits/utl_enable_if.h"
#include "utl/type_traits/utl_is_dereferenceable.h"
#include "utl/type_traits/utl_is_lvalue_reference.h"
#include "utl/type_traits/utl_is_rvalue_reference.h"
#include "utl/type_traits/utl_is_void.h"
#include "utl/type_traits/utl_void_t.h"
#include "utl/utility/utl_forward.h"
#include "utl/utility/utl_move.h"

UTL_NAMESPACE_BEGIN

namespace details {
namespace iterator_move {
template <typename T UTL_CONSTRAINT_CXX11(
    UTL_TRAIT_is_lvalue_reference(decltype(*__UTL declval<T&>())) &&
    UTL_TRAIT_is_rvalue_reference(decltype(__UTL move(*__UTL declval<T&>()))))>
UTL_CONSTRAINT_CXX20(requires(T&& it) {
    { *it } -> lvalue_reference;
    __UTL move(*it);
})
__UTL_HIDE_FROM_ABI constexpr auto iter_move(T&& it) noexcept(
    UTL_TRAIT_is_nothrow_dereferenceable(T)) -> decltype(__UTL move(*it)) {
    return __UTL move(*it);
}

template <typename T UTL_CONSTRAINT_CXX11(
    UTL_TRAIT_is_rvalue_reference(decltype(*__UTL declval<T&>())))>
UTL_CONSTRAINT_CXX20(requires(T&& it) {
    { *it } -> rvalue_reference;
})
__UTL_HIDE_FROM_ABI constexpr auto iter_move(T&& it) noexcept(
    UTL_TRAIT_is_nothrow_dereferenceable(T)) -> decltype(*it) {
    return *it;
}

struct function_t {
    template <typename T UTL_CONSTRAINT_CXX11(
        __UTL always_true_type<decltype(iter_move(__UTL declval<T>()))>::value)>
    UTL_CONSTRAINT_CXX20(requires(T&& it) { iter_move(__UTL forward<T>(it)); })
    __UTL_HIDE_FROM_ABI constexpr auto operator()(T&& it) const
        noexcept(UTL_TRAIT_is_nothrow_dereferenceable(T))
            -> decltype(iter_move(__UTL declval<T>())) {
        return iter_move(__UTL forward<T>(it));
    }
};
} // namespace iterator_move
} // namespace details

namespace ranges {
inline namespace cpo {
UTL_DEFINE_CUSTOMIZATION_POINT(__UTL details::iterator_move::function_t, iter_move);
} // namespace cpo
} // namespace ranges

UTL_NAMESPACE_END
