// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_config.h"

#include "utl/concepts/utl_lvalue_reference.h"
#include "utl/concepts/utl_rvalue_reference.h"
#include "utl/type_traits/utl_enable_if.h"
#include "utl/type_traits/utl_is_lvalue_reference.h"
#include "utl/type_traits/utl_is_nothrow_dereferenceable.h"
#include "utl/type_traits/utl_is_rvalue_reference.h"
#include "utl/type_traits/utl_is_void.h"
#include "utl/type_traits/utl_void_t.h"
#include "utl/utility/utl_forward.h"
#include "utl/utility/utl_move.h"

UTL_NAMESPACE_BEGIN

namespace details {
namespace iterator_move {
template <typename T UTL_REQUIRES_CXX11(
    UTL_TRAIT_is_lvalue_reference(decltype(*UTL_SCOPE declval<T>())) &&
    UTL_TRAIT_is_rvalue_reference(decltype(UTL_SCOPE move(*UTL_SCOPE declval<T>()))))>
UTL_REQUIRES_CXX20(requires(T&& it) {
    { *it } -> lvalue_reference;
    UTL_SCOPE move(*it);
})
constexpr auto iter_move(T&& it) noexcept(UTL_TRAIT_is_nothrow_dereferenceable(T))
    -> decltype(UTL_SCOPE move(*it)) {
    return UTL_SCOPE move(*it);
}

template <typename T UTL_REQUIRES_CXX11(
    UTL_TRAIT_is_rvalue_reference(decltype(*UTL_SCOPE declval<T>())))>
UTL_REQUIRES_CXX20(requires(T&& it) {
    { *it } -> rvalue_reference;
})
constexpr auto iter_move(T&& it) noexcept(UTL_TRAIT_is_nothrow_dereferenceable(T))
    -> decltype(*it) {
    return *it;
}

struct function_t {
    template <typename T UTL_REQUIRES_CXX11(
        UTL_SCOPE always_true_type<decltype(iter_move(UTL_SCOPE declval<T>()))>::value)>
    UTL_REQUIRES_CXX20(requires(T&& it) { iter_move(UTL_SCOPE forward<T>(it)); })
    constexpr auto operator()(T&& it) const noexcept(UTL_TRAIT_is_nothrow_dereferenceable(T))
        -> decltype(iter_move(UTL_SCOPE declval<T>())) {
        return iter_move(UTL_SCOPE forward<T>(it));
    }
};
} // namespace iterator_move
} // namespace details

namespace ranges {
inline namespace cpo {
UTL_DEFINE_CUSTOMIZATION_POINT(UTL_SCOPE details::iterator_move::function_t, iter_move);
} // namespace cpo
} // namespace ranges

UTL_NAMESPACE_END
