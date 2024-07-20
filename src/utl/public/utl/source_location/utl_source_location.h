// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_config.h"

#if UTL_COMPILER_CLANG_AT_LEAST(16, 0, 0) | UTL_COMPILER_GCC_AT_LEAST(13, 1, 0) | \
    UTL_COMPILER_MSVC_AT_LEAST(1937) | UTL_COMPILER_ICX_AT_LEAST(20230200)
#  define UTL_D129488_FIXED 1
#else
// https://reviews.llvm.org/D129488
#  define UTL_D129488_FIXED 0
#endif

#if (UTL_D129488_FIXED | UTL_USE_STD_SOURCE_LOCATION) && defined(UTL_BUILTIN_source_location) && \
    UTL_CXX20

#  define UTL_COMPILER_SUPPORTS_SOURCE_LOCATION 1
#  include <source_location>

UTL_NAMESPACE_BEGIN
using ::std::source_location;
UTL_NAMESPACE_END

#  define UTL_SOURCE_LOCATION() UTL_SCOPE source_location::current()

#elif defined(UTL_BUILTIN_LINE) & defined(UTL_BUILTIN_FILE) & defined(UTL_BUILTIN_FUNCTION)

#  define UTL_COMPILER_SUPPORTS_SOURCE_LOCATION 1

#  if UTL_CXX20 & UTL_D129488_FIXED
#    define UTL_SOURCE_LOCATION_CONSTEVAL consteval
#  else
#    define UTL_SOURCE_LOCATION_CONSTEVAL constexpr
#  endif

#  ifdef UTL_BUILTIN_FUNCSIG
#    define UTL_SL_LOCATION_FUNCTION() UTL_BUILTIN_FUNCSIG()
#  else
#    define UTL_SL_LOCATION_FUNCTION() UTL_BUILTIN_FUNCTION()
#  endif

#  ifdef UTL_BUILTIN_COLUMN
#    define UTL_SL_LOCATION_COLUMN() UTL_BUILTIN_COLUMN()
#  else
#    define UTL_SL_LOCATION_COLUMN() -1
#    include "utl/type_traits/utl_constants.h"
#  endif

UTL_NAMESPACE_BEGIN

class source_location {
public:
    static UTL_SOURCE_LOCATION_CONSTEVAL source_location current(
        char const* file_name = UTL_BUILTIN_FILE(),
        char const* function_name = UTL_SL_LOCATION_FUNCTION(), unsigned line = UTL_BUILTIN_LINE(),
        unsigned column = UTL_SL_LOCATION_COLUMN()) noexcept {
        source_location location;
        location.file_name_ = file_name;
        location.function_name_ = function_name;
        location.line_ = line;
        location.column_ = column;
        return location;
    }

    constexpr source_location() noexcept = default;

    UTL_ATTRIBUTES(NODISCARD, PURE)
    constexpr char const* file_name() const noexcept { return file_name_; }

    UTL_ATTRIBUTES(NODISCARD, PURE)
    constexpr char const* function_name() const noexcept { return function_name_; }

    UTL_ATTRIBUTES(NODISCARD, PURE)
    constexpr unsigned line() const noexcept { return line_; }

#  ifdef UTL_BUILTIN_COLUMN
    UTL_ATTRIBUTES(NODISCARD, PURE)
    constexpr unsigned column() const noexcept { return column_; }
#  else
    template <typename T = int>
    constexpr unsigned column() const noexcept {
#    if UTL_COMPILER_GCC
        static_assert(
            always_false<T>(), "This function is currently unimplemented, please update to C++20");
#    else
        static_assert(always_false<T>(),
            "This function is currently unimplemented, please update the compiler or update to "
            "C++20");
#    endif
        return column_;
    }
#  endif

private:
    static constexpr char const empty[] = "";
    char const* file_name_ = empty;
    char const* function_name_ = empty;
    unsigned line_ = 0;
    unsigned column_ = 0;
};

UTL_NAMESPACE_END
static_assert(sizeof(UTL_SCOPE source_location) > 0);

#  undef UTL_SL_LOCATION_FUNCTION
#  undef UTL_SL_LOCATION_COLUMN
#  define UTL_SOURCE_LOCATION() UTL_SCOPE source_location::current()

#else // UTL_USE_STD_SOURCE_LOCATION && defined(UTL_BUILTIN_source_location) && UTL_CXX20

#  include "utl/type_traits/utl_constants.h"

UTL_NAMESPACE_BEGIN

class source_location {
public:
    template <typename T = int>
    static source_location current() noexcept {
        static_assert(always_false<T>(),
            "This function is currently unimplemented, please update the compiler or update to "
            "C++20");
        return source_location{};
    }

    static constexpr source_location UTL_current(
        char const* file_name, char const* function_name, unsigned line) noexcept {
        source_location location;
        location.file_name_ = file_name;
        location.function_name_ = function_name;
        location.line_ = line;
        location.column_ = -1;
        return location;
    }

    constexpr source_location() noexcept = default;

    UTL_ATTRIBUTES(NODISCARD, PURE)
    constexpr char const* file_name() const noexcept { return file_name_; }

    UTL_ATTRIBUTES(NODISCARD, PURE)
    constexpr char const* function_name() const noexcept { return function_name_; }

    UTL_ATTRIBUTES(NODISCARD, PURE)
    constexpr unsigned line() const noexcept { return line_; }

    template <typename T = int>
    constexpr unsigned column() const noexcept {
#  if UTL_COMPILER_GCC
        static_assert(
            always_false<T>(), "This function is currently unimplemented, please update to C++20");
#  else
        static_assert(always_false<T>(),
            "This function is currently unimplemented, please update the compiler or update to "
            "C++20");
#  endif
        return column_;
    }

private:
    static constexpr char const empty[] = "";
    char const* file_name_ = empty;
    char const* function_name_ = empty;
    unsigned line_ = 0;
    unsigned column_ = 0;
};

UTL_NAMESPACE_END

#  if UTL_COMPILER_MSVC
#    define UTL_SOURCE_LOCATION() \
        UTL_SCOPE source_location::UTL_current(__FILE__, __FUNCSIG__, __LINE__)
#  elif UTL_COMPILER_GCC | UTL_COMPILER_CLANG | UTL_COMPILER_ICX | UTL_COMPILER_ICC
#    define UTL_SOURCE_LOCATION() \
        UTL_SCOPE source_location::UTL_current(__FILE__, __PRETTY_FUNCTION__, __LINE__)
#  else
#    define UTL_SOURCE_LOCATION() \
        UTL_SCOPE source_location::UTL_current(__FILE__, __func__, __LINE__)
#  endif

#endif
