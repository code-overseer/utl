/* Copyright 2023-2024 Bryan Wong */

#pragma once

#include "utl/preprocessor/utl_paste.h"

/**
 * By Jens Gustedt
 * https://gustedt.wordpress.com/2010/06/08/detect-empty-macro-arguments/
 */
#define UTL_IE_TRIGGER_PARENTHESIS() ,

/* increase if needed */
#define UTL_IE_ARG16(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, ...) _15
#define UTL_IE_HAS_COMMA(...) \
    UTL_PASTE(UTL_IE_ARG16(__VA_ARGS__, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0))

#define UTL_IE_CONCAT5(_0, _1, _2, _3, _4) _0##_1##_2##_3##_4
#define UTL_IE_EMPTY_CASE_0001 ,
/* clang-format off */
#define UTL_IS_EMPTY_(_0, _1, _2, _3) UTL_IE_HAS_COMMA(UTL_IE_CONCAT5(UTL_IE_EMPTY_CASE_, _0, _1, _2, _3))
#define UTL_IS_EMPTY(...)                                                        \
UTL_IS_EMPTY_(                                                                   \
          /* test if there is just one argument, eventually an empty             \
             one */                                                              \
          UTL_PASTE(UTL_IE_HAS_COMMA(__VA_ARGS__)),                              \
          /* test if _TRIGGER_PARENTHESIS_ together with the argument            \
             adds a comma */                                                     \
          UTL_PASTE(UTL_IE_HAS_COMMA(UTL_IE_TRIGGER_PARENTHESIS __VA_ARGS__)),   \
          /* test if the argument together with a parenthesis                    \
             adds a comma */                                                     \
          UTL_PASTE(UTL_IE_HAS_COMMA(__VA_ARGS__ ())),                           \
          /* test if placing it between UTL_TRIGGER_PARENTHESIS and the          \
             parenthesis adds a comma */                                         \
          UTL_PASTE(UTL_IE_HAS_COMMA(UTL_IE_TRIGGER_PARENTHESIS __VA_ARGS__ ())) \
          )
/* clang-format on */

#define UTL_C89_STATIC_ASSERT_3(COND, MSG) typedef char static_assertion[(!!(COND)) * 2 - 1]
#define UTL_C89_STATIC_ASSERT_2(X, L) UTL_C89_STATIC_ASSERT_3(X, LINE##L)
#define UTL_C89_STATIC_ASSERT_1(X, L) UTL_C89_STATIC_ASSERT_2(X, L)
#define UTL_C89_STATIC_ASSERT(X) UTL_C89_STATIC_ASSERT_1(X, __LINE__)
UTL_C89_STATIC_ASSERT(UTL_IS_EMPTY());
UTL_C89_STATIC_ASSERT(!UTL_IS_EMPTY(XYZ));
UTL_C89_STATIC_ASSERT(!UTL_IS_EMPTY(A, B, C));

/**
 * Appends the result of IS_EMPTY to a prefix, used to resolve macro overloads
 *
 * If the number of variadic arguments is 0, the prefix will be appended with 1 (i.e. result of
 * IS_EMPTY) If the number of variadic arguments is >0, the prefix will be appended with 0 (i.e.
 * result of IS_EMPTY)
 */
#define UTL_APPEND_IS_EMPTY(PREFIX, ...) \
    UTL_PASTE(UTL_CONCAT(PREFIX, UTL_PASTE(UTL_IS_EMPTY(__VA_ARGS__))))
