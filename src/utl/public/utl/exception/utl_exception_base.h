// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_config.h"

#include "utl/preprocessor/utl_assertion.h"

#include "utl/type_traits/utl_constants.h"

#include <exception>

#if UTL_WITH_EXCEPTIONS

#  define UTL_THROW(...) throw(__VA_ARGS__)
#  define UTL_RETHROW(...) throw
#  define UTL_TRY try
#  define UTL_CATCH(...) catch (__VA_ARGS__)

#  if UTL_CXX17
#    define UTL_THROW_IF(CONDITION, ...)       \
        [&](bool UTL_THROW_IF__lambda_param) { \
        if (UTL_THROW_IF__lambda_param) {      \
            UTL_THROW(__VA_ARGS__);            \
        }                                      \
        }(CONDITION)
#  else
#    define UTL_THROW_IF(CONDITION, ...) ((CONDITION) ? UTL_THROW(__VA_ARGS__) : (void)0)
#  endif

UTL_NAMESPACE_BEGIN
UTL_INLINE_CXX17 constexpr bool with_exceptions = true;

#  define UTL_THROWS
#  define UTL_NOEXCEPT(...) noexcept(__VA_ARGS__)

#  if UTL_THROWABLE_FUNCTION_NOEXCEPT
#    error `UTL_THROWABLE_FUNCTION_NOEXCEPT` cannot be set when exceptions are enabled
#  endif

using std::exception;
using std::exception_ptr;
#  if UTL_CXX17
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

using std::exception;
UTL_INLINE_CXX17 constexpr bool with_exceptions = false;

namespace details {
namespace exception {
template <typename F>
UTL_ATTRIBUTES(NODISCARD) constexpr bool catch_statement(F&&) noexcept {
    return false;
}
} // namespace exception
} // namespace details

UTL_NAMESPACE_END

#  if UTL_THROWABLE_FUNCTION_NOEXCEPT
#    define UTL_THROWS noexcept
#    define UTL_NOEXCEPT(...) noexcept
#  else
#    define UTL_THROWS
#    define UTL_NOEXCEPT(...) noexcept(__VA_ARGS__)
#  endif

#  define UTL_THROW(...)                                           \
      UTL_ASSERT(UTL_SCOPE always_false<decltype(__VA_ARGS__)>()); \
      UTL_BUILTIN_unreachable()
#  define UTL_RETHROW(...)            \
      UTL_ASSERT(false, __VA_ARGS__); \
      UTL_BUILTIN_unreachable()
#  define UTL_TRY if UTL_CONSTEXPR_CXX17 (1)

#  define UTL_THROW_IF(CONDITION, ...)                                             \
      UTL_ASSERT(!(CONDITION) && UTL_SCOPE always_false<decltype(__VA_ARGS__)>()); \
      UTL_BUILTIN_unreachable()

#  define UTL_CATCH(...)                                                          \
      else if UTL_CONSTEXPR_CXX17 (UTL_SCOPE details::exception::catch_statement( \
                                       [](__VA_ARGS__) -> void {}))

#endif // UTL_WITH_EXCEPTIONS
