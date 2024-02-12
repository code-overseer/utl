// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/type_traits/utl_common.h"

#if defined(UTL_USE_STD_TYPE_TRAITS) && defined(UTL_CXX20)

#include <type_traits>

UTL_NAMESPACE_BEGIN

using std::remove_cvref;

using std::remove_cvref_t;

UTL_NAMESPACE_END

#else   // ifdef UTL_USE_STD_TYPE_TRAITS

#ifndef UTL_DISABLE_BUILTIN_remove_cvref
#define UTL_DISABLE_BUILTIN_remove_cvref 0
#endif

#if UTL_SHOULD_USE_BUILTIN(remove_cvref)
#define UTL_BUILTIN_remove_cvref(...) __remove_cvref(__VA_ARGS__)
#endif  // UTL_SHOULD_USE_BUILTIN(remove_cvref)

#ifdef UTL_BUILTIN_remove_cvref

UTL_NAMESPACE_BEGIN

template<typename T>
struct remove_cvref { using type = UTL_BUILTIN_remove_cvref(T); };

template<typename T>
using remove_cvref_t = UTL_BUILTIN_remove_cvref(T);

UTL_NAMESPACE_END

#else   // ifdef UTL_BUILTIN_remove_cvref

#include "utl/type_traits/utl_modify_x_reference.h"

UTL_NAMESPACE_BEGIN

template<typename T>
struct remove_cvref : remove_reference<remove_cv_t<T>> {};

template<typename T>
using remove_cvref_t = typename remove_cvref<T>::type;

UTL_NAMESPACE_END

#endif  // ifdef UTL_BUILTIN_remove_cvref

#endif  // ifdef UTL_USE_STD_TYPE_TRAITS

#define UTL_TRAIT_SUPPORTED_remove_cvref 1
