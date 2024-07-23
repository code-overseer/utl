// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_config.h"

#if UTL_CXX20
#  include "utl/concepts/utl_predicate.h"

UTL_NAMESPACE_BEGIN
template <typename F, typename... Ts>
struct is_predicate : bool_constant<predicate<F, Ts...>> {};

template <typename F, typename... Ts>
inline constexpr bool is_predicate_v = predicate<F, Ts...>;
UTL_NAMESPACE_END

#  define UTL_TRAIT_is_predicate(...) UTL_SCOPE predicate<__VA_ARGS__>

#else

#  include "utl/type_traits/utl_invoke.h"
#  include "utl/type_traits/utl_is_boolean_testable.h"

UTL_NAMESPACE_BEGIN
template <typename F, typename... Ts>
struct is_predicate : is_boolean_testable<invoke_result_t<F, Ts...>> {};

#  if UTL_CXX14
template <typename F, typename... Ts>
UTL_INLINE_CXX17 constexpr bool is_predicate_v = is_boolean_testable_v<invoke_result_t<F, Ts...>>;

#    define UTL_TRAIT_is_predicate(...) \
        UTL_SCOPE is_boolean_testable_v<invoke_result_t<__VA_ARGS__>>

#  else

#    define UTL_TRAIT_is_predicate(...) \
        UTL_SCOPE is_boolean_testable<invoke_result_t<__VA_ARGS__>>::value

#  endif

UTL_NAMESPACE_END
#endif
