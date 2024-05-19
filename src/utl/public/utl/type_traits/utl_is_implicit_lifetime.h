// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/type_traits/utl_common.h"

#if defined(UTL_USE_STD_TYPE_TRAITS) && defined(UTL_CXX23)

#  include <type_traits>

UTL_NAMESPACE_BEGIN

using std::is_implicit_lifetime;

using std::is_implicit_lifetime_v;

UTL_NAMESPACE_END

#else // ifdef UTL_USE_STD_TYPE_TRAITS

#  ifndef UTL_DISABLE_BUILTIN_is_implicit_lifetime
#    define UTL_DISABLE_BUILTIN_is_implicit_lifetime 0
#  endif

#  if UTL_SHOULD_USE_BUILTIN(is_implicit_lifetime)
#    define UTL_BUILTIN_is_implicit_lifetime(...) __is_implicit_lifetime(__VA_ARGS__)
#  endif // UTL_SHOULD_USE_BUILTIN(is_implicit_lifetime)

#  ifdef UTL_BUILTIN_is_implicit_lifetime

UTL_NAMESPACE_BEGIN

template <typename T>
struct is_implicit_lifetime {
    using type = UTL_BUILTIN_is_implicit_lifetime(T);
};

template <typename T>
using is_implicit_lifetime_v = UTL_BUILTIN_is_implicit_lifetime(T);

UTL_NAMESPACE_END

#  else // ifdef UTL_BUILTIN_is_implicit_lifetime

#    include "utl/type_traits/utl_is_aggregate.h"
#    include "utl/type_traits/utl_is_trivially_copy_constructible.h"
#    include "utl/type_traits/utl_is_trivially_default_constructible.h"
#    include "utl/type_traits/utl_is_trivially_destructible.h"
#    include "utl/type_traits/utl_is_trivially_move_constructible.h"

UTL_NAMESPACE_BEGIN

template <typename T>
struct is_implicit_lifetime :
    bool_constant<UTL_TRAIT_is_trivially_destructible(T) &&
        (UTL_TRAIT_is_aggregate(T) || UTL_TRAIT_is_trivially_default_constructible(T) ||
            UTL_TRAIT_is_trivially_copy_constructible(T) ||
            UTL_TRAIT_is_trivially_move_constructible(T))> {};

#    ifdef UTL_CXX14
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_implicit_lifetime_v = UTL_TRAIT_is_trivially_destructible(T) &&
    (UTL_TRAIT_is_aggregate(T) || UTL_TRAIT_is_trivially_default_constructible(T) ||
        UTL_TRAIT_is_trivially_copy_constructible(T) ||
        UTL_TRAIT_is_trivially_move_constructible(T));
#    endif // UTL_CXX14
UTL_NAMESPACE_END

#  endif // ifdef UTL_BUILTIN_is_implicit_lifetime

#endif // ifdef UTL_USE_STD_TYPE_TRAITS

#define UTL_TRAIT_SUPPORTED_is_implicit_lifetime 1

#ifdef UTL_BUILTIN_is_implicit_lifetime
#  define UTL_TRAIT_is_implicit_lifetime(...) UTL_BUILTIN_is_implicit_lifetime(__VA_ARGS__)
#elif UTL_CXX14
#  define UTL_TRAIT_is_implicit_lifetime(...) UTL_SCOPE is_implicit_lifetime_v<__VA_ARGS__>
#else
#  define UTL_TRAIT_is_implicit_lifetime(...) UTL_SCOPE is_implicit_lifetime<__VA_ARGS__>::value
#endif
