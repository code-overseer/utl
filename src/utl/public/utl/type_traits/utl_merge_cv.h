// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

#include "utl/type_traits/utl_merge_const.h"
#include "utl/type_traits/utl_merge_volatile.h"
#include "utl/type_traits/utl_template_list.h"

UTL_NAMESPACE_BEGIN

/**
 * A union operation of the cv-ness of the parameter pack
 * The resulting type is cv iff there is a cv-qualified type in the type list
 *
 * @tparam Target - type to apply the resulting qualifer
 * @tparam TypeList - type list to merge/unionize
 *
 * @result typename type - cv-qualified Target depending on the const/volatile-ness of the elements
 * in the typelist
 */
template <typename Target, typename TypeList>
struct merge_cv;
template <typename Target, template <typename...> class TypeList>
struct __UTL_PUBLIC_TEMPLATE merge_cv<Target, TypeList<>> {
    using type UTL_NODEBUG = Target;
};
template <typename Target, template <typename...> class TypeList, typename U0, typename... Us>
struct __UTL_PUBLIC_TEMPLATE merge_cv<Target, TypeList<U0 const volatile, Us...>> {
    using type UTL_NODEBUG = Target const volatile;
};
template <typename Target, template <typename...> class TypeList, typename U0, typename... Us>
struct __UTL_PUBLIC_TEMPLATE merge_cv<Target, TypeList<U0 const, Us...>> :
    merge_volatile<Target const, type_list<Us...>> {};
template <typename Target, template <typename...> class TypeList, typename U0, typename... Us>
struct __UTL_PUBLIC_TEMPLATE merge_cv<Target, TypeList<U0 volatile, Us...>> :
    merge_const<Target volatile, type_list<Us...>> {};
template <typename Target, template <typename...> class TypeList, typename U0, typename... Us>
struct __UTL_PUBLIC_TEMPLATE merge_cv<Target, TypeList<U0, Us...>> :
    merge_cv<Target, type_list<Us...>> {};
template <typename Target, typename TypeList>
using merge_cv_t = typename merge_cv<Target, TypeList>::type;

UTL_NAMESPACE_END
