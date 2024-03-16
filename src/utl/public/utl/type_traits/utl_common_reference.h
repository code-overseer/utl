// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/type_traits/utl_common.h"
#include "utl/type_traits/utl_common_type.h"
#include "utl/type_traits/utl_constants.h"
#include "utl/type_traits/utl_copy_cvref.h"
#include "utl/type_traits/utl_declval.h"
#include "utl/type_traits/utl_has_type.h"
#include "utl/type_traits/utl_is_x_convertible.h"
#include "utl/type_traits/utl_merge_x_cv.h"
#include "utl/type_traits/utl_modify_x_reference.h"
#include "utl/type_traits/utl_remove_cvref.h"

#if defined(UTL_USE_STD_TYPE_TRAITS) && defined(UTL_CXX20)

#  include <type_traits>

UTL_NAMESPACE_BEGIN

using std::basic_common_reference;
using std::common_reference;

using std::basic_common_reference_t;
using std::common_reference_t;

UTL_NAMESPACE_END

#else // defined(UTL_USE_STD_TYPE_TRAITS) && defined(UTL_CXX20)

UTL_STD_NAMESPACE_BEGIN
/* UTL_UNDEFINED_BEHAVIOUR */
template <typename T, typename U, template <typename> class TQual, template <typename> class UQual>
struct basic_common_reference;
UTL_STD_NAMESPACE_END

UTL_NAMESPACE_BEGIN

namespace details {
namespace common_reference {

struct empty_t {};

template <typename T, typename U, template <typename> class TQual, template <typename> class UQual,
    size_t = sizeof(::std::basic_common_reference<T, U, TQual, UQual>)>
::std::basic_common_reference<T, U, TQual, UQual> basic_ref_test(int);

template <typename T, typename U, template <typename> class TQual, template <typename> class UQual>
empty_t basic_ref_test(float);

} // namespace common_reference
} // namespace details

template <typename T, typename U, template <typename> class TQual, template <typename> class UQual>
struct basic_common_reference :
    decltype(details::common_reference::basic_ref_test<T, U, TQual, UQual>(0)) {};

template <typename T, typename U, template <typename> class TQual, template <typename> class UQual>
using basic_common_reference_t = typename basic_common_reference<T, U, TQual, UQual>::type;

template <typename...>
struct common_reference;

template <typename... Ts>
using common_reference_t = typename common_reference<Ts...>::type;

template <>
struct common_reference<> {};
template <typename T>
struct common_reference<T> {
    using type = T;
};

namespace details {
namespace common_reference {

template <typename T>
struct copy_cvref_from {
    template <typename U>
    using apply = copy_cvref_t<T, U>;
};

template <typename T0, typename T1, typename = void>
struct ternary_result {};
template <typename T0, typename T1>
struct ternary_result<T0, T1,
    void_t<decltype(false ? declval<T0 (&)()>()() : declval<T1 (&)()>()())>> {
    using type = decltype(false ? declval<T0 (&)()>()() : declval<T1 (&)()>()());
};

template <typename T0, typename T1>
using ternary_result_t = typename ternary_result<T0, T1>::type;

template <typename T0, typename T1, typename = void>
struct simple_common_ref;

template <typename T0, typename T1>
using simple_common_ref_t = typename simple_common_ref<T0, T1>::type;

template <typename T0, typename T1>
struct simple_common_ref<T0&, T1&,
    void_t<ternary_result_t<merge_cv_t<T0, type_list<T0, T1>>&,
        merge_cv_t<T1, type_list<T0, T1>>&> // ternary_result_t
        >                                   // void_t
    > : ternary_result<merge_cv_t<T0, type_list<T0, T1>>&, merge_cv_t<T1, type_list<T0, T1>>&> {};

template <typename Target, typename... Ts>
using all_convertible_to = conjunction<is_convertible<Ts, Target>...>;
template <typename T>
using rvalue_ref = add_rvalue_reference<remove_reference_t<T>>;
template <typename T>
using rvalue_ref_t = typename rvalue_ref<T>::type;

template <typename T0, typename T1>
struct simple_common_ref<T0&&, T1&&,
    enable_if_t<all_convertible_to<rvalue_ref_t<simple_common_ref_t<T0&, T1&>>, T0&&,
        T1&&>::value> // enable_if_t
    > : rvalue_ref<simple_common_ref_t<T0&, T1&>> {};

template <typename T0, typename T1>
struct simple_common_ref<T0&, T1&&,
    enable_if_t<
        is_convertible<T1&&, simple_common_ref_t<T0&, add_const_t<T1>&>>::value> // enable_if_t
    > : simple_common_ref<T0&, add_const_t<T1>&> {};

template <typename T0, typename T1>
struct simple_common_ref<T0&&, T1&, void_t<simple_common_ref_t<T1&, T0&&>> // void_t
    > : simple_common_ref<T1&, T0&&> {};

template <typename...>
struct first_type;

template <>
struct first_type<> {};

template <typename T, typename... Ts>
struct first_type<T, Ts...> : conditional_t<has_type<T>::value, T, first_type<Ts...>> {};

template <typename T, typename U>
using impl = first_type<simple_common_ref<T, U>,
    basic_common_reference<remove_cvref_t<T>, remove_cvref_t<U>, copy_cvref_from<T>::template apply,
        copy_cvref_from<U>::template apply>, // basic_common_reference
    ternary_result<T, U>, common_type<T, U>>;

template <typename List, typename = void>
struct sfinae_gt_2 {};

template <typename T, typename U, typename... Vs>
struct sfinae_gt_2<type_list<T, U, Vs...>, void_t<UTL_SCOPE common_reference_t<T, U>>> :
    UTL_SCOPE common_reference<UTL_SCOPE common_reference_t<T, U>, Vs...> {};

template <typename T, typename U, typename... Vs>
using impl_gt_2 = sfinae_gt_2<type_list<T, U, Vs...>>;

} // namespace common_reference
} // namespace details

template <typename T, typename U>
struct common_reference<T, U> : details::common_reference::impl<T, U> {};

template <typename T, typename U, typename... Vs>
struct common_reference<T, U, Vs...> : details::common_reference::impl_gt_2<T, U, Vs...> {};

UTL_NAMESPACE_END

#endif // defined(UTL_USE_STD_TYPE_TRAITS) && defined(UTL_CXX20)
