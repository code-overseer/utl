// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_config.h"
#include "utl/type_traits/utl_declval.h"
#include "utl/utility/utl_forward.h"

#ifdef UTL_CXX20
#  if defined(UTL_COMPILER_CLANG) | defined(UTL_COMPILER_ICX)
#    include <new>
/**
 * The Clang frontend allows constexpr placement new within the std namespace
 */
namespace std {
/* UTL_UNDEFINED_BEHAVIOUR */
template <typename T, typename... Args>
inline constexpr T* utl_construct_at_utl(T* location, Args&&... args) noexcept(
    noexcept(::new((void*)0) T(declval<Args>()...))) {
    UTL_ASSERT_CXX14(location != nullptr);
    return ::new ((void*)(location)) T(forward<Args>(args)...);
}
} // namespace std

UTL_NAMESPACE_BEGIN
template <typename T, typename... Args>
inline constexpr T* construct_at(T* location, Args&&... args) noexcept(
    noexcept(::new((void*)0) T(declval<Args>()...))) {
    UTL_ASSERT_CXX14(location != nullptr);
    return ::std::utl_construct_at_utl(location, forward<Args>(args)...);
}
UTL_NAMESPACE_END
#    define UTL_CONSTEXPR_CONSTRUCTS_AT constexpr

#  elif defined(UTL_COMPILER_GCC)

#    include <bits/stl_construct.h>
/**
 * GCC frontend relies on compiler magic specifically for std::construct_at
 */
UTL_NAMESPACE_BEGIN
using std::construct_at;
UTL_NAMESPACE_END
#    define UTL_CONSTEXPR_CONSTRUCTS_AT constexpr

#  elif defined(UTL_COMPILER_MSVC)
/**
 * MSVC uses a constexpr attribute to allow constexpr placement-new
 */
#    if UTL_HAS_CPP_ATTRIBUTE(msvc::constexpr)
#      include <new>
UTL_NAMESPACE_BEGIN
template <typename T, typename... Args>
inline constexpr T* construct_at(T* location, Args&&... args) noexcept(
    noexcept(::new((void*)0) T(declval<Args>()...))) {
    UTL_ASSERT_CXX14(location != nullptr);
    [[msvc::constexpr]] return ::new ((void*)(location)) T(forward<Args>(args)...);
}
UTL_NAMESPACE_END
#      define UTL_CONSTEXPR_CONSTRUCTS_AT constexpr
#    endif // UTL_HAS_CPP_ATTRIBUTE(msvc::constexpr)
#  endif   // if defined(UTL_COMPILER_CLANG) | defined(UTL_COMPILER_ICX)
#endif     // ifdef UTL_CXX20

#ifndef UTL_CONSTEXPR_CONSTRUCTS_AT
#  include <new>
UTL_NAMESPACE_BEGIN
template <typename T, typename... Args>
inline T* construct_at(T* location, Args&&... args) noexcept(
    noexcept(::new((void*)0) T(declval<Args>()...))) {
    UTL_ASSERT(location != nullptr);
    return ::new ((void*)(location)) T(forward<Args>(args)...);
}
UTL_NAMESPACE_END
#  define UTL_CONSTEXPR_CONSTRUCTS_AT
#endif // ifndef UTL_CONSTEXPR_CONSTRUCTS_AT