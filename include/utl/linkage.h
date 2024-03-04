// Copyright 2023-2024 Bryan Wong

#pragma once

/** Probably don't need this **/

#ifndef UTL_USE_DLLEXPORT

#  if defined(_WIN32) || defined(__CYGWIN__)
#    define UTL_USE_DLLEXPORT
#  endif
#endif

#ifdef UTL_EXPORT_LIB

#  ifdef UTL_USE_DLLEXPORT

#    ifdef __GNUC__
#      define UTL_API __attribute__((dllexport))
#    else // defined(__GNUC__)
#      define UTL_API __declspec(dllexport)
#    endif // defined(__GNUC__)

#  else // defined(UTL_USE_DLLEXPORT)

#    if __GNUC__ >= 4
#      define UTL_API __attribute__((visibility("default")))
#    else // __GNUC__ >= 4
#      define UTL_API
#    endif // __GNUC__ >= 4

#  endif // defined(UTL_USE_DLLEXPORT)

#else // UTL_EXPORT_LIB

#  ifdef UTL_USE_DLLEXPORT

#    if defined(__GNUC__)
#      define UTL_API __attribute__((dllimport))
#    else // defined(__GNUC__)
#      define UTL_API __declspec(dllimport)
#    endif // defined(__GNUC__)

#  else // UTL_USE_DLLEXPORT

#    if __GNUC__ >= 4
#      define UTL_API __attribute__((visibility("default")))
#    else // __GNUC__ >= 4
#      define UTL_API
#    endif // __GNUC__ >= 4

#  endif // UTL_USE_DLLEXPORT

#endif // UTL_EXPORT_LIB
