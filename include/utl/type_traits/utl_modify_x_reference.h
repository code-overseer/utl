// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/type_traits/utl_common.h"

#ifdef UTL_USE_STD_TYPE_TRAITS

#  include <type_traits>

UTL_NAMESPACE_BEGIN

using std::add_lvalue_reference;
using std::add_rvalue_reference;
using std::remove_reference;

#  ifdef UTL_CXX14

using std::add_lvalue_reference_t;
using std::add_rvalue_reference_t;
using std::remove_reference_t;

#  else  // ifdef UTL_CXX14
template <typename T> using add_lvalue_reference_t = typename add_lvalue_reference<T>::type;
template <typename T> using add_rvalue_reference_t = typename add_rvalue_reference<T>::type;
template <typename T> using remove_reference_t = typename remove_reference<T>::type;
#  endif // ifdef UTL_CXX14

UTL_NAMESPACE_END

#  define UTL_TRAIT_SUPPORTED_add_lvalue_reference 1
#  define UTL_TRAIT_SUPPORTED_add_rvalue_reference 1
#  define UTL_TRAIT_SUPPORTED_remove_reference 1

#else // ifdef UTL_USE_STD_TYPE_TRAITS

#  include "utl/type_traits/utl_constants.h"

#  ifndef UTL_DISABLE_BUILTIN_remove_reference
#    define UTL_DISABLE_BUILTIN_remove_reference 0
#  endif // ifndef UTL_DISABLE_BUILTIN_remove_reference

#  ifndef UTL_DISABLE_BUILTIN_add_lvalue_reference
#    define UTL_DISABLE_BUILTIN_add_lvalue_reference 0
#  endif // ifndef UTL_DISABLE_BUILTIN_add_lvalue_reference

#  ifndef UTL_DISABLE_BUILTIN_add_rvalue_reference
#    define UTL_DISABLE_BUILTIN_add_rvalue_reference 0
#  endif // ifndef UTL_DISABLE_BUILTIN_add_rvalue_reference

#  if UTL_SHOULD_USE_BUILTIN(remove_reference)
#    define UTL_BUILTIN_remove_reference(...) __remove_reference(__VA_ARGS__)
#  endif // UTL_SHOULD_USE_BUILTIN(remove_reference)

#  if UTL_SHOULD_USE_BUILTIN(add_lvalue_reference)
#    define UTL_BUILTIN_add_lvalue_reference(...) __add_lvalue_reference(__VA_ARGS__)
#  endif // UTL_SHOULD_USE_BUILTIN(add_lvalue_reference)

#  if UTL_SHOULD_USE_BUILTIN(add_rvalue_reference)
#    define UTL_BUILTIN_add_rvalue_reference(...) __add_rvalue_reference(__VA_ARGS__)
#  endif // UTL_SHOULD_USE_BUILTIN(add_rvalue_reference)

#  ifdef UTL_BUILTIN_remove_reference

UTL_NAMESPACE_BEGIN

template <typename T> struct remove_reference {
    using type = UTL_BUILTIN_remove_reference(T);
};

template <typename T> using remove_reference_t = UTL_BUILTIN_remove_reference(T);

UTL_NAMESPACE_END

#  else // ifdef UTL_BUILTIN_remove_reference

UTL_NAMESPACE_BEGIN

template <typename T> struct remove_reference {
    using type = T;
};
template <typename T> struct remove_reference<T&> {
    using type = T;
};
template <typename T> struct remove_reference<T&&> {
    using type = T;
};

template <typename T> using remove_reference_t = typename remove_reference<T>::type;

UTL_NAMESPACE_END

#  endif // ifdef UTL_BUILTIN_remove_reference

#  ifdef UTL_BUILTIN_add_lvalue_reference

UTL_NAMESPACE_BEGIN

template <typename T> struct add_lvalue_reference {
    using type = UTL_BUILTIN_add_lvalue_reference(T);
};

template <typename T> using add_lvalue_reference_t = UTL_BUILTIN_add_lvalue_reference(T);

UTL_NAMESPACE_END

#  else // ifdef UTL_BUILTIN_add_lvalue_reference

UTL_NAMESPACE_BEGIN

namespace details {
namespace type_traits {
template <typename T> T& add_lvalue_ref(int) noexcept;
template <typename T> T  add_lvalue_ref(float) noexcept;
} // namespace type_traits
} // namespace details

template <typename T> struct add_lvalue_reference {
    using type = decltype(details::type_traits::add_rvalue_ref<T>(0));
};

template <typename T>
using add_lvalue_reference_t = decltype(details::type_traits::add_rvalue_ref<T>(0));

UTL_NAMESPACE_END

#  endif // ifdef UTL_BUILTIN_add_lvalue_reference

#  ifdef UTL_BUILTIN_add_rvalue_reference

UTL_NAMESPACE_BEGIN

template <typename T> struct add_rvalue_reference {
    using type = UTL_BUILTIN_add_rvalue_reference(T);
};

template <typename T> using add_rvalue_reference_t = UTL_BUILTIN_add_rvalue_reference(T);

UTL_NAMESPACE_END

#  else // ifdef UTL_BUILTIN_add_rvalue_reference

UTL_NAMESPACE_BEGIN

namespace details {
namespace type_traits {
template <typename T> T&& add_rvalue_ref(int) noexcept;
template <typename T> T   add_rvalue_ref(float) noexcept;
} // namespace type_traits
} // namespace details

template <typename T> struct add_rvalue_reference {
    using type = decltype(details::type_traits::add_rvalue_ref<T>(0));
};

template <typename T>
using add_rvalue_reference_t = decltype(details::type_traits::add_rvalue_ref<T>(0));

UTL_NAMESPACE_END

#  endif // ifdef UTL_BUILTIN_add_rvalue_reference

#  define UTL_TRAIT_SUPPORTED_add_lvalue_reference 1
#  define UTL_TRAIT_SUPPORTED_add_rvalue_reference 1
#  define UTL_TRAIT_SUPPORTED_remove_reference 1

#endif // ifdef UTL_USE_STD_TYPE_TRAITS
