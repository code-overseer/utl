// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_namespace.h"
#include "utl/preprocessor/utl_standard.h"

UTL_NAMESPACE_BEGIN

enum class standard : long {
    cpp11 = 201103L,
    cpp14 = 201402L,
    cpp17 = 201703L,
    cpp20 = 202002L,
    cpp23 = 202302L,
    #ifdef UTL_CXX23
    current = cpp23
    #elif defined(UTL_CXX20)    // ifdef UTL_CXX23
    current = cpp20
    #elif defined(UTL_CXX17)    // ifdef UTL_CXX23
    current = cpp17
    #elif defined(UTL_CXX14)    // ifdef UTL_CXX23
    current = cpp14
    #else   // ifdef UTL_CXX23
    current = cpp11
    #endif  // ifdef UTL_CXX23
};

UTL_NAMESPACE_END
