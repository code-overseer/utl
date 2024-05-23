/* Copyright 2023-2024 Bryan Wong */

#include "utl/preprocessor/utl_compiler.h"
#include "utl/preprocessor/utl_target.h"

#if UTL_COMPILER_MSVC && !UTL_COMPILER_MSVC_AT_LEAST(1925)
#  define UTL_PRAGMA(ARG) __pragma(ARG)
#else
#  define UTL_PRAGMA(ARG) _Pragma(#ARG)
#endif

#if UTL_TARGET_APPLE
/* Apple supposedly silently ignores the region token */
#  define UTL_SUPPORTS_PRAGMA_REGION
#  define UTL_BEGIN_REGION(LABEL) UTL_PRAGMA(region LABEL)
#  define UTL_END_REGION(LABEL) UTL_PRAGMA(endregion LABEL)
#elif UTL_COMPILER_CLANG_AT_LEAST(6, 0, 0)
#  define UTL_SUPPORTS_PRAGMA_REGION
#  define UTL_BEGIN_REGION(LABEL) UTL_PRAGMA(region LABEL)
#  define UTL_END_REGION(LABEL) UTL_PRAGMA(endregion LABEL)
#elif UTL_COMPILER_GCC_AT_LEAST(13, 0, 0)
#  define UTL_SUPPORTS_PRAGMA_REGION
#  define UTL_BEGIN_REGION(LABEL) UTL_PRAGMA(region LABEL)
#  define UTL_END_REGION(LABEL) UTL_PRAGMA(endregion LABEL)
#elif defined(UTL_COMPILER_MSVC)
#  define UTL_SUPPORTS_PRAGMA_REGION
#  define UTL_BEGIN_REGION(LABEL) UTL_PRAGMA(region LABEL)
#  define UTL_END_REGION(LABEL) UTL_PRAGMA(endregion LABEL)
#else
#  define UTL_BEGIN_REGION(_)
#  define UTL_END_REGION(_)
#endif

#if UTL_COMPILER_CLANG || UTL_COMPILER_ICX

#  define UTL_PRAGMA_WARN(x) UTL_PRAGMA(message #x)

#elif UTL_COMPILER_GCC

#  if UTL_COMPILER_GCC_AT_LEAST(9, 1, 0)
#    define UTL_PRAGMA_WARN(x) UTL_PRAGMA(GCC warning #x)
#  else /* UTL_COMPILER_GCC_AT_LEAST(9,1,0) */
#    define UTL_PRAGMA_WARN(x) UTL_PRAGMA(GCC message #x)
#  endif

#elif UTL_COMPILER_MSVC

#  define UTL_WARN_PRIVATE_STR2(x) #x
#  define UTL_WARN_PRIVATE_STR1(x) UTL_WARN_PRIVATE_STR2(x)
#  define UTL_WARN_PRIVATE_LOCATION __FILE__ ":" UTL_WARN_PRIVATE_STR1(__LINE__) ":"
#  define UTL_PRAGMA_WARN(x) UTL_PRAGMA(message(UTL_WARN_PRIVATE_LOCATION " warning: " #x))

#else

#  define UTL_PRAGMA_WARN(_)

#endif
/* TODO: Find out ICC's warning pragma */
