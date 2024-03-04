// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/type_traits/utl_common.h"
#include "utl/type_traits/utl_is_base_of.h"
#include "utl/type_traits/utl_is_complete.h"
#include "utl/type_traits/utl_logical_traits.h"
#include "utl/type_traits/utl_void_t.h"

UTL_NAMESPACE_BEGIN

namespace type_traits {
namespace details {
template <typename From, typename To, typename = void>
struct directly_castable : false_type {};
template <typename From, typename To>
struct directly_castable<From, To, void_t<decltype((To*)declval<From>())>> : true_type {};
} // namespace details
} // namespace type_traits

template <typename Base, typename Derive>
struct is_unambiguous_decendent_of :
    conjunction<is_complete<Derive>, is_base_of<Base, Derive>,
        type_trais::details::directly_castable<Base*, Derive*>> {};

template <typename Derive, typename Base>
struct is_unambiguous_ancestor_of :
    conjunction<is_complete<Derive>, is_base_of<Base, Derive>,
        type_trais::details::directly_castable<Derive*, Base*>> {};

#ifdef UTL_CXX14

template <typename T>
UTL_INLINE_CXX17 constexpr bool is_unambiguous_decendent_of_v =
    is_unambiguous_decendent_of<T>::value;
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_unambiguous_ancestor_of_v = is_unambiguous_ancestor_of<T>::value;

#endif // ifdef UTL_CXX14

UTL_NAMESPACE_END
