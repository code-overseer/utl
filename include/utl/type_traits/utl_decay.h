// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/type_traits/utl_common.h"

#ifdef UTL_USE_STD_TYPE_TRAITS

#  include <type_traits>

UTL_NAMESPACE_BEGIN

using std::decay;

#  ifdef UTL_CXX14

using std::decay_t;

#  else // UTL_CXX14

template <typename T> using decay_t = typename decay<T>::type;

#  endif // UTL_CXX14

UTL_NAMESPACE_END

#else // ifdef UTL_USE_STD_TYPE_TRAITS

#  ifndef UTL_DISABLE_BUILTIN_decay
#    define UTL_DISABLE_BUILTIN_decay 0
#  endif

#  if UTL_SHOULD_USE_BUILTIN(decay)
#    define UTL_BUILTIN_decay(...) __decay(__VA_ARGS__)
#  endif // UTL_SHOULD_USE_BUILTIN(decay)

#  ifdef UTL_BUILTIN_decay

UTL_NAMESPACE_BEGIN

template <typename T> struct decay {
    using type = UTL_BUILTIN_decay(T);
};

template <typename T> using decay_t = UTL_BUILTIN_decay(T);

UTL_NAMESPACE_END

#  else // ifdef UTL_BUILTIN_decay

#    include "utl/type_traits/utl_enable_if.h"
#    include "utl/type_traits/utl_is_function.h"
#    include "utl/type_traits/utl_modify_x_cv.h"
#    include "utl/type_traits/utl_modify_x_reference.h"

UTL_NAMESPACE_BEGIN

namespace details {
namespace type_traits {
template <typename T, typename = void> struct decay_impl {
    using type = remove_cv_t<T>;
};

template <typename T> struct decay_impl<T, enable_if_t<is_function<T>::value>> {
    using type = T*;
};

template <typename T> struct decay_impl<T[]> {
    using type = T*;
};

template <typename T, size_t N> struct decay_impl<T[N]> {
    using type = T*;
};
} // namespace type_traits
} // namespace details

template <typename T> struct decay : details::type_traits::decay_impl<remove_reference_t<T>> {};

using decay_t = typename details::type_traits::decay_impl<remove_reference_t<T>>::type;

UTL_NAMESPACE_END

#  endif // ifdef UTL_BUILTIN_decay

#endif // ifdef UTL_USE_STD_TYPE_TRAITS

#define UTL_TRAIT_SUPPORTED_decay 1
