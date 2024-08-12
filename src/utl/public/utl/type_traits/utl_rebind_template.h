// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_namespace.h"

UTL_NAMESPACE_BEGIN

template <typename From, template <typename...> To>
struct __UTL_PUBLIC_TEMPLATE rebind_template;

template <template <typename...> class From, template <typename...> class To, typename... A>
struct __UTL_PUBLIC_TEMPLATE rebind_template<From<A...>, To> {
    using type UTL_NODEBUG = To<A...>;
};

template <typename From, template <typename...> To>
using rebind_template_t = typename rebind_template<From, To>::type;

UTL_NAMESPACE_END
