// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/base_preprocessor.h"
#include "utl/type_traits/utl_is_base_of.h"
#include "utl_reference_counting_fwd.h"

UTL_NAMESPACE_BEGIN

/**
 * Type trait to determine if a type is reference countable.
 *
 * This type trait determines whether a given type `T` supports reference counting. It checks
 * if `T` is either derived from `atomic_reference_count`, derived from `reference_count`, or
 * if `T` itself is `atomic_reference_count` or `reference_count`.
 *
 * @tparam T The type to be checked for reference countability.
 *
 * @note This type trait provides a compile-time boolean value indicating whether `T` supports
 *       reference counting.
 */
template <typename T>
struct is_reference_countable :
    disjunction<is_base_of<atomic_reference_count<T>, T>, is_base_of<reference_count<T>, T>> {};

/**
 * Specialization of `is_reference_countable` for `atomic_reference_count`.
 *
 * This specialization ensures that `atomic_reference_count` itself is considered reference
 * countable.
 */
template <typename T>
struct is_reference_countable<atomic_reference_count<T>> : true_type {};

/**
 * Specialization of `is_reference_countable` for `reference_count`.
 *
 * This specialization ensures that `reference_count` itself is considered reference countable.
 */
template <typename T>
struct is_reference_countable<reference_count<T>> : true_type {};

/**
 * Specialization of `is_reference_countable` for `const`-qualified types.
 *
 * This specialization ensures that `const`-qualified versions of a reference countable type `T`
 * are also considered reference countable.
 */
template <typename T>
struct is_reference_countable<T const> : is_reference_countable<T> {};

#ifdef UTL_CXX20

/**
 * Concept to check if a type is reference countable.
 *
 * This C++20 concept checks if a given type `T` is reference countable, i.e., if it supports
 * reference counting.
 *
 * @tparam T The type to be checked for reference countability.
 */
template <typename T>
concept reference_countable = is_reference_countable<T>::value;
#endif

UTL_NAMESPACE_END
