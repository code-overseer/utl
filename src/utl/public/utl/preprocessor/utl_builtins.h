/* Copyright 2023-2024 Bryan Wong */

#pragma once

#include "utl/preprocessor/utl_compiler.h"
#include "utl/preprocessor/utl_standard.h"

#ifdef __is_identifier
#  define UTL_IS_RESERVED_IDENTIFIER(X) !__is_identifier(X)
#else
#  define UTL_IS_RESERVED_IDENTIFIER(...) 0
#endif

#ifdef __has_builtin
#  define UTL_HAS_BUILTIN(BUILTIN) __has_builtin(BUILTIN)
#else /* ifdef __has_builtin */
#  define UTL_HAS_BUILTIN(...) 0
#endif /* ifdef __has_builtin */

#define UTL_SHOULD_USE_BUILTIN(BUILTIN)                                          \
    (UTL_HAS_BUILTIN(__##BUILTIN) || UTL_IS_RESERVED_IDENTIFIER(__##BUILTIN)) && \
        !UTL_DISABLE_BUILTIN_##BUILTIN

#if UTL_HAS_BUILTIN(__builtin_launder)
#  define UTL_BUILTIN_launder(X) __builtin_launder(X)
#elif UTL_COMPILER_GCC_AT_LEAST(7, 1, 0)
#  define UTL_BUILTIN_launder(X) __builtin_launder(X)
#elif UTL_COMPILER_MSVC_AT_LEAST(1914) && defined(UTL_CXX17)
#  define UTL_BUILTIN_launder(X) __builtin_launder(X)
#elif UTL_COMPILER_ICC_AT_LEAST(1900)
#  define UTL_BUILTIN_launder(X) __builtin_launder(X)
#elif defined(UTL_COMPILER_ICX)
#  define UTL_BUILTIN_launder(X) __builtin_launder(X)
#else
/* If launder isn't defined, most compilers should still be able to deal with the aliasing UB */
#  define UTL_BUILTIN_launder(X) X
#endif /* UTL_HAS_BUILTIN(__builtin_launder) */

#if UTL_HAS_BUILTIN(__builtin_addressof)
#  define UTL_BUILTIN_addressof(X) __builtin_addressof(X)
#elif UTL_COMPILER_GCC_AT_LEAST(7, 1, 0)
#  define UTL_BUILTIN_addressof(X) __builtin_addressof(X)
#elif UTL_COMPILER_MSVC_AT_LEAST(1914)
#  define UTL_BUILTIN_addressof(X) __builtin_addressof(X)
#elif UTL_COMPILER_ICC_AT_LEAST(1900)
#  define UTL_BUILTIN_addressof(X) __builtin_addressof(X)
#endif /* ifdef UTL_HAS_BUILTIN(__builtin_addressof) */

#if UTL_HAS_BUILTIN(__builtin_unreachable)
#  define UTL_BUILTIN_unreachable() __builtin_unreachable()
#elif UTL_COMPILER_GCC_AT_LEAST(4, 5, 0)
#  define UTL_BUILTIN_unreachable() __builtin_unreachable()
#elif UTL_COMPILER_ICC_AT_LEAST(1300)
#  define UTL_BUILTIN_unreachable() __builtin_unreachable()
#elif defined(UTL_COMPILER_MSVC) /* UTL_HAS_BUILTIN(__builtin_unreachable) */
#  define UTL_BUILTIN_unreachable() __assume(0)
#else /* UTL_HAS_BUILTIN(__builtin_unreachable) */
#  define UTL_BUILTIN_unreachable() \
      static_assert(false, "`unreachable` is not supported on this compiler")
#endif /* UTL_HAS_BUILTIN(__builtin_unreachable) */

#if UTL_HAS_BUILTIN(__builtin_is_constant_evaluated)
#  define UTL_BUILTIN_is_constant_evaluated() __builtin_is_constant_evaluated()
#elif UTL_COMPILER_MSVC_AT_LEAST(1925) /* UTL_HAS_BUILTIN(__builtin_is_constant_evaluated) */
#  define UTL_BUILTIN_is_constant_evaluated() __builtin_is_constant_evaluated()
#endif /* UTL_HAS_BUILTIN(__builtin_is_constant_evaluated) */

#if UTL_HAS_BUILTIN(__builtin_assume)
#  define UTL_BUILTIN_assume(...) __builtin_assume(__VA_ARGS__)
#elif defined(UTL_COMPILER_MSVC) /* UTL_HAS_BUILTIN(__builtin_unreachable) */
#  define UTL_BUILTIN_assume(...) __assume(__VA_ARGS__)
#elif UTL_HAS_BUILTIN(__builtin_unreachable)
#  define UTL_BUILTIN_assume(...) (__VA_ARGS__) ? (void)0 : __builtin_unreachable()
#else
#  define UTL_BUILTIN_assume(...) (void)0
#endif /* UTL_HAS_BUILTIN(__builtin_assume) */

#if UTL_HAS_BUILTIN(__is_layout_compatible) || UTL_COMPILER_GCC_AT_LEAST(12, 0, 0) || \
    UTL_COMPILER_MSVC_AT_LEAST(1929)
#  define UTL_BUILTIN_is_layout_compatible(...) __is_layout_compatible(__VA_ARGS__)
#endif

#if UTL_HAS_BUILTIN(__is_pointer_interconvertible_base_of) || \
    UTL_COMPILER_GCC_AT_LEAST(12, 0, 0) || UTL_COMPILER_MSVC_AT_LEAST(1929)
#  define UTL_BUILTIN_is_pointer_interconvertible_base_of(...) \
      __is_pointer_interconvertible_base_of(__VA_ARGS__)
#endif
