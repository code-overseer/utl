// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

UTL_NAMESPACE_BEGIN

template <template <typename...> class Variadic, typename... Types>
struct __UTL_PUBLIC_TEMPLATE variadic_proxy {
    template <typename... UTypes>
    using apply UTL_NODEBUG = Variadic<Types..., UTypes...>;
};

UTL_NAMESPACE_END
