// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

#include "utl/string/utl_basic_short_string.h"

// clang-format off
#define UTL_STRING_ABI_TEMPLATE_LIST(_Func, _CharType) \
  _Func(__UTL_ABI_PUBLIC basic_string<_CharType>::heap_type clone_heap(allocator_type&, basic_string<_CharType> const&)) \
  _Func(__UTL_ABI_PUBLIC void basic_string<_CharType>::destroy())\
  _Func(__UTL_ABI_PUBLIC void basic_string<_CharType>::reserve_impl(size_type)) \
  _Func(__UTL_ABI_PUBLIC void basic_string<_CharType>::grow_heap(size_type)) \
  _Func(__UTL_ABI_PUBLIC void basic_string<_CharType>::transfer_to_heap(size_type)) \
  _Func(__UTL_ABI_PUBLIC basic_string<_CharType>& basic_string<_CharType>::replace(size_type, size_type, const_pointer, size_type)) \
  _Func(__UTL_ABI_PUBLIC basic_string<_CharType>::size_type basic_string<_CharType>::rfind(const_pointer, size_type, size_type) const) \
  _Func(__UTL_ABI_PUBLIC basic_string<_CharType>::basic_string(basic_string const&)) \
  _Func(__UTL_ABI_PUBLIC basic_string<_CharType>& basic_string<_CharType>::replace(size_type, size_type, const_pointer)) \
  _Func(__UTL_ABI_PUBLIC basic_string<_CharType>::basic_string(basic_string const&, allocator<_CharType> const&)) \
  _Func(__UTL_ABI_PUBLIC basic_string<_CharType>::size_type basic_string<_CharType>::find_last_not_of(const_pointer, size_type, size_type) const) \
  _Func(__UTL_ABI_PUBLIC basic_string<_CharType>::~basic_string()) \
  _Func(__UTL_ABI_PUBLIC basic_string<_CharType>::size_type basic_string<_CharType>::find_first_not_of(const_pointer, size_type, size_type) const) \
  _Func(__UTL_ABI_PUBLIC basic_string<_CharType>& basic_string<_CharType>::insert(size_type, size_type, value_type)) \
  _Func(__UTL_ABI_PUBLIC basic_string<_CharType>& basic_string<_CharType>::operator=(value_type)) \
  _Func(__UTL_ABI_PUBLIC const _CharType& basic_string<_CharType>::at(size_type) const) \
  _Func(__UTL_ABI_PUBLIC basic_string<_CharType>& basic_string<_CharType>::insert(size_type, const_pointer, size_type)) \
  _Func(__UTL_ABI_PUBLIC basic_string<_CharType>::size_type basic_string<_CharType>::find_first_of(const_pointer, size_type, size_type) const) \
  _Func(__UTL_ABI_PUBLIC basic_string<_CharType>& basic_string<_CharType>::replace(size_type, size_type, size_type, value_type)) \
  _Func(__UTL_ABI_PUBLIC basic_string<_CharType>& basic_string<_CharType>::assign(const_pointer, size_type)) \
  _Func(__UTL_ABI_PUBLIC void basic_string<_CharType>::reserve(size_type)) \
  _Func(__UTL_ABI_PUBLIC basic_string<_CharType>& basic_string<_CharType>::append(const_pointer, size_type)) \
  _Func(__UTL_ABI_PUBLIC basic_string<_CharType>& basic_string<_CharType>::assign(basic_string const&, size_type, size_type)) \
  _Func(__UTL_ABI_PUBLIC basic_string<_CharType>::size_type basic_string<_CharType>::copy(value_type*, size_type, size_type) const) \
  _Func(__UTL_ABI_PUBLIC basic_string<_CharType>::basic_string(basic_string const&, size_type, size_type, allocator<_CharType> const&)) \
  _Func(__UTL_ABI_PUBLIC basic_string<_CharType>::size_type basic_string<_CharType>::find(value_type, size_type) const) \
  _Func(__UTL_ABI_PUBLIC basic_string<_CharType>& basic_string<_CharType>::insert(size_type, const_pointer)) \
  _Func(__UTL_ABI_PUBLIC basic_string<_CharType>::size_type basic_string<_CharType>::find_last_of(const_pointer, size_type, size_type) const) \
  _Func(__UTL_ABI_PUBLIC void basic_string<_CharType>::push_back(value_type)) \
  _Func(__UTL_ABI_PUBLIC basic_string<_CharType>& basic_string<_CharType>::append(size_type, value_type)) \
  _Func(__UTL_ABI_PUBLIC basic_string<_CharType>::size_type basic_string<_CharType>::rfind(value_type, size_type) const) \
  _Func(__UTL_ABI_PUBLIC const basic_string<_CharType>::size_type basic_string<_CharType>::npos) \
  _Func(__UTL_ABI_PUBLIC basic_string<_CharType>& basic_string<_CharType>::assign(size_type, value_type)) \
  _Func(__UTL_ABI_PUBLIC basic_string<_CharType>& basic_string<_CharType>::erase(size_type, size_type)) \
  _Func(__UTL_ABI_PUBLIC basic_string<_CharType>& basic_string<_CharType>::append(basic_string const&, size_type, size_type)) \
  _Func(__UTL_ABI_PUBLIC int basic_string<_CharType>::compare(const_pointer) const) \
  _Func(__UTL_ABI_PUBLIC int basic_string<_CharType>::compare(size_type, size_type, const_pointer) const) \
  _Func(__UTL_ABI_PUBLIC _CharType& basic_string<_CharType>::at(size_type)) \
  _Func(__UTL_ABI_PUBLIC basic_string<_CharType>& basic_string<_CharType>::assign(const_pointer)) \
  _Func(__UTL_ABI_PUBLIC basic_string<_CharType>::size_type basic_string<_CharType>::find(const_pointer, size_type, size_type) const) \
  _Func(__UTL_ABI_PUBLIC int basic_string<_CharType>::compare(size_type, size_type, basic_string const&, size_type, size_type) const) \
  _Func(__UTL_ABI_PUBLIC int basic_string<_CharType>::compare(size_type, size_type, const_pointer, size_type) const) \
  _Func(__UTL_ABI_PUBLIC basic_string<_CharType>& basic_string<_CharType>::operator=(basic_string const&)) \
  _Func(__UTL_ABI_PUBLIC basic_string<_CharType>& basic_string<_CharType>::append(const_pointer)) \
  _Func(__UTL_ABI_PUBLIC basic_string<_CharType>& basic_string<_CharType>::replace(size_type, size_type, basic_string const&, size_type, size_type)) \
  _Func(__UTL_ABI_PUBLIC basic_string<_CharType>::iterator basic_string<_CharType>::insert(basic_string::const_iterator, value_type)) \
  _Func(__UTL_ABI_PUBLIC void basic_string<_CharType>::resize(size_type, value_type)) \
  _Func(__UTL_ABI_PUBLIC basic_string<_CharType>& basic_string<_CharType>::insert(size_type, basic_string const&, size_type, size_type))
// clang-format on
