// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_config.h"

#if UTL_CXX20
#  include "utl/concepts/utl_same_as.h"
#  include "utl/concepts/utl_unsigned_integral.h"
#  include "utl/type_traits/utl_is_same.h"
#else
#  include "utl/type_traits/utl_enable_if.h"
#  include "utl/type_traits/utl_is_unsigned.h"
#endif

#include <limits.h>

UTL_NAMESPACE_BEGIN

#if UTL_SUPPORTS_CHAR8_T
#  define __UTL_IS_CHAR8_CXX20(TYPE) UTL_TRAIT_is_same(char8_t, TYPE)
#else
#  define __UTL_IS_CHAR8_CXX20(TYPE) false
#endif

template <typename T>
using is_bit_readable = bool_constant<UTL_TRAIT_is_unsigned(T) && !UTL_TRAIT_is_same(bool, T) &&
    !UTL_TRAIT_is_same(char, T) && !UTL_TRAIT_is_same(char16_t, T) &&
    !UTL_TRAIT_is_same(char32_t, T) && !UTL_TRAIT_is_same(wchar_t, T) && !__UTL_IS_CHAR8_CXX20(T)>;

#if UTL_CXX14
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_bit_readable_v = UTL_TRAIT_is_unsigned(T) && !UTL_TRAIT_is_same(bool, T) &&
    !UTL_TRAIT_is_same(char, T) && !UTL_TRAIT_is_same(char16_t, T) &&
    !UTL_TRAIT_is_same(char32_t, T) && !UTL_TRAIT_is_same(wchar_t, T) && !__UTL_IS_CHAR8_CXX20(T);
#  define UTL_TRAIT_is_bit_readable(...) UTL_SCOPE is_bit_readable_v<__VA_ARGS__>
#else
#  define UTL_TRAIT_is_bit_readable(...) UTL_SCOPE is_bit_readable<__VA_ARGS__>::value
#endif

#if UTL_CXX20
template <typename T>
concept bit_readable = unsigned_integral<T> && !same_as<T, bool> && !same_as<T, char> &&
    !same_as<T, wchar_t> && !same_as<T, char8_t> && !same_as<T, char16_t> && !same_as<T, char32_t>;
#endif

#undef __UTL_IS_CHAR8_CXX20

namespace details {
namespace bit {
template <typename T>
constexpr int unsigned_width_diff() noexcept {
    return CHAR_BIT * ((int)sizeof(unsigned) - (int)sizeof(T));
}
} // namespace bit
} // namespace details

UTL_NAMESPACE_END
