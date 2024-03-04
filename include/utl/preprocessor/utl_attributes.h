/* Copyright 2023-2024 Bryan Wong */

#pragma once

#include "utl/preprocessor/utl_attribute_list.h"
#include "utl/preprocessor/utl_compiler.h"

#ifdef __has_cpp_attribute
#  define UTL_HAS_CPP_ATTRIBUTE(...) __has_cpp_attribute(__VA_ARGS__)
#else /* ifdef __has_cpp_attribute */
#  define UTL_HAS_CPP_ATTRIBUTE(...) 0
#endif /* ifdef __has_cpp_attribute */

#ifdef __has_attribute
#  define UTL_HAS_ATTRIBUTE(...) __has_attribute(__VA_ARGS__)
#else /* ifdef __has_attribute */
#  define UTL_HAS_ATTRIBUTE(...) 0
#endif /* ifdef __has_attribute */

#if UTL_HAS_CPP_ATTRIBUTE(nodiscard)

#  define UTL_NODISCARD [[nodiscard]]
#  define UTL_ATTRIBUTE_NODISCARD nodiscard

#elif UTL_HAS_CPP_ATTRIBUTE(gnu::warn_unused_result)

#  define UTL_NODISCARD [[gnu::warn_unused_result]]
#  define UTL_ATTRIBUTE_NODISCARD gnu::warn_unused_result

#elif UTL_HAS_CPP_ATTRIBUTE(clang::warn_unused_result)

#  define UTL_NODISCARD [[clang::warn_unused_result]]
#  define UTL_ATTRIBUTE_NODISCARD clang::warn_unused_result

#elif UTL_HAS_ATTRIBUTE(warn_unused_result)

#  define UTL_ATTRIBUTE_NODISCARD
#  define UTL_NODISCARD __attribute__((warn_unused_result))
#  define UTL_NONSTD_ATTRIBUTE_NODISCARD __attribute__((warn_unused_result))
#  define UTL_NONSTD_ATTRIBUTE_NODISCARD_DEFINED

#elif UTL_COMPILER_MSVC_AT_LEAST(1700)

#  define UTL_NODISCARD _Check_return_
#  define UTL_NONSTD_ATTRIBUTE_NODISCARD _Check_return_
#  define UTL_NONSTD_ATTRIBUTE_NODISCARD_DEFINED

#endif /* UTL_HAS_CPP_ATTRIBUTE(nodiscard) */

#if UTL_HAS_CPP_ATTRIBUTE(msvc::lifetimebound)
#  define UTL_ATTRIBUTE_LIFETIMEBOUND msvc::lifetimebound
#elif UTL_HAS_CPP_ATTRIBUTE(clang::lifetimebound) /* UTL_HAS_CPP_ATTRIBUTE(msvc::lifetimebound) */
#  define UTL_ATTRIBUTE_LIFETIMEBOUND clang::lifetimebound
#elif UTL_HAS_ATTRIBUTE(lifetimebound) /* UTL_HAS_CPP_ATTRIBUTE(msvc::lifetimebound) */
#  define UTL_NONSTD_ATTRIBUTE_LIFETIMEBOUND __attribute__((lifetimebound))
#  define UTL_NONSTD_ATTRIBUTE_LIFETIMEBOUND_DEFINED
#endif /* UTL_HAS_CPP_ATTRIBUTE(msvc::lifetimebound) */

#if UTL_HAS_CPP_ATTRIBUTE(gnu::pure)
#  define UTL_ATTRIBUTE_PURE gnu::pure
#elif UTL_HAS_ATTRIBUTE(pure) /* UTL_HAS_CPP_ATTRIBUTE(gnu::pure) */
#  define UTL_NONSTD_ATTRIBUTE_PURE __attribute__((pure))
#  define UTL_NONSTD_ATTRIBUTE_PURE_DEFINED
#endif /* UTL_HAS_CPP_ATTRIBUTE(gnu::pure) */

#if UTL_HAS_CPP_ATTRIBUTE(gnu::const)
#  define UTL_ATTRIBUTE_CONST gnu::const
#elif UTL_HAS_ATTRIBUTE(const)
#  define UTL_NONSTD_ATTRIBUTE_CONST __attribute__((const))
#  define UTL_NONSTD_ATTRIBUTE_CONST_DEFINED
#endif /* UTL_HAS_CPP_ATTRIBUTE(gnu::const) */

#if UTL_HAS_CPP_ATTRIBUTE(msvc::intrinsic)
#  define UTL_ATTRIBUTE_INTRINSIC msvc::intrinsic
#endif /* UTL_HAS_CPP_ATTRIBUTE(msvc::intrinsic) */

#if UTL_HAS_CPP_ATTRIBUTE(gnu::flatten)
#  define UTL_ATTRIBUTE_FLATTEN gnu::flatten
#elif UTL_HAS_CPP_ATTRIBUTE(msvc::flatten)
#  define UTL_ATTRIBUTE_FLATTEN msvc::flatten
#elif UTL_HAS_ATTRIBUTE(flatten)
#  define UTL_NONSTD_ATTRIBUTE_FLATTEN __attribute__((flatten))
#  define UTL_NONSTD_ATTRIBUTE_FLATTEN_DEFINED
#endif /* UTL_HAS_CPP_ATTRIBUTE(gnu::const) */

#if UTL_HAS_CPP_ATTRIBUTE(msvc::no_unique_address)
#  define UTL_ATTRIBUTE_NO_UNIQUE_ADDRESS msvc::no_unique_address
#elif UTL_HAS_CPP_ATTRIBUTE(no_unique_address)
#  define UTL_ATTRIBUTE_NO_UNIQUE_ADDRESS no_unique_address
#elif UTL_HAS_ATTRIBUTE(no_unique_address)
#  define UTL_NONSTD_ATTRIBUTE_NO_UNIQUE_ADDRESS __attribute__((no_unique_address))
#  define UTL_NONSTD_ATTRIBUTE_NO_UNIQUE_ADDRESS_DEFINED
#endif /* UTL_HAS_CPP_ATTRIBUTE(msvc::no_unique_address) */

#if UTL_HAS_CPP_ATTRIBUTE(clang::always_inline)
#  define UTL_ATTRIBUTE_ALWAYS_INLINE clang::always_inline
#elif UTL_HAS_CPP_ATTRIBUTE(gnu::always_inline)
#  define UTL_ATTRIBUTE_ALWAYS_INLINE gnu::always_inline
#elif UTL_HAS_ATTRIBUTE(always_inline)
#  define UTL_NONSTD_ATTRIBUTE_ALWAYS_INLINE __attribute__((always_inline))
#  define UTL_NONSTD_ATTRIBUTE_ALWAYS_INLINE_DEFINED
#endif /* UTL_HAS_CPP_ATTRIBUTE(clang::always_inline) */
#if defined(UTL_COMPILER_MSVC)
#  if !defined(UTL_ATTRIBUTE_ALWAYS_INLINE) && !defined(UTL_NONSTD_ATTRIBUTE_ALWAYS_INLINE)
#    define UTL_NONSTD_ATTRIBUTE_ALWAYS_INLINE __forceinline
#    define UTL_NONSTD_ATTRIBUTE_ALWAYS_INLINE_DEFINED
#  endif
#endif

#ifndef UTL_NODISCARD
#  define UTL_NODISCARD
#endif /* ifndef UTL_NODISCARD */

#ifndef UTL_ATTRIBUTE_CONST
#  define UTL_ATTRIBUTE_CONST
#endif /* ifndef UTL_ATTRIBUTE_CONST */
#ifndef UTL_ATTRIBUTE_INTRINSIC
#  define UTL_ATTRIBUTE_INTRINSIC
#endif /* ifndef UTL_ATTRIBUTE_INTRINSIC */
#ifndef UTL_ATTRIBUTE_LIFETIMEBOUND
#  define UTL_ATTRIBUTE_LIFETIMEBOUND
#endif /* ifndef UTL_ATTRIBUTE_LIFETIMEBOUND */
#ifndef UTL_ATTRIBUTE_NODISCARD
#  define UTL_ATTRIBUTE_NODISCARD
#endif /* ifndef UTL_ATTRIBUTE_NODISCARD */
#ifndef UTL_ATTRIBUTE_PURE
#  define UTL_ATTRIBUTE_PURE
#endif /* ifndef UTL_ATTRIBUTE_PURE */
#ifndef UTL_ATTRIBUTE_FLATTEN
#  define UTL_ATTRIBUTE_FLATTEN
#endif /* ifndef UTL_ATTRIBUTE_FLATTEN */
#ifndef UTL_ATTRIBUTE_NO_UNIQUE_ADDRESS
#  define UTL_ATTRIBUTE_NO_UNIQUE_ADDRESS
#endif /* ifndef UTL_ATTRIBUTE_NO_UNIQUE_ADDRESS */
#ifndef UTL_ATTRIBUTE_ALWAYS_INLINE
#  define UTL_ATTRIBUTE_ALWAYS_INLINE
#endif /* #ifndef UTL_ATTRIBUTE_ALWAYS_INLINE */
