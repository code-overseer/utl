// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_config.h"
#include "utl/type_traits/utl_modify_cv.h"
#include <type_traits>

UTL_NAMESPACE_BEGIN

using ::std::remove_reference;
using ::std::add_lvalue_reference;
using ::std::add_rvalue_reference;
using ::std::remove_pointer;
using ::std::add_pointer;

using ::std::make_signed;
using ::std::make_unsigned;

using ::std::remove_extent;
using ::std::remove_all_extents;

using ::std::decay;

template<typename T>
struct remove_cvref : remove_cv<typename remove_reference<T>::type> {};

template<typename T> using remove_reference_t = typename remove_reference<T>::type;
template<typename T> using add_lvalue_reference_t = typename add_lvalue_reference<T>::type;
template<typename T> using add_rvalue_reference_t = typename add_rvalue_reference<T>::type;
template<typename T> using remove_pointer_t = typename remove_pointer<T>::type;
template<typename T> using add_pointer_t = typename add_pointer<T>::type;

template<typename T> using make_signed_t = typename make_signed<T>::type;
template<typename T> using make_unsigned_t = typename make_unsigned<T>::type;

template<typename T> using remove_extent_t = typename remove_extent<T>::type;
template<typename T> using remove_all_extents_t = typename remove_all_extents<T>::type;

template<typename T> using decay_t = typename decay<T>::type;

template<typename T> using remove_cvref_t = typename remove_cvref<T>::type;

UTL_NAMESPACE_END

#define UTL_TRAIT_SUPPORTED_remove_reference 1
#define UTL_TRAIT_SUPPORTED_add_lvalue_reference 1
#define UTL_TRAIT_SUPPORTED_add_rvalue_reference 1
#define UTL_TRAIT_SUPPORTED_remove_pointer 1
#define UTL_TRAIT_SUPPORTED_add_pointer 1

#define UTL_TRAIT_SUPPORTED_make_signed 1
#define UTL_TRAIT_SUPPORTED_make_unsigned 1

#define UTL_TRAIT_SUPPORTED_remove_extent 1
#define UTL_TRAIT_SUPPORTED_remove_all_extents 1

#define UTL_TRAIT_SUPPORTED_decay 1
#define UTL_TRAIT_SUPPORTED_remove_cvref 1

