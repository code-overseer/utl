// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_namespace.h"
#include "utl/preprocessor/utl_standard.h"
#include "utl/type_traits/utl_constants.h"

UTL_NAMESPACE_BEGIN

namespace type_traits {
namespace details {
using size_t = decltype(sizeof(0));

template<typename T, size_t = sizeof(T)>
true_type is_complete(int);

template<typename T>
false_type is_complete(float);

}
}

template<typename T, typename R = decltype(type_traits::details::is_complete<T>(0))>
struct is_complete : R {};

#ifdef UTL_CXX14
template<typename T, typename R = decltype(type_traits::details::is_complete<T>(0))>
UTL_INLINE_CXX17 constexpr bool is_complete_v = R::value;
#endif  // UTL_CXX14

UTL_NAMESPACE_END
