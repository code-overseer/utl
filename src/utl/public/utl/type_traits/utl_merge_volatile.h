// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_namespace.h"

#include "utl/type_traits/utl_template_list.h"

UTL_NAMESPACE_BEGIN

/**
 * A union operation of the volatile-ness of the parameter pack
 * The resulting type is volatile iff there is a volatile-qualified type in the type list
 *
 * @tparam Target - type to apply the resulting qualifer
 * @tparam TypeList - type list to merge/unionize
 *
 * @result typename type - volatile-qualified Target iff there is a volatile-qualified type in the
 * type list
 */
template <typename Target, typename TypeList>
struct merge_volatile;
template <typename Target, template <typename...> class TypeList>
struct __UTL_PUBLIC_TEMPLATE merge_volatile<Target, TypeList<>> {
    using type UTL_NODEBUG = Target;
};
template <typename Target, template <typename...> class TypeList, typename U0, typename... Us>
struct __UTL_PUBLIC_TEMPLATE merge_volatile<Target, TypeList<U0 volatile, Us...>> {
    using type UTL_NODEBUG = Target volatile;
};
template <typename Target, template <typename...> class TypeList, typename U0, typename... Us>
struct __UTL_PUBLIC_TEMPLATE merge_volatile<Target, TypeList<U0, Us...>> :
    merge_volatile<Target, type_list<Us...>> {};
template <typename Target, typename TypeList>
using merge_volatile_t = typename merge_volatile<Target, TypeList>::type;

UTL_NAMESPACE_END
