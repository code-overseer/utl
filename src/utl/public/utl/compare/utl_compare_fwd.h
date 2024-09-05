// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

#if UTL_COMPILER_GCC
// TODO: add preprocessor flag to either include std header/use this UB/disable std compatibility
namespace std {
/* UTL_UNDEFINED_BEHAVIOUR */
/* @note (22/12/2023) GCC does not put the orderings under an inline namespace */
class __UTL_ABI_PUBLIC strong_ordering;
class __UTL_ABI_PUBLIC partial_ordering;
class __UTL_ABI_PUBLIC weak_ordering;
} // namespace std
#else
UTL_STD_NAMESPACE_BEGIN
/* UTL_UNDEFINED_BEHAVIOUR */
class strong_ordering;
class partial_ordering;
class weak_ordering;
UTL_STD_NAMESPACE_END
#endif
