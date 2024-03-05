// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_namespace.h"
#include "utl/preprocessor/utl_standard.h"

#ifdef UTL_CXX20
UTL_NAMESPACE_BEGIN
namespace concepts {
namespace details {
template <typename T>
struct proxy_t {};
} // namespace details
} // namespace concepts

template <typename T, typename U>
concept same_as = requires(void (*f)(concepts::details::proxy_t<T>*),
                      concepts::details::proxy_t<U>* u) { f(u); } &&
    requires(void (*f)(concepts::details::proxy_t<U>*), concepts::details::proxy_t<T>* t) { f(t); };

UTL_NAMESPACE_END
#endif
