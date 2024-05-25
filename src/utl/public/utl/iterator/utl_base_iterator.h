// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/concepts/utl_referenceable.h"
#include "utl/concepts/utl_same_as.h"
#include "utl/preprocessor/utl_config.h"
#include "utl/type_traits/utl_is_copy_assignable.h"
#include "utl/type_traits/utl_is_copy_constructible.h"
#include "utl/type_traits/utl_is_destructible.h"
#include "utl/type_traits/utl_is_referenceable.h"
#include "utl/type_traits/utl_is_same.h"
#include "utl/type_traits/utl_logical_traits.h"
#include "utl/utility/utl_swap.h"

UTL_NAMESPACE_BEGIN

namespace details {
namespace iterator {

template <typename T UTL_REQUIRES_CXX11(UTL_TRAIT_is_same(
    decltype(++declval<T&>()), T &) &&UTL_TRAIT_is_referenceable(decltype(*declval<T&>()++)))>
UTL_REQUIRES_CXX20(requires(T t) {
    { ++t } -> same_as<T&>;
    { *t++ } -> referenceable;
})
true_type evaluate_incrementable(int) noexcept;

template <typename T>
false_type evaluate_incrementable(float) noexcept;

template <typename T>
using incrementable = decltype(evaluate_incrementable<T>(0));

} // namespace iterator
} // namespace details

template <typename T>
struct is_iterator :
    conjunction<is_copy_constructible<T>, is_copy_assignable<T>, is_destructible<T>,
        is_swappable<T>, is_dereferenceable<T>, has_iter_pointer<T>, has_iter_reference<T>,
        has_iter_difference<T>, details::iterator::incrementable<T>> {};

UTL_NAMESPACE_END
