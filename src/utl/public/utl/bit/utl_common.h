// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_config.h"

#ifdef UTL_CXX20
#  include "utl/concepts.h" // unsigned_integral
#  include "utl/concepts/utl_same_as.h"
#  include "utl/type_traits/utl_is_same.h"
#else
#  include "utl/type_traits/utl_enable_if.h"
#  include "utl/type_traits/utl_is_unsigned.h"
#endif

#include <limits.h>

UTL_NAMESPACE_BEGIN

#ifdef UTL_SUPPORTS_CHAR8_T
#  define UTL_IS_CHAR8_CXX20(TYPE) UTL_TRAIT_VALUE(is_same, char8_t, TYPE)
#else
#  define UTL_IS_CHAR8_CXX20(TYPE) false
#endif

template <typename T>
using is_bit_readable = bool_constant<UTL_TRAIT_VALUE(is_unsigned, T) &&
    !UTL_TRAIT_VALUE(is_same, bool, T) && !UTL_TRAIT_VALUE(is_same, char, T) &&
    !UTL_TRAIT_VALUE(is_same, char16_t, T) && !UTL_TRAIT_VALUE(is_same, char32_t, T) &&
    !UTL_TRAIT_VALUE(is_same, wchar_t, T) && !UTL_IS_CHAR8_CXX20(T)>;

#ifdef UTL_CXX14
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_bit_readable_v = UTL_TRAIT_VALUE(is_unsigned, T) &&
    !UTL_TRAIT_VALUE(is_same, bool, T) && !UTL_TRAIT_VALUE(is_same, char, T) &&
    !UTL_TRAIT_VALUE(is_same, char16_t, T) && !UTL_TRAIT_VALUE(is_same, char32_t, T) &&
    !UTL_TRAIT_VALUE(is_same, wchar_t, T) && !UTL_IS_CHAR8_CXX20(T);
#endif

#ifdef UTL_CXX20
template <typename T>
concept bit_readable = unsigned_integral<T> && !same_as<T, bool> && !same_as<T, char> &&
    !same_as<T, wchar_t> && !same_as<T, char8_t> && !same_as<T, char16_t> && !same_as<T, char32_t>;
#endif

#undef UTL_IS_CHAR8_CXX20

namespace bit {
namespace details {
template <typename T>
constexpr int unsigned_width_diff() noexcept {
    return CHAR_BIT * (sizeof(unsigned) - sizeof(T));
}
} // namespace details
} // namespace bit

UTL_NAMESPACE_END
