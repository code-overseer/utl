// Copyright 2023-2024 Bryan Wong

#pragma once

#if !defined(UTL_ATOMIC_PRIVATE_HEADER_GUARD)
#  error "Private header accessed"
#endif

#include "utl/utl_config.h"

#if !UTL_COMPILER_MSVC
#  error Invalid compiler
#endif

extern "C" __int8 __iso_volatile_load8(__int8 const volatile*);
extern "C" __int16 __iso_volatile_load16(__int16 const volatile*);
extern "C" __int32 __iso_volatile_load32(__int32 const volatile*);
extern "C" __int64 __iso_volatile_load64(__int64 const volatile*);
extern "C" unsigned __int8 __ldar8(unsigned __int8 volatile*);
extern "C" unsigned __int16 __ldar16(unsigned __int16 volatile*);
extern "C" unsigned __int32 __ldar32(unsigned __int32 volatile*);
extern "C" unsigned __int64 __ldar64(unsigned __int64 volatile*);
extern "C" void __iso_volatile_store8(__int8 volatile*, __int8);
extern "C" void __iso_volatile_store16(__int16 volatile*, __int16);
extern "C" void __iso_volatile_store32(__int32 volatile*, __int32);
extern "C" void __iso_volatile_store64(__int64 volatile*, __int64);
extern "C" void __stlr8(unsigned __int8 volatile*, unsigned __int8);
extern "C" void __stlr16(unsigned __int16 volatile*, unsigned __int16);
extern "C" void __stlr32(unsigned __int32 volatile*, unsigned __int32);
extern "C" void __stlr64(unsigned __int8 volatile*, unsigned __int64);
#pragma intrinsic(__iso_volatile_load8)
#pragma intrinsic(__iso_volatile_load16)
#pragma intrinsic(__iso_volatile_load32)
#pragma intrinsic(__iso_volatile_load64)
#pragma intrinsic(__ldar8)
#pragma intrinsic(__ldar16)
#pragma intrinsic(__ldar32)
#pragma intrinsic(__ldar64)
#pragma intrinsic(__iso_volatile_store8)
#pragma intrinsic(__iso_volatile_store16)
#pragma intrinsic(__iso_volatile_store32)
#pragma intrinsic(__iso_volatile_store64)
#pragma intrinsic(__stlr8)
#pragma intrinsic(__stlr16)
#pragma intrinsic(__stlr32)
#pragma intrinsic(__stlr64)

#if __ARM_ARCH >= 830 && !defined(UTL_ENABLE_RCPC_LOAD)
#  define UTL_ENABLE_RCPC_LOAD
#endif

#ifdef UTL_ENABLE_RCPC_LOAD
extern "C" unsigned __int8 __ldapr8(unsigned __int8 volatile*);
extern "C" unsigned __int16 __ldapr16(unsigned __int16 volatile*);
extern "C" unsigned __int32 __ldapr32(unsigned __int32 volatile*);
extern "C" unsigned __int64 __ldapr64(unsigned __int64 volatile*);
#  pragma intrinsic(__ldapr8)
#  pragma intrinsic(__ldapr16)
#  pragma intrinsic(__ldapr32)
#  pragma intrinsic(__ldapr64)
#endif
