// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_config.h"

UTL_NAMESPACE_BEGIN

template <template <typename...> class Variadic, typename... Types>
struct variadic_proxy {
    template <typename... UTypes>
    using apply = Variadic<Types..., UTypes...>;
};

UTL_NAMESPACE_END
