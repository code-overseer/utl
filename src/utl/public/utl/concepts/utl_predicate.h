// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/concepts/utl_boolean_testable.h"
#include "utl/concepts/utl_invocable.h"
#include "utl/preprocessor/utl_config.h"

#if UTL_CXX20
UTL_NAMESPACE_BEGIN

template <typename F, typename... Args>
concept predicate = regular_invocable<F, Args...> && boolean_testable<invoke_result_t<F, Args...>>;

UTL_NAMESPACE_END
#endif
