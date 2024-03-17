/* Copyright 2023-2024 Bryan Wong */

#pragma once

#include "utl/preprocessor/utl_namespace.h"
#include "utl/preprocessor/utl_standard.h"
#include "utl/type_traits/utl_add_const.h"
#include "utl/type_traits/utl_add_cv.h"
#include "utl/type_traits/utl_add_lvalue_reference.h"
#include "utl/type_traits/utl_add_rvalue_reference.h"
#include "utl/type_traits/utl_add_volatile.h"
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
#include "utl/type_traits/utl_is_const.h"
#include "utl/type_traits/utl_is_enum.h"
#include "utl/type_traits/utl_is_floating_point.h"
#include "utl/type_traits/utl_is_function.h"
#include "utl/type_traits/utl_is_fundamental.h"
#include "utl/type_traits/utl_is_integral.h"
#include "utl/type_traits/utl_is_lvalue_reference.h"
#include "utl/type_traits/utl_is_member_function_pointer.h"
#include "utl/type_traits/utl_is_member_object_pointer.h"
#include "utl/type_traits/utl_is_member_pointer.h"
#include "utl/type_traits/utl_is_null_pointer.h"
#include "utl/type_traits/utl_is_object.h"
#include "utl/type_traits/utl_is_pointer.h"
#include "utl/type_traits/utl_is_reference.h"
#include "utl/type_traits/utl_is_rvalue_reference.h"
#include "utl/type_traits/utl_is_same.h"
#include "utl/type_traits/utl_is_scalar.h"
#include "utl/type_traits/utl_is_signed.h"
#include "utl/type_traits/utl_is_union.h"
#include "utl/type_traits/utl_is_unsigned.h"
#include "utl/type_traits/utl_is_void.h"
#include "utl/type_traits/utl_is_volatile.h"
#include "utl/type_traits/utl_is_x_constructible.h"
#include "utl/type_traits/utl_is_x_convertible.h"
#include "utl/type_traits/utl_logical_traits.h"
#include "utl/type_traits/utl_modify_pointer.h"
#include "utl/type_traits/utl_rank.h"
#include "utl/type_traits/utl_remove_all_extents.h"
#include "utl/type_traits/utl_remove_const.h"
#include "utl/type_traits/utl_remove_cv.h"
#include "utl/type_traits/utl_remove_cvref.h"
#include "utl/type_traits/utl_remove_extent.h"
#include "utl/type_traits/utl_remove_reference.h"
#include "utl/type_traits/utl_remove_volatile.h"
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

using ::std::is_destructible;
using ::std::is_nothrow_destructible;
using ::std::is_trivially_destructible;

using ::std::has_virtual_destructor;

template <typename... Ts>
struct common_type : ::std::common_type<Ts...> {};
template <typename... T>
using common_type_t = typename common_type<T...>::type;

using ::std::underlying_type;
template <typename T>
using underlying_type_t = typename underlying_type<T>::type;

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
