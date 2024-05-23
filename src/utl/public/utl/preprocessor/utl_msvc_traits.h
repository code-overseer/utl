/* Copyright 2023-2024 Bryan Wong */

#pragma once

#include "utl/preprocessor/utl_compiler.h"

#if UTL_COMPILER_MSVC

#  if UTL_COMPILER_MSVC_AT_LEAST(1915)
#    define UTL_MSVC_SUPPORTS__is_aggregate 1
#  endif

#  if UTL_CXX17
#    define UTL_MSVC_SUPPORTS__builtin_launder 1
#  endif

#  if UTL_COMPILER_MSVC_AT_LEAST(1925) /* UTL_HAS_BUILTIN(__builtin_is_constant_evaluated) */
#    define UTL_MSVC_SUPPORTS__builtin_is_constant_evaluated 1
#  endif

#  if UTL_COMPILER_MSVC_AT_LEAST(1929)
#    define UTL_MSVC_SUPPORTS__is_layout_compatible 1
#    define UTL_MSVC_SUPPORTS__is_pointer_interconvertible_base_of
#    define UTL_MSVC_SUPPORTS__is_corresponding_member
#  endif

#  define UTL_MSVC_SUPPORTS__builtin_addressof 1

#  define UTL_MSVC_SUPPORTS__has_unique_object_representations 1
#  define UTL_MSVC_SUPPORTS__is_constructible 1
#  define UTL_MSVC_SUPPORTS__is_assignable 1
#  define UTL_MSVC_SUPPORTS__is_union 1
#  define UTL_MSVC_SUPPORTS__is_class 1
#  define UTL_MSVC_SUPPORTS__is_convertible_to 1
#  define UTL_MSVC_SUPPORTS__is_convertible 1
#  define UTL_MSVC_SUPPORTS__is_enum 1
#  define UTL_MSVC_SUPPORTS__is_member_function_pointer 1
#  define UTL_MSVC_SUPPORTS__is_member_object_pointer 1
#  define UTL_MSVC_SUPPORTS__is_empty 1
#  define UTL_MSVC_SUPPORTS__is_final 1
#  define UTL_MSVC_SUPPORTS__is_polymorphic 1
#  define UTL_MSVC_SUPPORTS__is_abstract 1
#  define UTL_MSVC_SUPPORTS__is_member_pointer 1
#  define UTL_MSVC_SUPPORTS__is_standard_layout 1
#  define UTL_MSVC_SUPPORTS__is_trivial 1
#  define UTL_MSVC_SUPPORTS__is_trivially_copyable 1
#  define UTL_MSVC_SUPPORTS__has_virtual_destructor 1
#  define UTL_MSVC_SUPPORTS__is_destructible 1
#  define UTL_MSVC_SUPPORTS__is_trivially_constructible 1
#  define UTL_MSVC_SUPPORTS__is_trivially_assignable 1
#  define UTL_MSVC_SUPPORTS__is_trivially_destructible 1
#  define UTL_MSVC_SUPPORTS__is_nothrow_constructible 1
#  define UTL_MSVC_SUPPORTS__is_base_of 1
#  define UTL_MSVC_SUPPORTS__underlying_type 1

#  define __is_convertible __is_convertible_to

#endif
