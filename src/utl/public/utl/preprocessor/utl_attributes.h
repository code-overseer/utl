/* Copyright 2023-2024 Bryan Wong */

#pragma once

#include "utl/preprocessor/utl_attribute_list.h"
#include "utl/preprocessor/utl_builtins.h"
#include "utl/preprocessor/utl_compiler.h"
#include "utl/preprocessor/utl_declspec.h"
#include "utl/preprocessor/utl_msvc_extensions.h"

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

#if UTL_HAS_CPP_ATTRIBUTE(nodiscard)

#  define UTL_NODISCARD [[nodiscard]]
#  define __UTL_ATTRIBUTE_NODISCARD nodiscard
#  define __UTL_ATTRIBUTE_TYPE_CPP_NODISCARD

#elif UTL_HAS_CPP_ATTRIBUTE(gnu::warn_unused_result)

#  define UTL_NODISCARD [[gnu::warn_unused_result]]
#  define __UTL_ATTRIBUTE_NODISCARD gnu::warn_unused_result
#  define __UTL_ATTRIBUTE_TYPE_CPP_NODISCARD

#elif UTL_HAS_CPP_ATTRIBUTE(clang::warn_unused_result)

#  define UTL_NODISCARD [[clang::warn_unused_result]]
#  define __UTL_ATTRIBUTE_NODISCARD clang::warn_unused_result
#  define __UTL_ATTRIBUTE_TYPE_CPP_NODISCARD

#elif UTL_HAS_GNU_ATTRIBUTE(warn_unused_result)

#  define UTL_NODISCARD __attribute__((warn_unused_result))
#  define __UTL_ATTRIBUTE_NODISCARD warn_unused_result
#  define __UTL_ATTRIBUTE_TYPE_GNU_NODISCARD

#elif UTL_HAS_SAL_ANNOTATION(_Check_return_)
#  define UTL_NODISCARD _Check_return_
#  define __UTL_ATTRIBUTE_NODISCARD _Check_return_
#  define __UTL_ATTRIBUTE_TYPE_MSVC_EXT_NODISCARD
#endif /* UTL_HAS_CPP_ATTRIBUTE(nodiscard) */

#if UTL_HAS_CPP_ATTRIBUTE(msvc::lifetimebound)
#  define __UTL_ATTRIBUTE_LIFETIMEBOUND msvc::lifetimebound
#  define __UTL_ATTRIBUTE_TYPE_CPP_LIFETIMEBOUND
#elif UTL_HAS_CPP_ATTRIBUTE(clang::lifetimebound) /* UTL_HAS_CPP_ATTRIBUTE(msvc::lifetimebound) */
#  define __UTL_ATTRIBUTE_LIFETIMEBOUND clang::lifetimebound
#  define __UTL_ATTRIBUTE_TYPE_CPP_LIFETIMEBOUND
#elif UTL_HAS_GNU_ATTRIBUTE(lifetimebound) /* UTL_HAS_CPP_ATTRIBUTE(msvc::lifetimebound) */
#  define __UTL_ATTRIBUTE_LIFETIMEBOUND lifetimebound
#  define __UTL_ATTRIBUTE_TYPE_GNU_LIFETIMEBOUND
#endif /* UTL_HAS_CPP_ATTRIBUTE(msvc::lifetimebound) */

#if UTL_HAS_CPP_ATTRIBUTE(gnu::pure)
#  define __UTL_ATTRIBUTE_PURE gnu::pure
#  define __UTL_ATTRIBUTE_TYPE_CPP_PURE
#elif UTL_HAS_GNU_ATTRIBUTE(pure) /* UTL_HAS_CPP_ATTRIBUTE(gnu::pure) */
#  define __UTL_ATTRIBUTE_PURE pure
#  define __UTL_ATTRIBUTE_TYPE_GNU_PURE
#endif /* UTL_HAS_CPP_ATTRIBUTE(gnu::pure) */

#if UTL_HAS_CPP_ATTRIBUTE(gnu::const)
#  define __UTL_ATTRIBUTE_CONST gnu::const
#  define __UTL_ATTRIBUTE_TYPE_CPP_CONST
#elif UTL_HAS_GNU_ATTRIBUTE(const)
#  define __UTL_ATTRIBUTE_CONST const
#  define __UTL_ATTRIBUTE_TYPE_GNU_CONST
#elif UTL_HAS_DECLSPEC(noalias)
#  define __UTL_ATTRIBUTE_CONST noalias
#  define __UTL_ATTRIBUTE_TYPE_DECLSPEC_CONST
#endif /* UTL_HAS_CPP_ATTRIBUTE(gnu::const) */

#if UTL_HAS_CPP_ATTRIBUTE(msvc::intrinsic)
#  define __UTL_ATTRIBUTE_INTRINSIC msvc::intrinsic
#  define __UTL_ATTRIBUTE_TYPE_CPP_INTRINSIC
#endif /* UTL_HAS_CPP_ATTRIBUTE(msvc::intrinsic) */

#if UTL_HAS_CPP_ATTRIBUTE(gnu::flatten)
#  define __UTL_ATTRIBUTE_FLATTEN gnu::flatten
#  define __UTL_ATTRIBUTE_TYPE_CPP_FLATTEN
#elif UTL_HAS_CPP_ATTRIBUTE(msvc::flatten)
#  define __UTL_ATTRIBUTE_FLATTEN msvc::flatten
#  define __UTL_ATTRIBUTE_TYPE_CPP_FLATTEN
#elif UTL_HAS_GNU_ATTRIBUTE(flatten)
#  define __UTL_ATTRIBUTE_FLATTEN flatten
#  define __UTL_ATTRIBUTE_TYPE_GNU_FLATTEN
#endif /* UTL_HAS_CPP_ATTRIBUTE(gnu::const) */

#if UTL_HAS_CPP_ATTRIBUTE(msvc::no_unique_address)
#  define __UTL_ATTRIBUTE_NO_UNIQUE_ADDRESS msvc::no_unique_address
#  define __UTL_ATTRIBUTE_TYPE_CPP_NO_UNIQUE_ADDRESS
#elif UTL_HAS_CPP_ATTRIBUTE(no_unique_address)
#  define __UTL_ATTRIBUTE_NO_UNIQUE_ADDRESS no_unique_address
#  define __UTL_ATTRIBUTE_TYPE_CPP_NO_UNIQUE_ADDRESS
#elif UTL_HAS_GNU_ATTRIBUTE(no_unique_address)
#  define __UTL_ATTRIBUTE_NO_UNIQUE_ADDRESS no_unique_address
#  define __UTL_ATTRIBUTE_TYPE_GNU_NO_UNIQUE_ADDRESS
#endif /* UTL_HAS_CPP_ATTRIBUTE(msvc::no_unique_address) */

#if UTL_HAS_CPP_ATTRIBUTE(clang::always_inline)
#  define __UTL_ATTRIBUTE_ALWAYS_INLINE clang::always_inline
#  define __UTL_ATTRIBUTE_TYPE_CPP_ALWAYS_INLINE
#elif UTL_HAS_CPP_ATTRIBUTE(gnu::always_inline)
#  define __UTL_ATTRIBUTE_ALWAYS_INLINE gnu::always_inline
#  define __UTL_ATTRIBUTE_TYPE_CPP_ALWAYS_INLINE
#elif UTL_HAS_GNU_ATTRIBUTE(always_inline)
#  define __UTL_ATTRIBUTE_ALWAYS_INLINE always_inline
#  define __UTL_ATTRIBUTE_TYPE_GNU_ALWAYS_INLINE
#elif UTL_HAS_MSVC_DECORATOR(__forceinline)
#  define __UTL_ATTRIBUTE_ALWAYS_INLINE __forceinline
#  define __UTL_ATTRIBUTE_TYPE_MSVC_EXT_ALWAYS_INLINE
#endif /* UTL_HAS_CPP_ATTRIBUTE(clang::always_inline) */

#if UTL_HAS_CPP_ATTRIBUTE(clang::malloc)
#  define __UTL_ATTRIBUTE_MALLOC clang::malloc
#  define __UTL_ATTRIBUTE_TYPE_CPP_MALLOC
#elif UTL_HAS_CPP_ATTRIBUTE(gnu::malloc)
#  define __UTL_ATTRIBUTE_MALLOC gnu::malloc
#  define __UTL_ATTRIBUTE_TYPE_CPP_MALLOC
#elif UTL_HAS_GNU_ATTRIBUTE(malloc)
#  define __UTL_ATTRIBUTE_MALLOC malloc
#  define __UTL_ATTRIBUTE_TYPE_GNU_MALLOC
#endif /* UTL_HAS_CPP_ATTRIBUTE(clang::malloc) */

#if UTL_HAS_CPP_ATTRIBUTE(clang::reinitializes)
#  define __UTL_ATTRIBUTE_REINITIALIZES clang::reinitializes
#  define __UTL_ATTRIBUTE_TYPE_CPP_REINITIALIZES
#elif UTL_HAS_CPP_ATTRIBUTE(gnu::reinitializes)
#  define __UTL_ATTRIBUTE_REINITIALIZES gnu::reinitializes
#  define __UTL_ATTRIBUTE_TYPE_CPP_REINITIALIZES
#elif UTL_HAS_GNU_ATTRIBUTE(reinitializes)
#  define __UTL_ATTRIBUTE_REINITIALIZES reinitializes
#  define __UTL_ATTRIBUTE_TYPE_GNU_REINITIALIZES
#endif /* UTL_HAS_CPP_ATTRIBUTE(clang::reinitializes) */

#if UTL_HAS_CPP_ATTRIBUTE(maybe_unused)
#  define __UTL_ATTRIBUTE_MAYBE_UNUSED maybe_unused
#  define __UTL_ATTRIBUTE_TYPE_CPP_MAYBE_UNUSED
#elif UTL_HAS_CPP_ATTRIBUTE(gnu::unused)
#  define __UTL_ATTRIBUTE_MAYBE_UNUSED gnu::unused
#  define __UTL_ATTRIBUTE_TYPE_CPP_MAYBE_UNUSED
#elif UTL_HAS_GNU_ATTRIBUTE(unused)
#  define __UTL_ATTRIBUTE_MAYBE_UNUSED unused
#  define __UTL_ATTRIBUTE_TYPE_GNU_MAYBE_UNUSED
#endif /* UTL_HAS_CPP_ATTRIBUTE(maybe_unused) */

#if UTL_HAS_CPP_ATTRIBUTE(likely)
#  define __UTL_ATTRIBUTE_LIKELY likely
#  define __UTL_ATTRIBUTE_TYPE_CPP_LIKELY
#endif /* UTL_HAS_CPP_ATTRIBUTE(likely) */

#if UTL_HAS_CPP_ATTRIBUTE(unlikely)
#  define __UTL_ATTRIBUTE_UNLIKELY unlikely
#  define __UTL_ATTRIBUTE_TYPE_CPP_UNLIKELY
#endif /* UTL_HAS_CPP_ATTRIBUTE(unlikely) */

#ifndef UTL_NODISCARD
#  define UTL_NODISCARD
#endif /* ifndef UTL_NODISCARD */
