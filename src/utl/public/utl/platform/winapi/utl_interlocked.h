// Copyright 2023-2024 Bryan Wong

#pragma once

#if !defined(UTL_PLATFORM_ATOMIC_PRIVATE_HEADER_GUARD)
#  error "Private header accessed"
#endif

#include "utl/utl_config.h"

#if !UTL_COMPILER_MSVC
#  error Invalid compiler
#endif

extern "C" char _InterlockedExchange8(char volatile*, char);
extern "C" char _InterlockedCompareExchange8(char volatile*, char, char);
extern "C" char _InterlockedAnd8(char volatile*, char);
extern "C" char _InterlockedOr8(char volatile*, char);
extern "C" char _InterlockedXor8(char volatile*, char);
extern "C" short _InterlockedIncrement16(short volatile*);
extern "C" short _InterlockedDecrement16(short volatile*);
extern "C" short _InterlockedExchange16(short volatile*, short);
extern "C" short _InterlockedCompareExchange16(short volatile*, short, short);
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

#if UTL_ARCH_ARM
extern "C" long _InterlockedAdd(long volatile*, long);
extern "C" long long _InterlockedAdd64(long long volatile*, long long);
extern "C" char _InterlockedExchange8_acq(char volatile*, char);
extern "C" char _InterlockedCompareExchange8_acq(char volatile*, char, char);
extern "C" char _InterlockedAnd8_acq(char volatile*, char);
extern "C" char _InterlockedOr8_acq(char volatile*, char);
extern "C" char _InterlockedXor8_acq(char volatile*, char);
extern "C" short _InterlockedIncrement16_acq(short volatile*);
extern "C" short _InterlockedDecrement16_acq(short volatile*);
extern "C" short _InterlockedExchange16_acq(short volatile*, short);
extern "C" short _InterlockedCompareExchange16_acq(short volatile*, short, short);
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
extern "C" char _InterlockedAnd8_rel(char volatile*, char);
extern "C" char _InterlockedOr8_rel(char volatile*, char);
extern "C" char _InterlockedXor8_rel(char volatile*, char);
extern "C" short _InterlockedIncrement16_rel(short volatile*);
extern "C" short _InterlockedDecrement16_rel(short volatile*);
extern "C" short _InterlockedExchange16_rel(short volatile*, short);
extern "C" short _InterlockedCompareExchange16_rel(short volatile*, short, short);
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
extern "C" char _InterlockedAnd8_nf(char volatile*, char);
extern "C" char _InterlockedOr8_nf(char volatile*, char);
extern "C" char _InterlockedXor8_nf(char volatile*, char);
extern "C" short _InterlockedIncrement16_nf(short volatile*);
extern "C" short _InterlockedDecrement16_nf(short volatile*);
extern "C" short _InterlockedExchange16_nf(short volatile*, short);
extern "C" short _InterlockedCompareExchange16_nf(short volatile*, short, short);
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
#  pragma intrinsic(_InterlockedAnd8_acq)
#  pragma intrinsic(_InterlockedOr8_acq)
#  pragma intrinsic(_InterlockedXor8_acq)
#  pragma intrinsic(_InterlockedIncrement16_acq)
#  pragma intrinsic(_InterlockedDecrement16_acq)
#  pragma intrinsic(_InterlockedExchange16_acq)
#  pragma intrinsic(_InterlockedCompareExchange16_acq)
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
#  pragma intrinsic(_InterlockedAnd8_rel)
#  pragma intrinsic(_InterlockedOr8_rel)
#  pragma intrinsic(_InterlockedXor8_rel)
#  pragma intrinsic(_InterlockedIncrement16_rel)
#  pragma intrinsic(_InterlockedDecrement16_rel)
#  pragma intrinsic(_InterlockedExchange16_rel)
#  pragma intrinsic(_InterlockedCompareExchange16_rel)
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
#  pragma intrinsic(_InterlockedAnd8_nf)
#  pragma intrinsic(_InterlockedOr8_nf)
#  pragma intrinsic(_InterlockedXor8_nf)
#  pragma intrinsic(_InterlockedIncrement16_nf)
#  pragma intrinsic(_InterlockedDecrement16_nf)
#  pragma intrinsic(_InterlockedExchange16_nf)
#  pragma intrinsic(_InterlockedCompareExchange16_nf)
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
#endif
