// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/base_preprocessor.h"
#include "utl/preprocessor/utl_assertion.h"
#include "utl/preprocessor/utl_attributes.h"
#include "utl/preprocessor/utl_namespace.h"

#ifdef UTL_WITH_EXCEPTIONS
#  define UTL_THROW(...) throw(__VA_ARGS__)
#  define UTL_RETHROW throw
#  define UTL_TRY try
#  define UTL_CATCH(...) catch (__VA_ARGS__)
#else // UTL_WITH_EXCEPTIONS

UTL_NAMESPACE_BEGIN

namespace details {
namespace exception {

template <typename... A>
UTL_ATTRIBUTES(NODISCARD, CONST)
constexpr bool always_false(A&&...) noexcept {
    return false;
}
template <typename... A>
UTL_ATTRIBUTES(NODISCARD, CONST)
constexpr bool always_true(A&&...) noexcept {
    return true;
}
} // namespace exception
} // namespace details

UTL_NAMESPACE_END

#  define UTL_THROW(...)                                                   \
      UTL_ASSERT(UTL_SCOPE details::exception::always_false(__VA_ARGS__)); \
      UTL_BUILTIN_unreachable()
#  define UTL_RETHROW    \
      UTL_ASSERT(false); \
      UTL_BUILTIN_unreachable()
#  define UTL_TRY if UTL_CONSTEXPR_CXX17 (1)

#  define UTL_CATCH(...) else if (UTL_SCOPE always_false([](__VA_ARGS__) -> void {}))

#endif // UTL_WITH_EXCEPTIONS
