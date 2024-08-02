/* Copyright 2023-2024 Bryan Wong */

#pragma once

#include "utl/preprocessor/utl_compiler.h"
#include "utl/preprocessor/utl_concatenation.h"
#include "utl/preprocessor/utl_is_empty.h"

#if UTL_COMPILER_MSVC

#  ifdef __has_builtin
#    error '__has_builtin' definition collision
#  endif
#  define __has_builtin(BUILTIN) UTL_IS_EMPTY(UTL_CONCAT(UTL_MSVC_SUPPORTS, BUILTIN))

#  if UTL_COMPILER_MSVC_AT_LEAST(1915)
#    define UTL_MSVC_SUPPORTS__is_aggregate
#  endif

#  if UTL_CXX17
#    define UTL_MSVC_SUPPORTS__builtin_launder
#  endif

#  if UTL_COMPILER_MSVC_AT_LEAST(1925) /* UTL_HAS_BUILTIN(__builtin_is_constant_evaluated) */
#    define UTL_MSVC_SUPPORTS__builtin_is_constant_evaluated
#  endif

#  if UTL_COMPILER_MSVC_AT_LEAST(1927)
#    define UTL_MSVC_SUPPORTS__builtin_FILE
#    define UTL_MSVC_SUPPORTS__builtin_FUNCTION
#    define UTL_MSVC_SUPPORTS__builtin_LINE
#  endif

#  if UTL_COMPILER_MSVC_AT_LEAST(1929)
#    define UTL_MSVC_SUPPORTS__is_layout_compatible
#    define UTL_MSVC_SUPPORTS__is_pointer_interconvertible_base_of
#    define UTL_MSVC_SUPPORTS__is_corresponding_member
#  endif

#  if UTL_COMPILER_MSVC_AT_LEAST(1935)
#    define UTL_MSVC_SUPPORTS__builtin_FUNCSIG
#  endif

#  define UTL_MSVC_SUPPORTS__builtin_addressof

#  define UTL_MSVC_SUPPORTS__has_unique_object_representations
#  define UTL_MSVC_SUPPORTS__is_constructible
#  define UTL_MSVC_SUPPORTS__is_assignable
#  define UTL_MSVC_SUPPORTS__is_union
#  define UTL_MSVC_SUPPORTS__is_class
#  define UTL_MSVC_SUPPORTS__is_convertible_to
#  define UTL_MSVC_SUPPORTS__is_convertible
#  define UTL_MSVC_SUPPORTS__is_enum
#  define UTL_MSVC_SUPPORTS__is_member_function_pointer
#  define UTL_MSVC_SUPPORTS__is_member_object_pointer
#  define UTL_MSVC_SUPPORTS__is_empty
#  define UTL_MSVC_SUPPORTS__is_final
#  define UTL_MSVC_SUPPORTS__is_polymorphic
#  define UTL_MSVC_SUPPORTS__is_abstract
#  define UTL_MSVC_SUPPORTS__is_member_pointer
#  define UTL_MSVC_SUPPORTS__is_standard_layout
#  define UTL_MSVC_SUPPORTS__is_trivial
#  define UTL_MSVC_SUPPORTS__is_trivially_copyable
#  define UTL_MSVC_SUPPORTS__has_virtual_destructor
#  define UTL_MSVC_SUPPORTS__is_destructible
#  define UTL_MSVC_SUPPORTS__is_trivially_constructible
#  define UTL_MSVC_SUPPORTS__is_trivially_assignable
#  define UTL_MSVC_SUPPORTS__is_trivially_destructible
#  define UTL_MSVC_SUPPORTS__is_nothrow_constructible
#  define UTL_MSVC_SUPPORTS__is_base_of
#  define UTL_MSVC_SUPPORTS__underlying_type

#  ifdef __is_convertible
#    error '__is_convertible' definition collision
#  endif
#  define __is_convertible __is_convertible_to

#endif
