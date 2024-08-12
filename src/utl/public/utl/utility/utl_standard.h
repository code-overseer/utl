// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/configuration/utl_namespace.h"
#include "utl/configuration/utl_standard.h"

UTL_NAMESPACE_BEGIN

enum class standard : long {
    cpp11 = 201103L,
    cpp14 = 201402L,
    cpp17 = 201703L,
    cpp20 = 202002L,
    cpp23 = 202302L,
#if UTL_CXX23
    current = cpp23
#elif UTL_CXX20 // if UTL_CXX23
    current = cpp20
#elif UTL_CXX17 // if UTL_CXX23
    current = cpp17
#elif UTL_CXX14 // if UTL_CXX23
    current = cpp14
#else           // if UTL_CXX23
    current = cpp11
#endif          // if UTL_CXX23
};

UTL_NAMESPACE_END
