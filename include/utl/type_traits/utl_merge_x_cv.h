// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_namespace.h"
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
struct merge_const<Target, TypeList<>> {
    using type = Target;
};
template <typename Target, template <typename...> class TypeList, typename U0, typename... Us>
struct merge_const<Target, TypeList<U0 const, Us...>> {
    using type = Target const;
};
template <typename Target, template <typename...> class TypeList, typename U0, typename... Us>
struct merge_const<Target, TypeList<U0, Us...>> : merge_const<Target, type_list<Us...>> {};
template <typename Target, typename TypeList>
using merge_const_t = typename merge_const<Target, TypeList>::type;

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
struct merge_volatile<Target, TypeList<>> {
    using type = Target;
};
template <typename Target, template <typename...> class TypeList, typename U0, typename... Us>
struct merge_volatile<Target, TypeList<U0 volatile, Us...>> {
    using type = Target volatile;
};
template <typename Target, template <typename...> class TypeList, typename U0, typename... Us>
struct merge_volatile<Target, TypeList<U0, Us...>> : merge_volatile<Target, type_list<Us...>> {};
template <typename Target, typename TypeList>
using merge_volatile_t = typename merge_volatile<Target, TypeList>::type;

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
struct merge_cv<Target, TypeList<>> {
    using type = Target;
};
template <typename Target, template <typename...> class TypeList, typename U0, typename... Us>
struct merge_cv<Target, TypeList<U0 const volatile, Us...>> {
    using type = Target const volatile;
};
template <typename Target, template <typename...> class TypeList, typename U0, typename... Us>
struct merge_cv<Target, TypeList<U0 const, Us...>> :
    merge_volatile<Target const, type_list<Us...>> {};
template <typename Target, template <typename...> class TypeList, typename U0, typename... Us>
struct merge_cv<Target, TypeList<U0 volatile, Us...>> :
    merge_const<Target volatile, type_list<Us...>> {};
template <typename Target, template <typename...> class TypeList, typename U0, typename... Us>
struct merge_cv<Target, TypeList<U0, Us...>> : merge_cv<Target, type_list<Us...>> {};
template <typename Target, typename TypeList>
using merge_cv_t = typename merge_cv<Target, TypeList>::type;

UTL_NAMESPACE_END
