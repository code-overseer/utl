/* Copyright 2023-2024 Bryan Wong */

#pragma once

#include "utl/preprocessor/utl_apply_macro.h"
#include "utl/preprocessor/utl_compiler.h"
#include "utl/preprocessor/utl_concatenation.h"
#include "utl/preprocessor/utl_declspec.h"
#include "utl/preprocessor/utl_is_empty.h"
#include "utl/preprocessor/utl_paste.h"

#define __UTL_CPP_ATTRIBUTE_SCOPE_1()
#define __UTL_CPP_ATTRIBUTE_SCOPE_0(...) [[__VA_ARGS__]]
#define __UTL_CPP_ATTRIBUTE_SCOPE(...) \
    UTL_PASTE(UTL_APPEND_IS_EMPTY(__UTL_CPP_ATTRIBUTE_SCOPE_, __VA_ARGS__)(__VA_ARGS__))

#define __UTL_GNU_ATTRIBUTE_SCOPE_1()
#define __UTL_GNU_ATTRIBUTE_SCOPE_0(...) __attribute__((__VA_ARGS__))
#define __UTL_GNU_ATTRIBUTE_SCOPE(...) \
    UTL_PASTE(UTL_APPEND_IS_EMPTY(__UTL_GNU_ATTRIBUTE_SCOPE_, __VA_ARGS__)(__VA_ARGS__))

#define __UTL_DECLSPEC_SCOPE_1()
#define __UTL_DECLSPEC_SCOPE_0(...) __declspec(__VA_ARGS__)
#define __UTL_DECLSPEC_SCOPE(...) \
    UTL_PASTE(UTL_APPEND_IS_EMPTY(__UTL_DECLSPEC_SCOPE_, __VA_ARGS__)(__VA_ARGS__))

#define __UTL_TRANSLATE_ATTRIBUTE(ARGS) UTL_SWAP_CONCAT(_END, __UTL_TRANSLATE_ATTRIBUTE_1 ARGS)
#define __UTL_TRANSLATE_ATTRIBUTE_0(NAME) (UTL_CONCAT(__UTL_ATTRIBUTE_, NAME))
#if UTL_MSVC_PREPROCESSOR
#  define __UTL_TRANSLATE_ATTRIBUTE_1(NAME) \
      UTL_PASTE(__UTL_TRANSLATE_ATTRIBUTE_0(NAME) __UTL_TRANSLATE_ATTRIBUTE_2)
#  define __UTL_TRANSLATE_ATTRIBUTE_2(NAME) \
      UTL_PASTE(__UTL_TRANSLATE_ATTRIBUTE_0(NAME) __UTL_TRANSLATE_ATTRIBUTE_1)
#else
#  define __UTL_TRANSLATE_ATTRIBUTE_1(NAME) \
      __UTL_TRANSLATE_ATTRIBUTE_0(NAME) __UTL_TRANSLATE_ATTRIBUTE_2
#  define __UTL_TRANSLATE_ATTRIBUTE_2(NAME) \
      __UTL_TRANSLATE_ATTRIBUTE_0(NAME) __UTL_TRANSLATE_ATTRIBUTE_1
#endif
#define __UTL_TRANSLATE_ATTRIBUTE_1_END
#define __UTL_TRANSLATE_ATTRIBUTE_2_END

#define __UTL_BUILD_ATTRIBUTE_LIST(ARGS) UTL_SWAP_CONCAT(_END, __UTL_BUILD_ATTRIBUTE_LIST_1 ARGS)
#define __UTL_BEGIN_ATTRIBUTE_LIST(NAME) NAME
#define __UTL_APPEND_ATTRIBUTE(NAME) , NAME
#if UTL_MSVC_PREPROCESSOR
#  define __UTL_BUILD_ATTRIBUTE_LIST_1(MEMBER) \
      UTL_PASTE(__UTL_BEGIN_ATTRIBUTE_LIST(MEMBER) __UTL_BUILD_ATTRIBUTE_LIST_2)
#  define __UTL_BUILD_ATTRIBUTE_LIST_2(MEMBER) \
      UTL_PASTE(__UTL_APPEND_ATTRIBUTE(MEMBER) __UTL_BUILD_ATTRIBUTE_LIST_3)
#  define __UTL_BUILD_ATTRIBUTE_LIST_3(MEMBER) \
      UTL_PASTE(__UTL_APPEND_ATTRIBUTE(MEMBER) __UTL_BUILD_ATTRIBUTE_LIST_2)
#else
#  define __UTL_BUILD_ATTRIBUTE_LIST_1(MEMBER) \
      __UTL_BEGIN_ATTRIBUTE_LIST(MEMBER) __UTL_BUILD_ATTRIBUTE_LIST_2
#  define __UTL_BUILD_ATTRIBUTE_LIST_2(MEMBER) \
      __UTL_APPEND_ATTRIBUTE(MEMBER) __UTL_BUILD_ATTRIBUTE_LIST_3
#  define __UTL_BUILD_ATTRIBUTE_LIST_3(MEMBER) \
      __UTL_APPEND_ATTRIBUTE(MEMBER) __UTL_BUILD_ATTRIBUTE_LIST_2
#endif
#define __UTL_BUILD_ATTRIBUTE_LIST_1_END
#define __UTL_BUILD_ATTRIBUTE_LIST_2_END
#define __UTL_BUILD_ATTRIBUTE_LIST_3_END

#define __UTL_ATTRIBUTE_FILTER_1(NAME) (NAME)
#define __UTL_ATTRIBUTE_FILTER_0(X)
#define __UTL_ATTRIBUTE_FILTER(NAME, ...) \
    UTL_PASTE(UTL_APPEND_IS_EMPTY(__UTL_ATTRIBUTE_FILTER_, __VA_ARGS__)(NAME))

#define __UTL_FILTER_CPP_ATTRIBUTES(ARGS) UTL_SWAP_CONCAT(_END, __UTL_FILTER_CPP_ATTRIBUTES_1 ARGS)
#define __UTL_FILTER_CPP_ATTRIBUTES_1(NAME)                                   \
    __UTL_ATTRIBUTE_FILTER(NAME, UTL_CONCAT(__UTL_ATTRIBUTE_TYPE_CPP_, NAME)) \
    __UTL_FILTER_CPP_ATTRIBUTES_2
#define __UTL_FILTER_CPP_ATTRIBUTES_2(NAME)                                   \
    __UTL_ATTRIBUTE_FILTER(NAME, UTL_CONCAT(__UTL_ATTRIBUTE_TYPE_CPP_, NAME)) \
    __UTL_FILTER_CPP_ATTRIBUTES_1
#define __UTL_FILTER_CPP_ATTRIBUTES_1_END
#define __UTL_FILTER_CPP_ATTRIBUTES_2_END

#define __UTL_CPP_ATTRIBUTES(LIST) \
    __UTL_CPP_ATTRIBUTE_SCOPE(     \
        __UTL_BUILD_ATTRIBUTE_LIST(__UTL_TRANSLATE_ATTRIBUTE(__UTL_FILTER_CPP_ATTRIBUTES(LIST))))

#define __UTL_FILTER_GNU_ATTRIBUTES(ARGS) UTL_SWAP_CONCAT(_END, __UTL_FILTER_GNU_ATTRIBUTES_1 ARGS)
#define __UTL_FILTER_GNU_ATTRIBUTES_1(NAME)                                   \
    __UTL_ATTRIBUTE_FILTER(NAME, UTL_CONCAT(__UTL_ATTRIBUTE_TYPE_GNU_, NAME)) \
    __UTL_FILTER_GNU_ATTRIBUTES_2
#define __UTL_FILTER_GNU_ATTRIBUTES_2(NAME)                                   \
    __UTL_ATTRIBUTE_FILTER(NAME, UTL_CONCAT(__UTL_ATTRIBUTE_TYPE_GNU_, NAME)) \
    __UTL_FILTER_GNU_ATTRIBUTES_1
#define __UTL_FILTER_GNU_ATTRIBUTES_1_END
#define __UTL_FILTER_GNU_ATTRIBUTES_2_END

#define __UTL_GNU_ATTRIBUTES(LIST) \
    __UTL_GNU_ATTRIBUTE_SCOPE(     \
        __UTL_BUILD_ATTRIBUTE_LIST(__UTL_TRANSLATE_ATTRIBUTE(__UTL_FILTER_GNU_ATTRIBUTES(LIST))))

#if UTL_COMPILER_SUPPORTS_DECLSPEC
#  define __UTL_FILTER_DECLSPEC_ATTRIBUTES(ARGS) \
      UTL_SWAP_CONCAT(_END, __UTL_FILTER_DECLSPEC_ATTRIBUTES_1 ARGS)
#  define __UTL_FILTER_DECLSPEC_ATTRIBUTES_1(NAME)                                   \
      __UTL_ATTRIBUTE_FILTER(NAME, UTL_CONCAT(__UTL_ATTRIBUTE_TYPE_DECLSPEC_, NAME)) \
      __UTL_FILTER_DECLSPEC_ATTRIBUTES_2
#  define __UTL_FILTER_DECLSPEC_ATTRIBUTES_2(NAME)                                   \
      __UTL_ATTRIBUTE_FILTER(NAME, UTL_CONCAT(__UTL_ATTRIBUTE_TYPE_DECLSPEC_, NAME)) \
      __UTL_FILTER_DECLSPEC_ATTRIBUTES_1
#  define __UTL_FILTER_DECLSPEC_ATTRIBUTES_1_END
#  define __UTL_FILTER_DECLSPEC_ATTRIBUTES_2_END

#  define __UTL_DECLSPEC(LIST)                         \
      __UTL_DECLSPEC_SCOPE(__UTL_BUILD_ATTRIBUTE_LIST( \
          __UTL_TRANSLATE_ATTRIBUTE(__UTL_FILTER_DECLSPEC_ATTRIBUTES(LIST))))
#else
#  define __UTL_DECLSPEC(LIST)
#endif

#if UTL_COMPILER_MSVC

#  define __UTL_TRANSLATE_MSVC_EXT_ATTRIBUTE(ARGS) \
      UTL_SWAP_CONCAT(_END, __UTL_TRANSLATE_MSVC_EXT_ATTRIBUTE_1 ARGS)
#  define __UTL_TRANSLATE_MSVC_EXT_ATTRIBUTE_0(NAME) UTL_CONCAT(__UTL_ATTRIBUTE_, NAME)
#  if UTL_MSVC_PREPROCESSOR
#    define __UTL_TRANSLATE_MSVC_EXT_ATTRIBUTE_1(NAME) \
        UTL_PASTE(__UTL_TRANSLATE_MSVC_EXT_ATTRIBUTE_0(NAME) __UTL_TRANSLATE_MSVC_EXT_ATTRIBUTE_2)
#    define __UTL_TRANSLATE_MSVC_EXT_ATTRIBUTE_2(NAME) \
        UTL_PASTE(__UTL_TRANSLATE_MSVC_EXT_ATTRIBUTE_0(NAME) __UTL_TRANSLATE_MSVC_EXT_ATTRIBUTE_1)
#  else
#    define __UTL_TRANSLATE_MSVC_EXT_ATTRIBUTE_1(NAME) \
        __UTL_TRANSLATE_MSVC_EXT_ATTRIBUTE_0(NAME) __UTL_TRANSLATE_MSVC_EXT_ATTRIBUTE_2
#    define __UTL_TRANSLATE_MSVC_EXT_ATTRIBUTE_2(NAME) \
        __UTL_TRANSLATE_MSVC_EXT_ATTRIBUTE_0(NAME) __UTL_TRANSLATE_MSVC_EXT_ATTRIBUTE_1
#  endif
#  define __UTL_TRANSLATE_MSVC_EXT_ATTRIBUTE_1_END
#  define __UTL_TRANSLATE_MSVC_EXT_ATTRIBUTE_2_END

#  define __UTL_FILTER_MSVC_EXT_ATTRIBUTES(ARGS) \
      UTL_SWAP_CONCAT(_END, __UTL_FILTER_MSVC_EXT_ATTRIBUTES_1 ARGS)
#  define __UTL_FILTER_MSVC_EXT_ATTRIBUTES_1(NAME)                                   \
      __UTL_ATTRIBUTE_FILTER(NAME, UTL_CONCAT(__UTL_ATTRIBUTE_TYPE_MSVC_EXT_, NAME)) \
      __UTL_FILTER_MSVC_EXT_ATTRIBUTES_2
#  define __UTL_FILTER_MSVC_EXT_ATTRIBUTES_2(NAME)                                   \
      __UTL_ATTRIBUTE_FILTER(NAME, UTL_CONCAT(__UTL_ATTRIBUTE_TYPE_MSVC_EXT_, NAME)) \
      __UTL_FILTER_MSVC_EXT_ATTRIBUTES_1
#  define __UTL_FILTER_MSVC_EXT_ATTRIBUTES_1_END
#  define __UTL_FILTER_MSVC_EXT_ATTRIBUTES_2_END
#  define __UTL_MSVC_EXT_ATTRIBUTES(LIST) \
      __UTL_TRANSLATE_MSVC_EXT_ATTRIBUTE(__UTL_FILTER_MSVC_EXT_ATTRIBUTES(LIST))
#else
#  define __UTL_MSVC_EXT_ATTRIBUTES(LIST)
#endif

#define __UTL_ATTRIBUTES(LIST) \
    __UTL_CPP_ATTRIBUTES(LIST) \
    __UTL_GNU_ATTRIBUTES(LIST) __UTL_DECLSPEC(LIST) __UTL_MSVC_EXT_ATTRIBUTES(LIST)

#define __UTL_PARENTHESIZE(_0) (_0)
#define UTL_ATTRIBUTES(...) \
    __UTL_ATTRIBUTES(UTL_PASTE(UTL_APPLY_MACRO(__UTL_PARENTHESIZE, __VA_ARGS__)))
#define UTL_ATTRIBUTE(ATTR) __UTL_ATTRIBUTES((ATTR))

#define UTL_CPP_ATTRIBUTES(...) \
    __UTL_CPP_ATTRIBUTES(UTL_PASTE(UTL_APPLY_MACRO(__UTL_PARENTHESIZE, __VA_ARGS__)))
#define UTL_CPP_ATTRIBUTE(ATTR) __UTL_CPP_ATTRIBUTES((ATTR))

#define UTL_GNU_ATTRIBUTES(...) \
    __UTL_GNU_ATTRIBUTES(UTL_PASTE(UTL_APPLY_MACRO(__UTL_PARENTHESIZE, __VA_ARGS__)))
#define UTL_GNU_ATTRIBUTE(ATTR) __UTL_GNU_ATTRIBUTES((ATTR))

#define UTL_DECLSPEC_ATTRIBUTES(...) \
    __UTL_DECLSPEC(UTL_PASTE(UTL_APPLY_MACRO(__UTL_PARENTHESIZE, __VA_ARGS__)))
#define UTL_DECLSPEC_ATTRIBUTE(ATTR) __UTL_DECLSPEC((ATTR))
