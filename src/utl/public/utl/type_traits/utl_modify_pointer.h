// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/type_traits/utl_common.h"

#ifdef UTL_USE_STD_TYPE_TRAITS

#  include <type_traits>

UTL_NAMESPACE_BEGIN

using std::add_pointer;
using std::remove_pointer;

#  ifdef UTL_CXX14

using std::add_pointer_t;
using std::remove_pointer_t;

#  else // UTL_CXX14

template <typename T>
using add_pointer_t = typename add_pointer<T>::type;
template <typename T>
using remove_pointer_t = typename remove_pointer<T>::type;

#  endif // UTL_CXX14

UTL_NAMESPACE_END

#else // ifdef UTL_USE_STD_TYPE_TRAITS

#  ifndef UTL_DISABLE_BUILTIN_add_pointer
#    define UTL_DISABLE_BUILTIN_add_pointer 0
#  endif

#  ifndef UTL_DISABLE_BUILTIN_remove_pointer
#    define UTL_DISABLE_BUILTIN_remove_pointer 0
#  endif

#  if UTL_SHOULD_USE_BUILTIN(add_pointer)
#    define UTL_BUILTIN_add_pointer(...) __add_pointer(__VA_ARGS__)
#  endif // UTL_SHOULD_USE_BUILTIN(add_pointer)

#  if UTL_SHOULD_USE_BUILTIN(remove_pointer)
#    define UTL_BUILTIN_remove_pointer(...) __remove_pointer(__VA_ARGS__)
#  endif // UTL_SHOULD_USE_BUILTIN(remove_pointer)

#  ifdef UTL_BUILTIN_add_pointer

UTL_NAMESPACE_BEGIN

template <typename T>
struct add_pointer {
    using type = UTL_BUILTIN_add_pointer(T);
};

template <typename T>
using add_pointer_t = UTL_BUILTIN_add_pointer(T);

UTL_NAMESPACE_END

#  else // ifdef UTL_BUILTIN_add_pointer

#    include "utl/type_traits/utl_remove_reference.h"

UTL_NAMESPACE_BEGIN

namespace details {
namespace type_traits {
template <typename T, typename = T&>
remove_reference_t<T>* add_ptr(int) noexcept;
template <typename T>
auto add_ptr(float) noexcept
    -> decltype(static_cast<T* (*)(T const volatile*)>(0)((void const volatile*)0));
template <typename T>
T add_ptr(...) noexcept;
} // namespace type_traits
} // namespace details

template <typename T>
struct add_pointer {
    using type = decltype(details::type_traits::add_ptr<T>(0));
};

template <typename T>
using add_pointer_t = decltype(details::type_traits::add_ptr<T>(0));

UTL_NAMESPACE_END

#  endif // ifdef UTL_BUILTIN_add_pointer

#  ifdef UTL_BUILTIN_remove_pointer

UTL_NAMESPACE_BEGIN

template <typename T>
struct remove_pointer {
    using type = UTL_BUILTIN_remove_pointer(T);
};

template <typename T>
using remove_pointer_t = UTL_BUILTIN_remove_pointer(T);

UTL_NAMESPACE_END

#  else // ifdef UTL_BUILTIN_remove_pointer

UTL_NAMESPACE_BEGIN

template <typename T>
struct remove_pointer {
    using type = T;
};
template <typename T>
struct remove_pointer<T*> {
    using type = T;
};
template <typename T>
struct remove_pointer<T* const> {
    using type = T;
};
template <typename T>
struct remove_pointer<T* volatile> {
    using type = T;
};
template <typename T>
struct remove_pointer<T* const volatile> {
    using type = T;
};

template <typename T>
using remove_pointer_t = typename remove_pointer<T>::type;

UTL_NAMESPACE_END

#  endif // ifdef UTL_BUILTIN_remove_pointer

#endif // ifdef UTL_USE_STD_TYPE_TRAITS

#define UTL_TRAIT_SUPPORTED_add_pointer 1
#define UTL_TRAIT_SUPPORTED_remove_pointer 1
