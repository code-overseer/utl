// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_assertion.h"
#include "utl/preprocessor/utl_attributes.h"
#include "utl/preprocessor/utl_config.h"
#include "utl/preprocessor/utl_namespace.h"

#ifdef UTL_WITH_EXCEPTIONS
#  include <exception>
#  define UTL_THROW(...) throw(__VA_ARGS__)
#  define UTL_RETHROW throw
#  define UTL_TRY try
#  define UTL_CATCH(...) catch (__VA_ARGS__)
#  define UTL_THROW_IF(CONDITION, ...)       \
      [&](bool UTL_THROW_IF__lambda_param) { \
        if (UTL_THROW_IF__lambda_param) {    \
            UTL_THROW(__VA_ARGS__);          \
        }                                    \
      }(CONDITION)

UTL_NAMESPACE_BEGIN
UTL_INLINE_CXX17 constexpr bool with_exceptions = true;
using std::exception;
using std::exception_ptr;
#  ifdef UTL_CXX17
using std::uncaught_exceptions;
#  else
using std::uncaught_exception;
#  endif
using std::current_exception;
using std::get_terminate;
using std::make_exception_ptr;
using std::rethrow_exception;
using std::set_terminate;
using std::terminate;
UTL_NAMESPACE_END

#else // UTL_WITH_EXCEPTIONS
UTL_NAMESPACE_BEGIN
class exception {
public:
    virtual char const* what() const noexcept { return nullptr; }
};
UTL_INLINE_CXX17 constexpr bool with_exceptions = false;
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

#  define UTL_THROW_IF(CONDITION, ...)                                                     \
      UTL_ASSERT(!(CONDITION) && UTL_SCOPE details::exception::always_false(__VA_ARGS__)); \
      UTL_BUILTIN_unreachable()

#  define UTL_CATCH(...) else if (UTL_SCOPE always_false([](__VA_ARGS__) -> void {}))

#endif // UTL_WITH_EXCEPTIONS
