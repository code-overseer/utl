// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

#if UTL_COMPILER_MSVC
extern "C" void __isb(unsigned int);
extern "C" void __dmb(unsigned int);
extern "C" void __dsb(unsigned int);
#  pragma intrinsic(__isb)
#  pragma intrinsic(__dmb)
#  pragma intrinsic(__dsb)
#endif

UTL_NAMESPACE_BEGIN
namespace arm {
struct barrier {
    enum : int {
        SY = 0xF,
        ST = 0xE,
        LD = 0xD,
        ISH = 0xB,
        ISHST = 0xA,
        ISHLD = 0x9,
        NSH = 0x7,
        NSHST = 0x6,
        NSHLD = 0x5,
        OSH = 0x3,
        OSHST = 0x2,
        OSHLD = 0x1
    };
};
} // namespace arm

UTL_NAMESPACE_END
