// Copyright 2023-2024 Bryan Wong

#pragma once

#if !defined(UTL_ATOMIC_PRIVATE_HEADER_GUARD)
#  error "Private header accessed"
#endif

#include "utl/utl_config.h"

#if !UTL_COMPILER_MSVC
#  error Invalid compiler
#endif

extern "C" char _InterlockedExchange8(char volatile*, char);
extern "C" char _InterlockedCompareExchange8(char volatile*, char, char);
extern "C" short _InterlockedExchangeAdd8(char volatile*, char);
extern "C" char _InterlockedAnd8(char volatile*, char);
extern "C" char _InterlockedOr8(char volatile*, char);
extern "C" char _InterlockedXor8(char volatile*, char);
extern "C" short _InterlockedIncrement16(short volatile*);
extern "C" short _InterlockedDecrement16(short volatile*);
extern "C" short _InterlockedExchange16(short volatile*, short);
extern "C" short _InterlockedCompareExchange16(short volatile*, short, short);
extern "C" short _InterlockedExchangeAdd16(short volatile*, short);
extern "C" short _InterlockedAnd16(short volatile*, short);
extern "C" short _InterlockedOr16(short volatile*, short);
extern "C" short _InterlockedXor16(short volatile*, short);
extern "C" long _InterlockedIncrement(long volatile*);
extern "C" long _InterlockedDecrement(long volatile*);
extern "C" long _InterlockedExchange(long volatile*, long);
extern "C" long _InterlockedCompareExchange(long volatile*, long, long);
extern "C" long _InterlockedExchangeAdd(long volatile*, long);
extern "C" long _InterlockedAnd(long volatile*, long);
extern "C" long _InterlockedOr(long volatile*, long);
extern "C" long _InterlockedXor(long volatile*, long);
extern "C" long long _InterlockedIncrement64(long long volatile*);
extern "C" long long _InterlockedDecrement64(long long volatile*);
extern "C" long long _InterlockedExchange64(long long volatile*, long long);
extern "C" long long _InterlockedCompareExchange64(long long volatile*, long long, long long);
extern "C" long long _InterlockedExchangeAdd64(long long volatile*, long long);
extern "C" long long _InterlockedAnd64(long long volatile*, long long);
extern "C" long long _InterlockedOr64(long long volatile*, long long);
extern "C" long long _InterlockedXor64(long long volatile*, long long);
extern "C" void* _InterlockedExchangePointer(void* volatile*, void*);
extern "C" void* _InterlockedCompareExchangePointer(void* volatile*, void*, void*);
extern "C" unsigned char _InterlockedCompareExchange128(
    __int64 volatile*, __int64, __int64, __int64*);
#pragma intrinsic(_InterlockedExchange8)
#pragma intrinsic(_InterlockedCompareExchange8)
#pragma intrinsic(_InterlockedAnd8)
#pragma intrinsic(_InterlockedOr8)
#pragma intrinsic(_InterlockedXor8)
#pragma intrinsic(_InterlockedIncrement16)
#pragma intrinsic(_InterlockedDecrement16)
#pragma intrinsic(_InterlockedExchange16)
#pragma intrinsic(_InterlockedCompareExchange16)
#pragma intrinsic(_InterlockedAnd16)
#pragma intrinsic(_InterlockedOr16)
#pragma intrinsic(_InterlockedXor16)
#pragma intrinsic(_InterlockedIncrement)
#pragma intrinsic(_InterlockedDecrement)
#pragma intrinsic(_InterlockedExchange)
#pragma intrinsic(_InterlockedCompareExchange)
#pragma intrinsic(_InterlockedExchangeAdd)
#pragma intrinsic(_InterlockedAnd)
#pragma intrinsic(_InterlockedOr)
#pragma intrinsic(_InterlockedXor)
#pragma intrinsic(_InterlockedIncrement64)
#pragma intrinsic(_InterlockedDecrement64)
#pragma intrinsic(_InterlockedExchange64)
#pragma intrinsic(_InterlockedCompareExchange64)
#pragma intrinsic(_InterlockedExchangeAdd64)
#pragma intrinsic(_InterlockedAnd64)
#pragma intrinsic(_InterlockedOr64)
#pragma intrinsic(_InterlockedXor64)
#pragma intrinsic(_InterlockedExchangePointer)
#pragma intrinsic(_InterlockedCompareExchangePointer)
#pragma intrinsic(_InterlockedCompareExchange128)

#if UTL_ARCH_X86
#  define _InterlockedExchange8_acq _InterlockedExchange8
#  define _InterlockedCompareExchange8_acq _InterlockedCompareExchange8
#  define _InterlockedExchangeAdd8_acq _InterlockedExchangeAdd8
#  define _InterlockedAnd8_acq _InterlockedAnd8
#  define _InterlockedOr8_acq _InterlockedOr8
#  define _InterlockedXor8_acq _InterlockedXor8
#  define _InterlockedIncrement16_acq _InterlockedIncrement16
#  define _InterlockedDecrement16_acq _InterlockedDecrement16
#  define _InterlockedExchange16_acq _InterlockedExchange16
#  define _InterlockedCompareExchange16_acq _InterlockedCompareExchange16
#  define _InterlockedExchangeAdd16_acq _InterlockedExchangeAdd16
#  define _InterlockedAnd16_acq _InterlockedAnd16
#  define _InterlockedOr16_acq _InterlockedOr16
#  define _InterlockedXor16_acq _InterlockedXor16
#  define _InterlockedIncrement_acq _InterlockedIncrement
#  define _InterlockedDecrement_acq _InterlockedDecrement
#  define _InterlockedAdd_acq _InterlockedAdd
#  define _InterlockedExchange_acq _InterlockedExchange
#  define _InterlockedCompareExchange_acq _InterlockedCompareExchange
#  define _InterlockedExchangeAdd_acq _InterlockedExchangeAdd
#  define _InterlockedAnd_acq _InterlockedAnd
#  define _InterlockedOr_acq _InterlockedOr
#  define _InterlockedXor_acq _InterlockedXor
#  define _InterlockedIncrement64_acq _InterlockedIncrement64
#  define _InterlockedDecrement64_acq _InterlockedDecrement64
#  define _InterlockedAdd64_acq _InterlockedAdd64
#  define _InterlockedExchange64_acq _InterlockedExchange64
#  define _InterlockedCompareExchange64_acq _InterlockedCompareExchange64
#  define _InterlockedExchangeAdd64_acq _InterlockedExchangeAdd64
#  define _InterlockedAnd64_acq _InterlockedAnd64
#  define _InterlockedOr64_acq _InterlockedOr64
#  define _InterlockedXor64_acq _InterlockedXor64
#  define _InterlockedExchangePointer_acq _InterlockedExchangePointer
#  define _InterlockedCompareExchangePointer_acq _InterlockedCompareExchangePointer
#  define _InterlockedCompareExchange128_acq _InterlockedCompareExchange128
#  define _InterlockedExchange8_rel _InterlockedExchange8
#  define _InterlockedCompareExchange8_rel _InterlockedCompareExchange8
#  define _InterlockedExchangeAdd8_rel _InterlockedExchangeAdd8
#  define _InterlockedAnd8_rel _InterlockedAnd8
#  define _InterlockedOr8_rel _InterlockedOr8
#  define _InterlockedXor8_rel _InterlockedXor8
#  define _InterlockedIncrement16_rel _InterlockedIncrement16
#  define _InterlockedDecrement16_rel _InterlockedDecrement16
#  define _InterlockedExchange16_rel _InterlockedExchange16
#  define _InterlockedCompareExchange16_rel _InterlockedCompareExchange16
#  define _InterlockedExchangeAdd16_rel _InterlockedExchangeAdd16
#  define _InterlockedAnd16_rel _InterlockedAnd16
#  define _InterlockedOr16_rel _InterlockedOr16
#  define _InterlockedXor16_rel _InterlockedXor16
#  define _InterlockedIncrement_rel _InterlockedIncrement
#  define _InterlockedDecrement_rel _InterlockedDecrement
#  define _InterlockedAdd_rel _InterlockedAdd
#  define _InterlockedExchange_rel _InterlockedExchange
#  define _InterlockedCompareExchange_rel _InterlockedCompareExchange
#  define _InterlockedExchangeAdd_rel _InterlockedExchangeAdd
#  define _InterlockedAnd_rel _InterlockedAnd
#  define _InterlockedOr_rel _InterlockedOr
#  define _InterlockedXor_rel _InterlockedXor
#  define _InterlockedIncrement64_rel _InterlockedIncrement64
#  define _InterlockedDecrement64_rel _InterlockedDecrement64
#  define _InterlockedAdd64_rel _InterlockedAdd64
#  define _InterlockedExchange64_rel _InterlockedExchange64
#  define _InterlockedCompareExchange64_rel _InterlockedCompareExchange64
#  define _InterlockedExchangeAdd64_rel _InterlockedExchangeAdd64
#  define _InterlockedAnd64_rel _InterlockedAnd64
#  define _InterlockedOr64_rel _InterlockedOr64
#  define _InterlockedXor64_rel _InterlockedXor64
#  define _InterlockedExchangePointer_rel _InterlockedExchangePointer
#  define _InterlockedCompareExchangePointer_rel _InterlockedCompareExchangePointer
#  define _InterlockedCompareExchange128_rel _InterlockedCompareExchange128
#  define _InterlockedExchange8_nf _InterlockedExchange8
#  define _InterlockedCompareExchange8_nf _InterlockedCompareExchange8
#  define _InterlockedExchangeAdd8_nf _InterlockedExchangeAdd8
#  define _InterlockedAnd8_nf _InterlockedAnd8
#  define _InterlockedOr8_nf _InterlockedOr8
#  define _InterlockedXor8_nf _InterlockedXor8
#  define _InterlockedIncrement16_nf _InterlockedIncrement16
#  define _InterlockedDecrement16_nf _InterlockedDecrement16
#  define _InterlockedExchange16_nf _InterlockedExchange16
#  define _InterlockedCompareExchange16_nf _InterlockedCompareExchange16
#  define _InterlockedExchangeAdd16_nf _InterlockedExchangeAdd16
#  define _InterlockedAnd16_nf _InterlockedAnd16
#  define _InterlockedOr16_nf _InterlockedOr16
#  define _InterlockedXor16_nf _InterlockedXor16
#  define _InterlockedIncrement_nf _InterlockedIncrement
#  define _InterlockedDecrement_nf _InterlockedDecrement
#  define _InterlockedAdd_nf _InterlockedAdd
#  define _InterlockedExchange_nf _InterlockedExchange
#  define _InterlockedCompareExchange_nf _InterlockedCompareExchange
#  define _InterlockedExchangeAdd_nf _InterlockedExchangeAdd
#  define _InterlockedAnd_nf _InterlockedAnd
#  define _InterlockedOr_nf _InterlockedOr
#  define _InterlockedXor_nf _InterlockedXor
#  define _InterlockedIncrement64_nf _InterlockedIncrement64
#  define _InterlockedDecrement64_nf _InterlockedDecrement64
#  define _InterlockedAdd64_nf _InterlockedAdd64
#  define _InterlockedExchange64_nf _InterlockedExchange64
#  define _InterlockedCompareExchange64_nf _InterlockedCompareExchange64
#  define _InterlockedExchangeAdd64_nf _InterlockedExchangeAdd64
#  define _InterlockedAnd64_nf _InterlockedAnd64
#  define _InterlockedOr64_nf _InterlockedOr64
#  define _InterlockedXor64_nf _InterlockedXor64
#  define _InterlockedExchangePointer_nf _InterlockedExchangePointer
#  define _InterlockedCompareExchangePointer_nf _InterlockedCompareExchangePointer
#  define _InterlockedCompareExchange128_nf _InterlockedCompareExchange128

#elif UTL_ARCH_ARM
extern "C" long _InterlockedAdd(long volatile*, long);
extern "C" long long _InterlockedAdd64(long long volatile*, long long);
extern "C" char _InterlockedExchange8_acq(char volatile*, char);
extern "C" char _InterlockedCompareExchange8_acq(char volatile*, char, char);
extern "C" short _InterlockedExchangeAdd8_acq(char volatile*, char);
extern "C" char _InterlockedAnd8_acq(char volatile*, char);
extern "C" char _InterlockedOr8_acq(char volatile*, char);
extern "C" char _InterlockedXor8_acq(char volatile*, char);
extern "C" short _InterlockedIncrement16_acq(short volatile*);
extern "C" short _InterlockedDecrement16_acq(short volatile*);
extern "C" short _InterlockedExchange16_acq(short volatile*, short);
extern "C" short _InterlockedCompareExchange16_acq(short volatile*, short, short);
extern "C" short _InterlockedExchangeAdd16_acq(short volatile*, short);
extern "C" short _InterlockedAnd16_acq(short volatile*, short);
extern "C" short _InterlockedOr16_acq(short volatile*, short);
extern "C" short _InterlockedXor16_acq(short volatile*, short);
extern "C" long _InterlockedIncrement_acq(long volatile*);
extern "C" long _InterlockedDecrement_acq(long volatile*);
extern "C" long _InterlockedAdd_acq(long volatile*, long);
extern "C" long _InterlockedExchange_acq(long volatile*, long);
extern "C" long _InterlockedCompareExchange_acq(long volatile*, long, long);
extern "C" long _InterlockedExchangeAdd_acq(long volatile*, long);
extern "C" long _InterlockedAnd_acq(long volatile*, long);
extern "C" long _InterlockedOr_acq(long volatile*, long);
extern "C" long _InterlockedXor_acq(long volatile*, long);
extern "C" long long _InterlockedIncrement64_acq(long long volatile*);
extern "C" long long _InterlockedDecrement64_acq(long long volatile*);
extern "C" long long _InterlockedAdd64_acq(long long volatile*, long long);
extern "C" long long _InterlockedExchange64_acq(long long volatile*, long long);
extern "C" long long _InterlockedCompareExchange64_acq(long long volatile*, long long, long long);
extern "C" long long _InterlockedExchangeAdd64_acq(long long volatile*, long long);
extern "C" long long _InterlockedAnd64_acq(long long volatile*, long long);
extern "C" long long _InterlockedOr64_acq(long long volatile*, long long);
extern "C" long long _InterlockedXor64_acq(long long volatile*, long long);
extern "C" void* _InterlockedExchangePointer_acq(void* volatile*, void*);
extern "C" void* _InterlockedCompareExchangePointer_acq(void* volatile*, void*, void*);
extern "C" unsigned char _InterlockedCompareExchange128_acq(
    __int64 volatile*, __int64, __int64, __int64*);
extern "C" char _InterlockedExchange8_rel(char volatile*, char);
extern "C" char _InterlockedCompareExchange8_rel(char volatile*, char, char);
extern "C" short _InterlockedExchangeAdd8_rel(char volatile*, char);
extern "C" char _InterlockedAnd8_rel(char volatile*, char);
extern "C" char _InterlockedOr8_rel(char volatile*, char);
extern "C" char _InterlockedXor8_rel(char volatile*, char);
extern "C" short _InterlockedIncrement16_rel(short volatile*);
extern "C" short _InterlockedDecrement16_rel(short volatile*);
extern "C" short _InterlockedExchange16_rel(short volatile*, short);
extern "C" short _InterlockedCompareExchange16_rel(short volatile*, short, short);
extern "C" short _InterlockedExchangeAdd16_rel(short volatile*, short);
extern "C" short _InterlockedAnd16_rel(short volatile*, short);
extern "C" short _InterlockedOr16_rel(short volatile*, short);
extern "C" short _InterlockedXor16_rel(short volatile*, short);
extern "C" long _InterlockedIncrement_rel(long volatile*);
extern "C" long _InterlockedDecrement_rel(long volatile*);
extern "C" long _InterlockedAdd_rel(long volatile*, long);
extern "C" long _InterlockedExchange_rel(long volatile*, long);
extern "C" long _InterlockedCompareExchange_rel(long volatile*, long, long);
extern "C" long _InterlockedExchangeAdd_rel(long volatile*, long);
extern "C" long _InterlockedAnd_rel(long volatile*, long);
extern "C" long _InterlockedOr_rel(long volatile*, long);
extern "C" long _InterlockedXor_rel(long volatile*, long);
extern "C" long long _InterlockedIncrement64_rel(long long volatile*);
extern "C" long long _InterlockedDecrement64_rel(long long volatile*);
extern "C" long long _InterlockedAdd64_rel(long long volatile*, long long);
extern "C" long long _InterlockedExchange64_rel(long long volatile*, long long);
extern "C" long long _InterlockedCompareExchange64_rel(long long volatile*, long long, long long);
extern "C" long long _InterlockedExchangeAdd64_rel(long long volatile*, long long);
extern "C" long long _InterlockedAnd64_rel(long long volatile*, long long);
extern "C" long long _InterlockedOr64_rel(long long volatile*, long long);
extern "C" long long _InterlockedXor64_rel(long long volatile*, long long);
extern "C" void* _InterlockedExchangePointer_rel(void* volatile*, void*);
extern "C" void* _InterlockedCompareExchangePointer_rel(void* volatile*, void*, void*);
extern "C" unsigned char _InterlockedCompareExchange128_rel(
    __int64 volatile*, __int64, __int64, __int64*);
extern "C" char _InterlockedExchange8_nf(char volatile*, char);
extern "C" char _InterlockedCompareExchange8_nf(char volatile*, char, char);
extern "C" short _InterlockedExchangeAdd8_nf(char volatile*, char);
extern "C" char _InterlockedAnd8_nf(char volatile*, char);
extern "C" char _InterlockedOr8_nf(char volatile*, char);
extern "C" char _InterlockedXor8_nf(char volatile*, char);
extern "C" short _InterlockedIncrement16_nf(short volatile*);
extern "C" short _InterlockedDecrement16_nf(short volatile*);
extern "C" short _InterlockedExchange16_nf(short volatile*, short);
extern "C" short _InterlockedCompareExchange16_nf(short volatile*, short, short);
extern "C" short _InterlockedExchangeAdd16_nf(short volatile*, short);
extern "C" short _InterlockedAnd16_nf(short volatile*, short);
extern "C" short _InterlockedOr16_nf(short volatile*, short);
extern "C" short _InterlockedXor16_nf(short volatile*, short);
extern "C" long _InterlockedIncrement_nf(long volatile*);
extern "C" long _InterlockedDecrement_nf(long volatile*);
extern "C" long _InterlockedAdd_nf(long volatile*, long);
extern "C" long _InterlockedExchange_nf(long volatile*, long);
extern "C" long _InterlockedCompareExchange_nf(long volatile*, long, long);
extern "C" long _InterlockedExchangeAdd_nf(long volatile*, long);
extern "C" long _InterlockedAnd_nf(long volatile*, long);
extern "C" long _InterlockedOr_nf(long volatile*, long);
extern "C" long _InterlockedXor_nf(long volatile*, long);
extern "C" long long _InterlockedIncrement64_nf(long long volatile*);
extern "C" long long _InterlockedDecrement64_nf(long long volatile*);
extern "C" long long _InterlockedAdd64_nf(long long volatile*, long long);
extern "C" long long _InterlockedExchange64_nf(long long volatile*, long long);
extern "C" long long _InterlockedCompareExchange64_nf(long long volatile*, long long, long long);
extern "C" long long _InterlockedExchangeAdd64_nf(long long volatile*, long long);
extern "C" long long _InterlockedAnd64_nf(long long volatile*, long long);
extern "C" long long _InterlockedOr64_nf(long long volatile*, long long);
extern "C" long long _InterlockedXor64_nf(long long volatile*, long long);
extern "C" void* _InterlockedExchangePointer_nf(void* volatile*, void*);
extern "C" void* _InterlockedCompareExchangePointer_nf(void* volatile*, void*, void*);
extern "C" unsigned char _InterlockedCompareExchange128_nf(
    __int64 volatile*, __int64, __int64, __int64*);
#  pragma intrinsic(_InterlockedAdd)
#  pragma intrinsic(_InterlockedAdd64)
#  pragma intrinsic(_InterlockedExchange8_acq)
#  pragma intrinsic(_InterlockedCompareExchange8_acq)
#  pragma intrinsic(_InterlockedExchangeAdd8_acq)
#  pragma intrinsic(_InterlockedAnd8_acq)
#  pragma intrinsic(_InterlockedOr8_acq)
#  pragma intrinsic(_InterlockedXor8_acq)
#  pragma intrinsic(_InterlockedIncrement16_acq)
#  pragma intrinsic(_InterlockedDecrement16_acq)
#  pragma intrinsic(_InterlockedExchange16_acq)
#  pragma intrinsic(_InterlockedCompareExchange16_acq)
#  pragma intrinsic(_InterlockedExchangeAdd16_acq)
#  pragma intrinsic(_InterlockedAnd16_acq)
#  pragma intrinsic(_InterlockedOr16_acq)
#  pragma intrinsic(_InterlockedXor16_acq)
#  pragma intrinsic(_InterlockedIncrement_acq)
#  pragma intrinsic(_InterlockedDecrement_acq)
#  pragma intrinsic(_InterlockedAdd_acq)
#  pragma intrinsic(_InterlockedExchange_acq)
#  pragma intrinsic(_InterlockedCompareExchange_acq)
#  pragma intrinsic(_InterlockedExchangeAdd_acq)
#  pragma intrinsic(_InterlockedAnd_acq)
#  pragma intrinsic(_InterlockedOr_acq)
#  pragma intrinsic(_InterlockedXor_acq)
#  pragma intrinsic(_InterlockedIncrement64_acq)
#  pragma intrinsic(_InterlockedDecrement64_acq)
#  pragma intrinsic(_InterlockedAdd64_acq)
#  pragma intrinsic(_InterlockedExchange64_acq)
#  pragma intrinsic(_InterlockedCompareExchange64_acq)
#  pragma intrinsic(_InterlockedExchangeAdd64_acq)
#  pragma intrinsic(_InterlockedAnd64_acq)
#  pragma intrinsic(_InterlockedOr64_acq)
#  pragma intrinsic(_InterlockedXor64_acq)
#  pragma intrinsic(_InterlockedExchangePointer_acq)
#  pragma intrinsic(_InterlockedCompareExchangePointer_acq)
#  pragma intrinsic(_InterlockedCompareExchange128_acq)
#  pragma intrinsic(_InterlockedExchange8_rel)
#  pragma intrinsic(_InterlockedCompareExchange8_rel)
#  pragma intrinsic(_InterlockedExchangeAdd8_rel)
#  pragma intrinsic(_InterlockedAnd8_rel)
#  pragma intrinsic(_InterlockedOr8_rel)
#  pragma intrinsic(_InterlockedXor8_rel)
#  pragma intrinsic(_InterlockedIncrement16_rel)
#  pragma intrinsic(_InterlockedDecrement16_rel)
#  pragma intrinsic(_InterlockedExchange16_rel)
#  pragma intrinsic(_InterlockedCompareExchange16_rel)
#  pragma intrinsic(_InterlockedExchangeAdd16_rel)
#  pragma intrinsic(_InterlockedAnd16_rel)
#  pragma intrinsic(_InterlockedOr16_rel)
#  pragma intrinsic(_InterlockedXor16_rel)
#  pragma intrinsic(_InterlockedIncrement_rel)
#  pragma intrinsic(_InterlockedDecrement_rel)
#  pragma intrinsic(_InterlockedAdd_rel)
#  pragma intrinsic(_InterlockedExchange_rel)
#  pragma intrinsic(_InterlockedCompareExchange_rel)
#  pragma intrinsic(_InterlockedExchangeAdd_rel)
#  pragma intrinsic(_InterlockedAnd_rel)
#  pragma intrinsic(_InterlockedOr_rel)
#  pragma intrinsic(_InterlockedXor_rel)
#  pragma intrinsic(_InterlockedIncrement64_rel)
#  pragma intrinsic(_InterlockedDecrement64_rel)
#  pragma intrinsic(_InterlockedAdd64_rel)
#  pragma intrinsic(_InterlockedExchange64_rel)
#  pragma intrinsic(_InterlockedCompareExchange64_rel)
#  pragma intrinsic(_InterlockedExchangeAdd64_rel)
#  pragma intrinsic(_InterlockedAnd64_rel)
#  pragma intrinsic(_InterlockedOr64_rel)
#  pragma intrinsic(_InterlockedXor64_rel)
#  pragma intrinsic(_InterlockedExchangePointer_rel)
#  pragma intrinsic(_InterlockedCompareExchangePointer_rel)
#  pragma intrinsic(_InterlockedCompareExchange128_rel)
#  pragma intrinsic(_InterlockedExchange8_nf)
#  pragma intrinsic(_InterlockedCompareExchange8_nf)
#  pragma intrinsic(_InterlockedExchangeAdd8_nf)
#  pragma intrinsic(_InterlockedAnd8_nf)
#  pragma intrinsic(_InterlockedOr8_nf)
#  pragma intrinsic(_InterlockedXor8_nf)
#  pragma intrinsic(_InterlockedIncrement16_nf)
#  pragma intrinsic(_InterlockedDecrement16_nf)
#  pragma intrinsic(_InterlockedExchange16_nf)
#  pragma intrinsic(_InterlockedCompareExchange16_nf)
#  pragma intrinsic(_InterlockedExchangeAdd16_nf)
#  pragma intrinsic(_InterlockedAnd16_nf)
#  pragma intrinsic(_InterlockedOr16_nf)
#  pragma intrinsic(_InterlockedXor16_nf)
#  pragma intrinsic(_InterlockedIncrement_nf)
#  pragma intrinsic(_InterlockedDecrement_nf)
#  pragma intrinsic(_InterlockedAdd_nf)
#  pragma intrinsic(_InterlockedExchange_nf)
#  pragma intrinsic(_InterlockedCompareExchange_nf)
#  pragma intrinsic(_InterlockedExchangeAdd_nf)
#  pragma intrinsic(_InterlockedAnd_nf)
#  pragma intrinsic(_InterlockedOr_nf)
#  pragma intrinsic(_InterlockedXor_nf)
#  pragma intrinsic(_InterlockedIncrement64_nf)
#  pragma intrinsic(_InterlockedDecrement64_nf)
#  pragma intrinsic(_InterlockedAdd64_nf)
#  pragma intrinsic(_InterlockedExchange64_nf)
#  pragma intrinsic(_InterlockedCompareExchange64_nf)
#  pragma intrinsic(_InterlockedExchangeAdd64_nf)
#  pragma intrinsic(_InterlockedAnd64_nf)
#  pragma intrinsic(_InterlockedOr64_nf)
#  pragma intrinsic(_InterlockedXor64_nf)
#  pragma intrinsic(_InterlockedExchangePointer_nf)
#  pragma intrinsic(_InterlockedCompareExchangePointer_nf)
#  pragma intrinsic(_InterlockedCompareExchange128_nf)
#else
#  error "Unsupported architecture"
#endif
