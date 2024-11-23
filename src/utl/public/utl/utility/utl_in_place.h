// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

#include "utl/concepts/utl_same_as.h"
#include "utl/type_traits/utl_is_same.h"

UTL_STD_NAMESPACE_BEGIN
struct in_place_t;
UTL_STD_NAMESPACE_END

UTL_NAMESPACE_BEGIN

struct __UTL_ABI_PUBLIC in_place_t {
    __UTL_HIDE_FROM_ABI explicit constexpr in_place_t() noexcept = default;
#ifdef UTL_CXX17
    template <UTL_CONCEPT_CXX20(same_as<::std::in_place_t>) U = ::std::in_place_t
            UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_same(U, ::std::in_place_t))>
    __UTL_HIDE_FROM_ABI constexpr in_place_t(U) noexcept {}
    template <UTL_CONCEPT_CXX20(same_as<::std::in_place_t>) U = ::std::in_place_t
            UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_same(U, ::std::in_place_t))>
    __UTL_HIDE_FROM_ABI constexpr operator U() const noexcept {
        return U{};
    }
#endif
};

#if UTL_CXX14
UTL_INLINE_CXX17 constexpr in_place_t in_place{};
#endif

namespace details {
namespace in_place {

#if UTL_CXX20
template <typename U>
concept tag_type = (same_as<U, ::std::in_place_t> || same_as<U, in_place_t>)

#  define __UTL_TRAIT_in_place_tag(...) __UTL details::in_place::tag_type<__VA_ARGS__>

#elif UTL_CXX14
template <typename U>
UTL_INLINE_CXX17 constexpr bool tag_type =
    UTL_TRAIT_is_same(U, ::std::in_place_t) || UTL_TRAIT_is_same(U, in_place_t);
#  define __UTL_TRAIT_in_place_tag(...) __UTL details::in_place::tag_type<__VA_ARGS__>

#else

template <typename U>
using tag_type =
    bool_constant<UTL_TRAIT_is_same(U, ::std::in_place_t) || UTL_TRAIT_is_same(U, in_place_t)>;
#  define __UTL_TRAIT_in_place_tag(...) __UTL details::in_place::tag_type<__VA_ARGS__>::value

#endif
} // namespace in_place
} // namespace details

UTL_NAMESPACE_END
