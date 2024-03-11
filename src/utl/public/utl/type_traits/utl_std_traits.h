/* Copyright 2023-2024 Bryan Wong */

#pragma once

#include "utl/preprocessor/utl_namespace.h"
#include "utl/preprocessor/utl_standard.h"
#include "utl/type_traits/utl_constants.h"
#include "utl/type_traits/utl_decay.h"
#include "utl/type_traits/utl_declval.h"
#include "utl/type_traits/utl_enable_if.h"
#include "utl/type_traits/utl_extent.h"
#include "utl/type_traits/utl_is_arithmetic.h"
#include "utl/type_traits/utl_is_base_of.h"
#include "utl/type_traits/utl_is_class.h"
#include "utl/type_traits/utl_is_complete.h"
#include "utl/type_traits/utl_is_compound.h"
#include "utl/type_traits/utl_is_enum.h"
#include "utl/type_traits/utl_is_floating_point.h"
#include "utl/type_traits/utl_is_function.h"
#include "utl/type_traits/utl_is_fundamental.h"
#include "utl/type_traits/utl_is_integral.h"
#include "utl/type_traits/utl_is_member_x_pointer.h"
#include "utl/type_traits/utl_is_null_pointer.h"
#include "utl/type_traits/utl_is_object.h"
#include "utl/type_traits/utl_is_pointer.h"
#include "utl/type_traits/utl_is_same.h"
#include "utl/type_traits/utl_is_scalar.h"
#include "utl/type_traits/utl_is_union.h"
#include "utl/type_traits/utl_is_void.h"
#include "utl/type_traits/utl_is_x_cv.h"
#include "utl/type_traits/utl_is_x_reference.h"
#include "utl/type_traits/utl_is_x_signed.h"
#include "utl/type_traits/utl_logical_traits.h"
#include "utl/type_traits/utl_modify_pointer.h"
#include "utl/type_traits/utl_modify_x_cv.h"
#include "utl/type_traits/utl_modify_x_reference.h"
#include "utl/type_traits/utl_rank.h"
#include "utl/type_traits/utl_remove_cvref.h"
#include "utl/type_traits/utl_remove_x_extent.h"
#include "utl/type_traits/utl_type_identity.h"
#include "utl/type_traits/utl_void_t.h"

#include <type_traits>

UTL_NAMESPACE_BEGIN

using ::std::is_abstract;
using ::std::is_empty;
using ::std::is_polymorphic;
using ::std::is_standard_layout;
using ::std::is_trivial;
using ::std::is_trivially_copyable;

using ::std::is_constructible;
using ::std::is_nothrow_constructible;
using ::std::is_trivially_constructible;

using ::std::is_default_constructible;
using ::std::is_nothrow_default_constructible;
using ::std::is_trivially_default_constructible;

using ::std::is_copy_constructible;
using ::std::is_nothrow_copy_constructible;
using ::std::is_trivially_copy_constructible;

using ::std::is_move_constructible;
using ::std::is_nothrow_move_constructible;
using ::std::is_trivially_move_constructible;

using ::std::is_assignable;
using ::std::is_nothrow_assignable;
using ::std::is_trivially_assignable;

using ::std::is_copy_assignable;
using ::std::is_nothrow_copy_assignable;
using ::std::is_trivially_copy_assignable;

using ::std::is_move_assignable;
using ::std::is_nothrow_move_assignable;
using ::std::is_trivially_move_assignable;

using ::std::is_convertible;
using ::std::is_destructible;
using ::std::is_nothrow_destructible;
using ::std::is_trivially_destructible;

using ::std::has_virtual_destructor;

using ::std::alignment_of;

template <typename... Ts>
struct common_type : ::std::common_type<Ts...> {};

using ::std::underlying_type;

template <typename... T>
using common_type_t = typename common_type<T...>::type;
template <typename T>
using underlying_type_t = typename underlying_type<T>::type;

namespace details {
namespace convertible {
template <typename From, typename To>
auto nothrow_test(int) -> enable_if_t<is_convertible<From, To>::value,
    bool_constant<noexcept(static_cast<To>(declval<From>()))>>;
template <typename, typename>
false_type nothrow_test(float);
template <typename From, typename To>
using is_nothrow = decltype(nothrow_test<From, To>(0));

} /* namespace convertible */
} /* namespace details */

template <typename From, typename To>
struct is_nothrow_convertible : details::convertible::is_nothrow<From, To> {};
template <typename To>
struct is_nothrow_convertible<To, To> : true_type {};

#ifdef UTL_BUILTIN_is_layout_compatible
template <typename T, typename U>
struct is_layout_compatible : bool_constant<UTL_BUILTIN_is_layout_compatible(T, U)> {};

#  ifdef UTL_CXX14
template <typename T, typename U>
UTL_INLINE_CXX17 constexpr bool is_layout_compatible_v = UTL_BUILTIN_is_layout_compatible(T, U);
#  endif

#  define UTL_TRAIT_SUPPORTED_is_layout_compatible 1
#else /* ifdef UTL_BUILTIN_is_layout_compatible */
template <typename, typename>
struct is_layout_compatible : false_type {};
template <typename T>
struct is_layout_compatible<T, T> : true_type {};

#  ifdef UTL_CXX14
template <typename T, typename U>
UTL_INLINE_CXX17 constexpr bool is_layout_compatible_v = is_layout_compatible<T, U>::value;
#  endif

#  define UTL_TRAIT_SUPPORTED_is_layout_compatible 0
#  define UTL_TRAIT_DEFINED_is_layout_compatible 1
#endif /* ifdef UTL_BUILTIN_is_layout_compatible */

#ifdef UTL_BUILTIN_is_pointer_interconvertible_base_of

template <typename T, typename U>
struct is_pointer_interconvertible_base_of :
    bool_constant<UTL_BUILTIN_is_pointer_interconvertible_base_of(T, U)> {};

#  ifdef UTL_CXX14
template <typename T, typename U>
UTL_INLINE_CXX17 constexpr bool is_pointer_interconvertible_base_of_v =
    UTL_BUILTIN_is_pointer_interconvertible_base_of(T, U);
#  endif

#  define UTL_TRAIT_SUPPORTED_is_pointer_interconvertible_base_of 1
#else /* ifdef UTL_BUILTIN_is_pointer_interconvertible_base_of */
template <typename, typename>
struct is_pointer_interconvertible_base_of : false_type {};
template <typename T>
struct is_pointer_interconvertible_base_of<T, T> : true_type {};

#  ifdef UTL_CXX14
template <typename T, typename U>
UTL_INLINE_CXX17 constexpr bool is_pointer_interconvertible_base_of_v =
    is_pointer_interconvertible_base_of<T, U>::value;
#  endif

#  define UTL_TRAIT_SUPPORTED_is_pointer_interconvertible_base_of 0
#  define UTL_TRAIT_DEFINED_is_pointer_interconvertible_base_of 1
#endif /* ifdef UTL_BUILTIN_is_pointer_interconvertible_base_of */

#ifdef UTL_BUILTIN_reference_constructs_from_temporary
template <typename T, typename U>
struct reference_constructs_from_temporary :
    bool_constant<UTL_BUILTIN_reference_constructs_from_temporary(T, U)> {};
#  define UTL_TRAIT_SUPPORTED_reference_constructs_from_temporary 1

#  ifdef UTL_CXX14
template <typename T, typename U>
UTL_INLINE_CXX17 constexpr bool reference_constructs_from_temporary_v =
    UTL_BUILTIN_reference_constructs_from_temporary(T, U);
#  endif

#else /* ifdef UTL_BUILTIN_reference_constructs_from_temporary */
template <typename, typename>
struct reference_constructs_from_temporary : false_type {};

#  ifdef UTL_CXX14
template <typename T, typename U>
UTL_INLINE_CXX17 constexpr bool reference_constructs_from_temporary_v =
    reference_constructs_from_temporary<T, U>::value;
#  endif

#  define UTL_TRAIT_SUPPORTED_reference_constructs_from_temporary 0
#  define UTL_TRAIT_DEFINED_reference_constructs_from_temporary 1
#endif /* ifdef UTL_BUILTIN_reference_constructs_from_temporary */

UTL_NAMESPACE_END

#include "utl/type_traits/utl_traits_support.h"
