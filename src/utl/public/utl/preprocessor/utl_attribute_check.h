/* Copyright 2023-2024 Bryan Wong */

#pragma once

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
