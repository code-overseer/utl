// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

#if UTL_COMPILER_MSVC
extern "C" __int64 _ReadStatusReg(int);
#  pragma intrinsic(_ReadStatusReg)
#endif

UTL_NAMESPACE_BEGIN

#define __UTL_AARCH64_SYSREG(op0, op1, crn, crm, op2)                                \
    (((op0 & 1) << 14) | ((op1 & 7) << 11) | ((crn & 15) << 7) | ((crm & 15) << 3) | \
        ((op2 & 7) << 0))

namespace aarch64 {
inline namespace el0 {
struct system_register {
    enum : int {
        CNTFRQ = __UTL_AARCH64_SYSREG(3, 3, 14, 0, 0),
        CNTPCT = __UTL_AARCH64_SYSREG(3, 3, 14, 0, 1),
        CNTVCT = __UTL_AARCH64_SYSREG(3, 3, 14, 0, 2),
        PMCCNTR = __UTL_AARCH64_SYSREG(3, 3, 9, 13, 0)
    };
};

} // namespace el0
} // namespace aarch64

#undef __UTL_AARCH64_SYSREG

UTL_NAMESPACE_END
