// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/type_traits/utl_common.h"

#ifdef UTL_USE_STD_TYPE_TRAITS

#include <type_traits>

UTL_NAMESPACE_BEGIN

using std::is_base_of;

#ifdef UTL_CXX17
using std::is_base_of_v;
#elif defined(UTL_CXX14)   // ifdef UTL_CXX17
template<typename B, typename D>
UTL_INLINE_CXX17 constexpr bool is_base_of_v = is_base_of<B,D>::value;
#endif  // ifdef UTL_CXX17

UTL_NAMESPACE_END

#define UTL_TRAIT_SUPPORTED_is_base_of 1

#else   // ifdef UTL_USE_STD_TYPE_TRAITS

#include "utl/type_traits/utl_constants.h"

#ifndef UTL_DISABLE_BUILTIN_is_base_of
#define UTL_DISABLE_BUILTIN_is_base_of 0
#endif  // ifndef UTL_DISABLE_BUILTIN_is_base_of

#if UTL_SHOULD_USE_BUILTIN(is_base_of)
#define UTL_BUILTIN_is_base_of(...) __is_base_of(__VA_ARGS__)
#endif  // UTL_SHOULD_USE_BUILTIN(is_base_of)

#ifdef UTL_BUILTIN_is_base_of

UTL_NAMESPACE_BEGIN

template<typename Base, typename Derive>
struct is_base_of : bool_constant<UTL_BUILTIN_is_base_of(Base, Derive)> {};

#ifdef UTL_CXX14
template<typename Base, typename Derive>
UTL_INLINE_CXX17 constexpr bool is_base_of_v = UTL_BUILTIN_is_base_of(Base, Derive);
#endif  // UTL_CXX14

UTL_NAMESPACE_END

#define UTL_TRAIT_SUPPORTED_is_base_of 1

#else   // ifdef UTL_BUILTIN_is_base_of

#include "utl/type_traits/utl_undefined_trait.h"

UTL_NAMESPACE_BEGIN

namespace type_traits {
namespace details {
    template<typename Base, typename Derive>
    auto unambiguous_public_inheritance(int) noexcept -> decltype(
            ((true_type(*)(Base const volatile*))0)(static_cast<Derive const volatile*>((Base const volatile*)0))
    );

    template<typename Base, typename Derive>
    auto unambiguous_public_inheritance(float) noexcept -> undefined_trait<Base, Derive>;

    template<typename Base, typename Derive>
    using is_unambiguous_public_inheritance = decltype(unambiguous_public_inheritance<Base, Derive>(0));
}
}

template<typename Base, typename Derive>
struct is_base_of : type_traits::details::is_unambiguous_public_inheritance<Base, Derive> {};

#ifdef UTL_CXX14
template<typename T>
UTL_INLINE_CXX17 constexpr bool is_base_of_v = is_base_of<T>::value;
#endif  // UTL_CXX14

UTL_NAMESPACE_END

#endif  // ifdef UTL_BUILTIN_is_base_of

#endif  // ifdef UTL_USE_STD_TYPE_TRAITS
