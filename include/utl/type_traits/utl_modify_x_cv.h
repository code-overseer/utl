// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/type_traits/utl_common.h"

#ifdef UTL_USE_STD_TYPE_TRAITS

#  include <type_traits>

UTL_NAMESPACE_BEGIN

using std::add_const;
using std::add_cv;
using std::add_volatile;
using std::remove_const;
using std::remove_cv;
using std::remove_volatile;

#  ifdef UTL_CXX14

using std::add_const_t;
using std::add_cv_t;
using std::add_volatile_t;
using std::remove_const_t;
using std::remove_cv_t;
using std::remove_volatile_t;

#  else // UTL_CXX14

template <typename T>
using add_const_t = T const;
template <typename T>
using add_volatile_t = T volatile;
template <typename T>
using add_cv_t = T const volatile;
template <typename T>
using remove_const_t = typename remove_const<T>::type;
template <typename T>
using remove_volatile_t = typename remove_volatile<T>::type;
template <typename T>
using remove_cv_t = typename remove_volatile<T>::type;

#  endif // UTL_CXX14

UTL_NAMESPACE_END

#else // ifdef UTL_USE_STD_TYPE_TRAITS

#  include "utl/type_traits/utl_constants.h"

#  ifndef UTL_DISABLE_BUILTIN_remove_const
#    define UTL_DISABLE_BUILTIN_remove_const 0
#  endif // ifndef UTL_DISABLE_BUILTIN_remove_const
#  ifndef UTL_DISABLE_BUILTIN_remove_volatile
#    define UTL_DISABLE_BUILTIN_remove_volatile 0
#  endif // ifndef UTL_DISABLE_BUILTIN_remove_volatile

#  if UTL_SHOULD_USE_BUILTIN(remove_const)
#    define UTL_BUILTIN_add_const(...) __remove_const(__VA_ARGS__)
#  endif // UTL_SHOULD_USE_BUILTIN(add_const)

#  if UTL_SHOULD_USE_BUILTIN(remove_volatile)
#    define UTL_BUILTIN_add_volatile(...) __remove_volatile(__VA_ARGS__)
#  endif // UTL_SHOULD_USE_BUILTIN(add_volatile)

#  ifdef UTL_BUILTIN_remove_const

UTL_NAMESPACE_BEGIN

template <typename T>
struct remove_volatile {
    using type = UTL_BUILTIN_remove_const(T);
};

template <typename T>
using remove_const_t = UTL_BUILTIN_remove_const(T);

UTL_NAMESPACE_END

#  else // ifdef UTL_BUILTIN_remove_const

UTL_NAMESPACE_BEGIN

template <typename T>
struct remove_const {
    using type = T;
};
template <typename T>
struct remove_const<T const> {
    using type = T;
};

template <typename T>
using remove_const_t = typename remove_const<T>::type;

UTL_NAMESPACE_END

#  endif // ifdef UTL_BUILTIN_remove_const

#  ifdef UTL_BUILTIN_remove_volatile

UTL_NAMESPACE_BEGIN

template <typename T>
struct remove_volatile {
    using type = UTL_BUILTIN_remove_volatile(T);
};

template <typename T>
using remove_volatile_t = UTL_BUILTIN_remove_volatile(T);

UTL_NAMESPACE_END

#  else // ifdef UTL_BUILTIN_remove_volatile

UTL_NAMESPACE_BEGIN

template <typename T>
struct remove_volatile {
    using type = T;
};
template <typename T>
struct remove_volatile<T volatile> {
    using type = T;
};

template <typename T>
using remove_volatile_t = typename remove_volatile<T>::type;

UTL_NAMESPACE_END

#  endif // ifdef UTL_BUILTIN_remove_volatile

UTL_NAMESPACE_BEGIN

template <typename T>
struct add_const {
    using type = T const;
};
template <typename T>
struct add_volatile {
    using type = T volatile;
};
template <typename T>
struct add_cv {
    using type = T const volatile;
};
template <typename T>
struct remove_cv {
    using type = remove_const_t<remove_volatile_t<T>>;
};

template <typename T>
using add_const_t = T const;
template <typename T>
using add_volatile_t = T volatile;
template <typename T>
using add_cv_t = T const volatile;
template <typename T>
using remove_cv_t = remove_const_t<remove_volatile_t<T>>;

UTL_NAMESPACE_END

#endif // ifdef UTL_USE_STD_TYPE_TRAITS

#define UTL_TRAIT_SUPPORTED_add_const 1
#define UTL_TRAIT_SUPPORTED_add_volatile 1
#define UTL_TRAIT_SUPPORTED_add_cv 1
#define UTL_TRAIT_SUPPORTED_remove_const 1
#define UTL_TRAIT_SUPPORTED_remove_volatile 1
#define UTL_TRAIT_SUPPORTED_remove_cv 1
