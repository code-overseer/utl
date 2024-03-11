/* Copyright 2023-2024 Bryan Wong */

#pragma once

#include "utl/preprocessor/utl_is_empty.h"
#include "utl/preprocessor/utl_paste.h"

/**
 * TODO Use VA_OPT in C++20
 */

#define UTL_EXPAND_VA_ARGS_1()
#define UTL_EXPAND_VA_ARGS_0(...) , __VA_ARGS__
#define UTL_EXPAND_VA_ARGS(...) \
    UTL_PASTE(UTL_APPEND_IS_EMPTY(UTL_EXPAND_VA_ARGS_, __VA_ARGS__)(__VA_ARGS__))
