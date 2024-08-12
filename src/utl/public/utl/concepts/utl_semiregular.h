// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

#include "utl/concepts/utl_copyable.h"
#include "utl/concepts/utl_default_initializable.h"

#if UTL_CXX20
UTL_NAMESPACE_BEGIN

template <typename T>
concept semiregular = copyable<T> && default_initializable<T>;

UTL_NAMESPACE_END
#endif
