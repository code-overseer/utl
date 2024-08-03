/* Copyright 2023-2024 Bryan Wong */

#pragma once

#include "utl/preprocessor/utl_attribute_check.h"
#include "utl/preprocessor/utl_attribute_list.h"
#include "utl/preprocessor/utl_builtins.h"
#include "utl/preprocessor/utl_compiler.h"
#include "utl/preprocessor/utl_declspec.h"
#include "utl/preprocessor/utl_msvc_extensions.h"

#if UTL_HAS_CPP_ATTRIBUTE(nodiscard)
#  define __UTL_ATTRIBUTE_NODISCARD nodiscard
#  define __UTL_ATTRIBUTE_TYPE_CPP_NODISCARD
#elif UTL_HAS_CPP_ATTRIBUTE(gnu::warn_unused_result)
#  define __UTL_ATTRIBUTE_NODISCARD gnu::warn_unused_result
#  define __UTL_ATTRIBUTE_TYPE_CPP_NODISCARD
#elif UTL_HAS_CPP_ATTRIBUTE(clang::warn_unused_result)
#  define __UTL_ATTRIBUTE_NODISCARD clang::warn_unused_result
#  define __UTL_ATTRIBUTE_TYPE_CPP_NODISCARD
#elif UTL_HAS_GNU_ATTRIBUTE(__warn_unused_result__)
#  define __UTL_ATTRIBUTE_NODISCARD __warn_unused_result__
#  define __UTL_ATTRIBUTE_TYPE_GNU_NODISCARD
#elif UTL_HAS_SAL_ANNOTATION(_Check_return_)
#  define __UTL_ATTRIBUTE_NODISCARD _Check_return_
#  define __UTL_ATTRIBUTE_TYPE_MSVC_EXT_NODISCARD
#endif /* UTL_HAS_CPP_ATTRIBUTE(nodiscard) */

#if UTL_HAS_CPP_ATTRIBUTE(msvc::lifetimebound)
#  define __UTL_ATTRIBUTE_LIFETIMEBOUND msvc::lifetimebound
#  define __UTL_ATTRIBUTE_TYPE_CPP_LIFETIMEBOUND
#elif UTL_HAS_CPP_ATTRIBUTE(clang::lifetimebound) /* UTL_HAS_CPP_ATTRIBUTE(msvc::lifetimebound) */
#  define __UTL_ATTRIBUTE_LIFETIMEBOUND clang::lifetimebound
#  define __UTL_ATTRIBUTE_TYPE_CPP_LIFETIMEBOUND
#elif UTL_HAS_GNU_ATTRIBUTE(__lifetimebound__) /* UTL_HAS_CPP_ATTRIBUTE(msvc::lifetimebound) */
#  define __UTL_ATTRIBUTE_LIFETIMEBOUND __lifetimebound__
#  define __UTL_ATTRIBUTE_TYPE_GNU_LIFETIMEBOUND
#endif /* UTL_HAS_CPP_ATTRIBUTE(msvc::lifetimebound) */

#if UTL_HAS_CPP_ATTRIBUTE(gnu::pure)
#  define __UTL_ATTRIBUTE_PURE gnu::pure
#  define __UTL_ATTRIBUTE_TYPE_CPP_PURE
#elif UTL_HAS_GNU_ATTRIBUTE(__pure__) /* UTL_HAS_CPP_ATTRIBUTE(gnu::pure) */
#  define __UTL_ATTRIBUTE_PURE __pure__
#  define __UTL_ATTRIBUTE_TYPE_GNU_PURE
#endif /* UTL_HAS_CPP_ATTRIBUTE(gnu::pure) */

#if UTL_HAS_CPP_ATTRIBUTE(gnu::const)
#  define __UTL_ATTRIBUTE_CONST gnu::const
#  define __UTL_ATTRIBUTE_TYPE_CPP_CONST
#elif UTL_HAS_GNU_ATTRIBUTE(__const__)
#  define __UTL_ATTRIBUTE_CONST __const__
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
#elif UTL_HAS_GNU_ATTRIBUTE(__flatten__)
#  define __UTL_ATTRIBUTE_FLATTEN __flatten__
#  define __UTL_ATTRIBUTE_TYPE_GNU_FLATTEN
#endif /* UTL_HAS_CPP_ATTRIBUTE(gnu::const) */

#if UTL_HAS_CPP_ATTRIBUTE(msvc::no_unique_address)
#  define __UTL_ATTRIBUTE_NO_UNIQUE_ADDRESS msvc::no_unique_address
#  define __UTL_ATTRIBUTE_TYPE_CPP_NO_UNIQUE_ADDRESS
#elif UTL_HAS_CPP_ATTRIBUTE(no_unique_address)
#  define __UTL_ATTRIBUTE_NO_UNIQUE_ADDRESS no_unique_address
#  define __UTL_ATTRIBUTE_TYPE_CPP_NO_UNIQUE_ADDRESS
#elif UTL_HAS_GNU_ATTRIBUTE(__no_unique_address__)
#  define __UTL_ATTRIBUTE_NO_UNIQUE_ADDRESS __no_unique_address__
#  define __UTL_ATTRIBUTE_TYPE_GNU_NO_UNIQUE_ADDRESS
#endif /* UTL_HAS_CPP_ATTRIBUTE(msvc::no_unique_address) */

#if UTL_HAS_CPP_ATTRIBUTE(clang::always_inline)
#  define __UTL_ATTRIBUTE_ALWAYS_INLINE clang::always_inline
#  define __UTL_ATTRIBUTE_TYPE_CPP_ALWAYS_INLINE
#elif UTL_HAS_CPP_ATTRIBUTE(gnu::always_inline)
#  define __UTL_ATTRIBUTE_ALWAYS_INLINE gnu::always_inline
#  define __UTL_ATTRIBUTE_TYPE_CPP_ALWAYS_INLINE
#elif UTL_HAS_GNU_ATTRIBUTE(__always_inline__)
#  define __UTL_ATTRIBUTE_ALWAYS_INLINE __always_inline__
#  define __UTL_ATTRIBUTE_TYPE_GNU_ALWAYS_INLINE
#elif UTL_HAS_MSVC_KEYWORD(__forceinline)
#  define __UTL_ATTRIBUTE_ALWAYS_INLINE __forceinline
#  define __UTL_ATTRIBUTE_TYPE_MSVC_EXT_ALWAYS_INLINE
#endif /* UTL_HAS_CPP_ATTRIBUTE(clang::always_inline) */

#if UTL_HAS_CPP_ATTRIBUTE(clang::malloc)
#  define __UTL_ATTRIBUTE_MALLOC clang::malloc
#  define __UTL_ATTRIBUTE_TYPE_CPP_MALLOC
#elif UTL_HAS_CPP_ATTRIBUTE(gnu::malloc)
#  define __UTL_ATTRIBUTE_MALLOC gnu::malloc
#  define __UTL_ATTRIBUTE_TYPE_CPP_MALLOC
#elif UTL_HAS_GNU_ATTRIBUTE(__malloc__)
#  define __UTL_ATTRIBUTE_MALLOC __malloc__
#  define __UTL_ATTRIBUTE_TYPE_GNU_MALLOC
#elif UTL_HAS_DECLSPEC(allocator) | UTL_HAS_DECLSPEC(restrict)
#  if UTL_HAS_DECLSPEC(allocator) & UTL_HAS_DECLSPEC(restrict)
#    define __UTL_ATTRIBUTE_MALLOC allocator, restrict
#    define __UTL_ATTRIBUTE_TYPE_DECLSPEC_MALLOC
#  elif UTL_HAS_DECLSPEC(allocator)
#    define __UTL_ATTRIBUTE_MALLOC allocator
#    define __UTL_ATTRIBUTE_TYPE_DECLSPEC_MALLOC
#  endif
#endif /* UTL_HAS_CPP_ATTRIBUTE(clang::malloc) */

#if UTL_COMPILER_GCC
#  if UTL_HAS_CPP_ATTRIBUTE(gnu::malloc)
#    define __UTL_ATTRIBUTE_DEALLOCATED_BY gnu::malloc
#    define __UTL_ATTRIBUTE_TYPE_CPP_DEALLOCATED_BY(FUNC, ...)
#  elif UTL_HAS_GNU_ATTRIBUTE(__malloc__)
#    define __UTL_ATTRIBUTE_DEALLOCATED_BY __malloc__
#    define __UTL_ATTRIBUTE_TYPE_GNU_DEALLOCATED_BY(FUNC, ...)
#  endif /* UTL_HAS_CPP_ATTRIBUTE(clang::malloc) */
#endif

#if UTL_HAS_CPP_ATTRIBUTE(visibility)
#  define __UTL_ATTRIBUTE_VISIBILITY visibility
#  define __UTL_ATTRIBUTE_TYPE_CPP_VISIBILITY(VISIBILITY)
#elif UTL_HAS_CPP_ATTRIBUTE(gnu::visibility)
#  define __UTL_ATTRIBUTE_VISIBILITY gnu::visibility
#  define __UTL_ATTRIBUTE_TYPE_CPP_VISIBILITY(VISIBILITY)
#elif UTL_HAS_GNU_ATTRIBUTE(__visibility__)
#  define __UTL_ATTRIBUTE_VISIBILITY __visibility__
#  define __UTL_ATTRIBUTE_TYPE_CPP_VISIBILITY(VISIBILITY)
#endif

#if UTL_HAS_CPP_ATTRIBUTE(type_visibility)
#  define __UTL_ATTRIBUTE_TYPE_VISIBILITY type_visibility
#  define __UTL_ATTRIBUTE_TYPE_CPP_TYPE_VISIBILITY(VISIBILITY)
#elif UTL_HAS_CPP_ATTRIBUTE(clang::type_visibility)
#  define __UTL_ATTRIBUTE_TYPE_VISIBILITY clang::type_visibility
#  define __UTL_ATTRIBUTE_TYPE_CPP_TYPE_VISIBILITY(VISIBILITY)
#elif UTL_HAS_CPP_ATTRIBUTE(gnu::type_visibility)
#  define __UTL_ATTRIBUTE_TYPE_VISIBILITY gnu::type_visibility
#  define __UTL_ATTRIBUTE_TYPE_CPP_TYPE_VISIBILITY(VISIBILITY)
#elif UTL_HAS_GNU_ATTRIBUTE(__type_visibility__)
#  define __UTL_ATTRIBUTE_TYPE_VISIBILITY __type_visibility__
#  define __UTL_ATTRIBUTE_TYPE_CPP_TYPE_VISIBILITY(VISIBILITY)
#endif

#if UTL_HAS_CPP_ATTRIBUTE(gnu::dllimport)
#  define __UTL_ATTRIBUTE_DLLIMPORT gnu::dllimport
#  define __UTL_ATTRIBUTE_TYPE_CPP_DLLIMPORT
#elif UTL_HAS_GNU_ATTRIBUTE(__dllimport__)
#  define __UTL_ATTRIBUTE_DLLIMPORT __dllimport__
#  define __UTL_ATTRIBUTE_TYPE_CPP_DLLIMPORT
#elif UTL_HAS_DECLSPEC(dllimport)
#  define __UTL_ATTRIBUTE_DLLIMPORT dllimport
#  define __UTL_ATTRIBUTE_TYPE_DECLSPEC_DLLIMPORT
#endif

#if UTL_HAS_CPP_ATTRIBUTE(gnu::dllexport)
#  define __UTL_ATTRIBUTE_DLLEXPORT gnu::dllexport
#  define __UTL_ATTRIBUTE_TYPE_CPP_DLLEXPORT
#elif UTL_HAS_GNU_ATTRIBUTE(__dllexport__)
#  define __UTL_ATTRIBUTE_DLLEXPORT __dllexport__
#  define __UTL_ATTRIBUTE_TYPE_CPP_DLLEXPORT
#elif UTL_HAS_DECLSPEC(dllexport)
#  define __UTL_ATTRIBUTE_DLLIMPORT dllexport
#  define __UTL_ATTRIBUTE_TYPE_DECLSPEC_DLLIMPORT
#endif

#if UTL_HAS_CPP_ATTRIBUTE(clang::reinitializes)
#  define __UTL_ATTRIBUTE_REINITIALIZES clang::reinitializes
#  define __UTL_ATTRIBUTE_TYPE_CPP_REINITIALIZES
#elif UTL_HAS_CPP_ATTRIBUTE(gnu::reinitializes)
#  define __UTL_ATTRIBUTE_REINITIALIZES gnu::reinitializes
#  define __UTL_ATTRIBUTE_TYPE_CPP_REINITIALIZES
#elif UTL_HAS_GNU_ATTRIBUTE(__reinitializes__)
#  define __UTL_ATTRIBUTE_REINITIALIZES __reinitializes__
#  define __UTL_ATTRIBUTE_TYPE_GNU_REINITIALIZES
#endif /* UTL_HAS_CPP_ATTRIBUTE(clang::reinitializes) */

#if UTL_HAS_CPP_ATTRIBUTE(maybe_unused)
#  define __UTL_ATTRIBUTE_MAYBE_UNUSED maybe_unused
#  define __UTL_ATTRIBUTE_TYPE_CPP_MAYBE_UNUSED
#elif UTL_HAS_CPP_ATTRIBUTE(gnu::unused)
#  define __UTL_ATTRIBUTE_MAYBE_UNUSED gnu::unused
#  define __UTL_ATTRIBUTE_TYPE_CPP_MAYBE_UNUSED
#elif UTL_HAS_GNU_ATTRIBUTE(__unused__)
#  define __UTL_ATTRIBUTE_MAYBE_UNUSED __unused__
#  define __UTL_ATTRIBUTE_TYPE_GNU_MAYBE_UNUSED
#endif /* UTL_HAS_CPP_ATTRIBUTE(maybe_unused) */

#if UTL_HAS_CPP_ATTRIBUTE(fallthrough)
#  define __UTL_ATTRIBUTE_FALLTHROUGH fallthrough
#  define __UTL_ATTRIBUTE_TYPE_CPP_FALLTHROUGH
#elif UTL_HAS_CPP_ATTRIBUTE(gnu::fallthrough)
#  define __UTL_ATTRIBUTE_FALLTHROUGH gnu::fallthrough
#  define __UTL_ATTRIBUTE_TYPE_CPP_FALLTHROUGH
#elif UTL_HAS_CPP_ATTRIBUTE(gnu::fallthrough)
#  define __UTL_ATTRIBUTE_FALLTHROUGH gnu::fallthrough
#  define __UTL_ATTRIBUTE_TYPE_CPP_FALLTHROUGH
#elif UTL_HAS_GNU_ATTRIBUTE(__fallthrough__)
#  define __UTL_ATTRIBUTE_FALLTHROUGH __fallthrough__
#  define __UTL_ATTRIBUTE_TYPE_GNU_FALLTHROUGH
#endif

#if UTL_HAS_CPP_ATTRIBUTE(clang::noescape)
#  define __UTL_ATTRIBUTE_NOESCAPE clang::noescape
#  define __UTL_ATTRIBUTE_TYPE_CPP_NOESCAPE
#endif

#if UTL_HAS_CPP_ATTRIBUTE(clang::nodebug)
#  define __UTL_ATTRIBUTE_NODEBUG clang::nodebug
#  define __UTL_ATTRIBUTE_TYPE_CPP_NODEBUG
#endif

#if UTL_HAS_CPP_ATTRIBUTE(likely)
#  define __UTL_ATTRIBUTE_LIKELY likely
#  define __UTL_ATTRIBUTE_TYPE_CPP_LIKELY
#elif UTL_HAS_CPP_ATTRIBUTE(clang::likely)
#  define __UTL_ATTRIBUTE_LIKELY clang::likely
#  define __UTL_ATTRIBUTE_TYPE_CPP_LIKELY
#endif /* UTL_HAS_CPP_ATTRIBUTE(likely) */

#if UTL_HAS_CPP_ATTRIBUTE(unlikely)
#  define __UTL_ATTRIBUTE_UNLIKELY unlikely
#  define __UTL_ATTRIBUTE_TYPE_CPP_UNLIKELY
#elif UTL_HAS_CPP_ATTRIBUTE(clang::unlikely)
#  define __UTL_ATTRIBUTE_UNLIKELY clang::unlikely
#  define __UTL_ATTRIBUTE_TYPE_CPP_UNLIKELY
#endif /* UTL_HAS_CPP_ATTRIBUTE(unlikely) */
