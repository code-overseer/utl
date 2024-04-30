// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_config.h"

#if UTL_HAS_BUILTIN(__builtin_is_constant_evaluated)

#  define UTL_CONSTANT_P(...) __builtin_is_constant_evaluated()

#elif defined(__builtin_constant_p)
#  define UTL_CONSTANT_P(...) __builtin_constant_p(__VA_ARGS__)

#elif defined(UTL_COMPILER_MSVC)
UTL_NAMESPACE_BEGIN
namespace details {
UTL_ATTRIBUTES(NODISCARD, CONST, ALWAYS_INLINE)
constexpr bool is_constant_evaluated() noexcept {
    struct C {};
    struct M : C {
        int a;
    };
    struct N : C {
        int a;
    };
    return &M::a != static_cast<int C::*>(&N::a);
}
} // namespace details
UTL_NAMESPACE_END

#  define UTL_CONSTANT_P(...) UTL_SCOPE details::is_constant_evaluated()

#else

#  error Unsupported compiler

#endif
