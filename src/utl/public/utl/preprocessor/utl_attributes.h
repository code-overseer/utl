/* Copyright 2023-2024 Bryan Wong */

#pragma once

#include "utl/preprocessor/utl_attribute_list.h"
#include "utl/preprocessor/utl_builtins.h"
#include "utl/preprocessor/utl_compiler.h"

#ifdef __has_cpp_attribute
#  define UTL_HAS_CPP_ATTRIBUTE(...) __has_cpp_attribute(__VA_ARGS__)
#else /* ifdef __has_cpp_attribute */
#  define UTL_HAS_CPP_ATTRIBUTE(...) 0
#endif /* ifdef __has_cpp_attribute */

#ifdef __has_attribute
#  define UTL_HAS_GNU_ATTRIBUTE(...) __has_attribute(__VA_ARGS__)
#else /* ifdef __has_attribute */
#  define UTL_HAS_GNU_ATTRIBUTE(...) 0
#endif /* ifdef __has_attribute */

#if UTL_COMPILER_MSVC
#  include "utl/preprocessor/utl_declspec.h"
#endif

#if UTL_HAS_CPP_ATTRIBUTE(nodiscard)

#  define UTL_NODISCARD [[nodiscard]]
#  define UTL_ATTRIBUTE_NODISCARD nodiscard

#elif UTL_HAS_CPP_ATTRIBUTE(gnu::warn_unused_result)

#  define UTL_NODISCARD [[gnu::warn_unused_result]]
#  define UTL_ATTRIBUTE_NODISCARD gnu::warn_unused_result

#elif UTL_HAS_CPP_ATTRIBUTE(clang::warn_unused_result)

#  define UTL_NODISCARD [[clang::warn_unused_result]]
#  define UTL_ATTRIBUTE_NODISCARD clang::warn_unused_result

#elif UTL_HAS_GNU_ATTRIBUTE(warn_unused_result)

#  define UTL_ATTRIBUTE_NODISCARD
#  define UTL_NODISCARD __attribute__((warn_unused_result))
#  define UTL_NONSTD_ATTRIBUTE_NODISCARD __attribute__((warn_unused_result))
#  define UTL_NONSTD_ATTRIBUTE_NODISCARD_DEFINED

#endif /* UTL_HAS_CPP_ATTRIBUTE(nodiscard) */

#if UTL_HAS_CPP_ATTRIBUTE(msvc::lifetimebound)
#  define UTL_ATTRIBUTE_LIFETIMEBOUND msvc::lifetimebound
#elif UTL_HAS_CPP_ATTRIBUTE(clang::lifetimebound) /* UTL_HAS_CPP_ATTRIBUTE(msvc::lifetimebound) */
#  define UTL_ATTRIBUTE_LIFETIMEBOUND clang::lifetimebound
#elif UTL_HAS_GNU_ATTRIBUTE(lifetimebound) /* UTL_HAS_CPP_ATTRIBUTE(msvc::lifetimebound) */
#  define UTL_NONSTD_ATTRIBUTE_LIFETIMEBOUND __attribute__((lifetimebound))
#  define UTL_NONSTD_ATTRIBUTE_LIFETIMEBOUND_DEFINED
#endif /* UTL_HAS_CPP_ATTRIBUTE(msvc::lifetimebound) */

#if UTL_HAS_CPP_ATTRIBUTE(gnu::pure)
#  define UTL_ATTRIBUTE_PURE gnu::pure
#elif UTL_HAS_GNU_ATTRIBUTE(pure) /* UTL_HAS_CPP_ATTRIBUTE(gnu::pure) */
#  define UTL_NONSTD_ATTRIBUTE_PURE __attribute__((pure))
#  define UTL_NONSTD_ATTRIBUTE_PURE_DEFINED
#endif /* UTL_HAS_CPP_ATTRIBUTE(gnu::pure) */

#if UTL_HAS_CPP_ATTRIBUTE(gnu::const)
#  define UTL_ATTRIBUTE_CONST gnu::const
#elif UTL_HAS_GNU_ATTRIBUTE(const)
#  define UTL_NONSTD_ATTRIBUTE_CONST __attribute__((const))
#  define UTL_NONSTD_ATTRIBUTE_CONST_DEFINED
#elif UTL_COMPILER_MSVC
#  define UTL_NONSTD_ATTRIBUTE_CONST __declspec(noalias)
#  define UTL_NONSTD_ATTRIBUTE_CONST_DEFINED
#endif /* UTL_HAS_CPP_ATTRIBUTE(gnu::const) */

#if UTL_HAS_CPP_ATTRIBUTE(msvc::intrinsic)
#  define UTL_ATTRIBUTE_INTRINSIC msvc::intrinsic
#endif /* UTL_HAS_CPP_ATTRIBUTE(msvc::intrinsic) */

#if UTL_HAS_CPP_ATTRIBUTE(gnu::flatten)
#  define UTL_ATTRIBUTE_FLATTEN gnu::flatten
#elif UTL_HAS_CPP_ATTRIBUTE(msvc::flatten)
#  define UTL_ATTRIBUTE_FLATTEN msvc::flatten
#elif UTL_HAS_GNU_ATTRIBUTE(flatten)
#  define UTL_NONSTD_ATTRIBUTE_FLATTEN __attribute__((flatten))
#  define UTL_NONSTD_ATTRIBUTE_FLATTEN_DEFINED
#endif /* UTL_HAS_CPP_ATTRIBUTE(gnu::const) */

#if UTL_HAS_CPP_ATTRIBUTE(msvc::no_unique_address)
#  define UTL_ATTRIBUTE_NO_UNIQUE_ADDRESS msvc::no_unique_address
#elif UTL_HAS_CPP_ATTRIBUTE(no_unique_address)
#  define UTL_ATTRIBUTE_NO_UNIQUE_ADDRESS no_unique_address
#elif UTL_HAS_GNU_ATTRIBUTE(no_unique_address)
#  define UTL_NONSTD_ATTRIBUTE_NO_UNIQUE_ADDRESS __attribute__((no_unique_address))
#  define UTL_NONSTD_ATTRIBUTE_NO_UNIQUE_ADDRESS_DEFINED
#endif /* UTL_HAS_CPP_ATTRIBUTE(msvc::no_unique_address) */

#if UTL_HAS_CPP_ATTRIBUTE(clang::always_inline)
#  define UTL_ATTRIBUTE_ALWAYS_INLINE clang::always_inline
#elif UTL_HAS_CPP_ATTRIBUTE(gnu::always_inline)
#  define UTL_ATTRIBUTE_ALWAYS_INLINE gnu::always_inline
#elif UTL_HAS_GNU_ATTRIBUTE(always_inline)
#  define UTL_NONSTD_ATTRIBUTE_ALWAYS_INLINE __attribute__((always_inline))
#  define UTL_NONSTD_ATTRIBUTE_ALWAYS_INLINE_DEFINED
#endif /* UTL_HAS_CPP_ATTRIBUTE(clang::always_inline) */
#if UTL_COMPILER_MSVC
#  if !defined(UTL_ATTRIBUTE_ALWAYS_INLINE) && !defined(UTL_NONSTD_ATTRIBUTE_ALWAYS_INLINE)
#    define UTL_NONSTD_ATTRIBUTE_ALWAYS_INLINE __forceinline
#    define UTL_NONSTD_ATTRIBUTE_ALWAYS_INLINE_DEFINED
#  endif
#endif

#if UTL_HAS_CPP_ATTRIBUTE(clang::malloc)
#  define UTL_ATTRIBUTE_MALLOC clang::malloc
#elif UTL_HAS_CPP_ATTRIBUTE(gnu::malloc)
#  define UTL_ATTRIBUTE_MALLOC gnu::malloc
#elif UTL_HAS_GNU_ATTRIBUTE(malloc)
#  define UTL_NONSTD_ATTRIBUTE_MALLOC __attribute__((malloc))
#  define UTL_NONSTD_ATTRIBUTE_MALLOC_DEFINED
#endif /* UTL_HAS_CPP_ATTRIBUTE(clang::malloc) */

#if UTL_HAS_CPP_ATTRIBUTE(clang::reinitializes)
#  define UTL_ATTRIBUTE_REINITIALIZES clang::reinitializes
#elif UTL_HAS_CPP_ATTRIBUTE(gnu::reinitializes)
#  define UTL_ATTRIBUTE_REINITIALIZES gnu::reinitializes
#elif UTL_HAS_GNU_ATTRIBUTE(reinitializes)
#  define UTL_NONSTD_ATTRIBUTE_REINITIALIZES __attribute__((reinitializes))
#  define UTL_NONSTD_ATTRIBUTE_REINITIALIZES_DEFINED
#endif /* UTL_HAS_CPP_ATTRIBUTE(clang::reinitializes) */

#if UTL_HAS_CPP_ATTRIBUTE(maybe_unused)
#  define UTL_ATTRIBUTE_MAYBE_UNUSED maybe_unused
#elif UTL_HAS_CPP_ATTRIBUTE(gnu::unused)
#  define UTL_ATTRIBUTE_MAYBE_UNUSED gnu::unused
#elif UTL_HAS_GNU_ATTRIBUTE(unused)
#  define UTL_NONSTD_ATTRIBUTE_MAYBE_UNUSED __attribute__((unused))
#  define UTL_NONSTD_ATTRIBUTE_MAYBE_UNUSED_DEFINED
#endif /* UTL_HAS_CPP_ATTRIBUTE(maybe_unused) */

#if UTL_HAS_CPP_ATTRIBUTE(likely)
#  define UTL_ATTRIBUTE_LIKELY likely
#endif /* UTL_HAS_CPP_ATTRIBUTE(likely) */

#if UTL_HAS_CPP_ATTRIBUTE(unlikely)
#  define UTL_ATTRIBUTE_UNLIKELY unlikely
#endif /* UTL_HAS_CPP_ATTRIBUTE(unlikely) */

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
#ifndef UTL_ATTRIBUTE_MALLOC
#  define UTL_ATTRIBUTE_MALLOC
#endif /* #ifndef UTL_ATTRIBUTE_MALLOC */
#ifndef UTL_ATTRIBUTE_REINITIALIZES
#  define UTL_ATTRIBUTE_REINITIALIZES
#endif /* #ifndef UTL_ATTRIBUTE_REINITIALIZES */
#ifndef UTL_ATTRIBUTE_MAYBE_UNUSED
#  define UTL_ATTRIBUTE_MAYBE_UNUSED
#endif /* #ifndef UTL_ATTRIBUTE_MAYBE_UNUSED */
#ifndef UTL_ATTRIBUTE_LIKELY
#  define UTL_ATTRIBUTE_LIKELY
#endif /* #ifndef UTL_ATTRIBUTE_LIKELY */
#ifndef UTL_ATTRIBUTE_UNLIKELY
#  define UTL_ATTRIBUTE_UNLIKELY
#endif /* #ifndef UTL_ATTRIBUTE_UNLIKELY */
