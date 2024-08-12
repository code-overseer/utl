// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/configuration/utl_namespace.h"
#include "utl/type_traits/utl_template_list.h"

UTL_NAMESPACE_BEGIN

/**
 * A union operation of the const-ness of the parameter pack
 * The resulting type is const iff there is a const-qualified type in the type list
 *
 * @tparam Target - type to apply the resulting qualifer
 * @tparam TypeList - type list to merge/unionize
 *
 * @result typename type - const-qualified Target iff there is a const-qualified type in the type
 * list
 */
template <typename Target, typename TypeList>
struct merge_const;
template <typename Target, template <typename...> class TypeList>
struct __UTL_PUBLIC_TEMPLATE merge_const<Target, TypeList<>> {
    using type UTL_NODEBUG = Target;
};
template <typename Target, template <typename...> class TypeList, typename U0, typename... Us>
struct __UTL_PUBLIC_TEMPLATE merge_const<Target, TypeList<U0 const, Us...>> {
    using type UTL_NODEBUG = Target const;
};
template <typename Target, template <typename...> class TypeList, typename U0, typename... Us>
struct __UTL_PUBLIC_TEMPLATE merge_const<Target, TypeList<U0, Us...>> :
    merge_const<Target, type_list<Us...>> {};
template <typename Target, typename TypeList>
using merge_const_t = typename merge_const<Target, TypeList>::type;

UTL_NAMESPACE_END
