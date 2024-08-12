// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_config.h"

#ifdef UTL_BUILTIN_is_constant_evaluated

#  define UTL_CONSTANT_P(...) UTL_BUILTIN_is_constant_evaluated()

#elif UTL_HAS_BUILTIN(__builtin_constant_p)
#  define UTL_CONSTANT_P(...) __builtin_constant_p((__VA_ARGS__))

#elif UTL_COMPILER_MSVC
UTL_NAMESPACE_BEGIN
namespace details {
UTL_ATTRIBUTES(NODISCARD, CONST, _HIDE_FROM_ABI) constexpr bool is_constant_evaluated() noexcept {
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

#  define UTL_CONSTANT_P(...) static_assert(false, "Unsupported feature")

#endif
