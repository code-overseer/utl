// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/type_traits/utl_common.h"

#if UTL_USE_STD_TYPE_TRAITS

#  include <type_traits>

UTL_NAMESPACE_BEGIN

using std::add_lvalue_reference;

#  if UTL_CXX14

using std::add_lvalue_reference_t;

#  else  // if UTL_CXX14
template <typename T>
using add_lvalue_reference_t = typename add_lvalue_reference<T>::type;
#  endif // if UTL_CXX14

UTL_NAMESPACE_END

#else // ifdef UTL_USE_STD_TYPE_TRAITS

#  include "utl/type_traits/utl_constants.h"

#  if UTL_SHOULD_USE_BUILTIN(add_lvalue_reference)
#    define UTL_BUILTIN_add_lvalue_reference(...) __add_lvalue_reference(__VA_ARGS__)
#  endif // UTL_SHOULD_USE_BUILTIN(add_lvalue_reference)

#  ifdef UTL_BUILTIN_add_lvalue_reference

UTL_NAMESPACE_BEGIN

template <typename T>
struct add_lvalue_reference {
    using type = UTL_BUILTIN_add_lvalue_reference(T);
};

template <typename T>
using add_lvalue_reference_t = UTL_BUILTIN_add_lvalue_reference(T);

UTL_NAMESPACE_END

#  else // ifdef UTL_BUILTIN_add_lvalue_reference

UTL_NAMESPACE_BEGIN

namespace details {
namespace lvalue_reference {
template <typename T>
T& impl(int) noexcept;
template <typename T>
T impl(float) noexcept;
} // namespace lvalue_reference
} // namespace details

template <typename T>
struct add_lvalue_reference {
    using type = decltype(details::lvalue_reference::impl<T>(0));
};

template <typename T>
using add_lvalue_reference_t = decltype(details::lvalue_reference::impl<T>(0));

UTL_NAMESPACE_END

#  endif // ifdef UTL_BUILTIN_add_lvalue_reference
#endif   // ifdef UTL_USE_STD_TYPE_TRAITS

#define UTL_TRAIT_SUPPORTED_add_lvalue_reference 1
