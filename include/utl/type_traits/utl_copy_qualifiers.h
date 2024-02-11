// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_namespace.h"

UTL_NAMESPACE_BEGIN

template <typename From, typename To> struct copy_const { using type = To; };
template <typename From, typename To> struct copy_const<From const, To>  { using type = To const; };
template <typename From, typename To> struct copy_const<From const volatile, To>  { using type = To const; };

template <typename From, typename To> struct copy_volatile { using type = To; };
template <typename From, typename To> struct copy_volatile<From volatile, To>  { using type = To volatile; };
template <typename From, typename To> struct copy_volatile<From const volatile, To>  { using type = To volatile; };

template <typename From, typename To> struct copy_cv { using type = To; };
template <typename From, typename To> struct copy_cv<From const, To>  { using type = To const; };
template <typename From, typename To> struct copy_cv<From volatile, To>  { using type = To volatile; };
template <typename From, typename To> struct copy_cv<From const volatile, To>  { using type = To const volatile; };

template <typename From, typename To> struct copy_reference { using type = To; };
template <typename From, typename To> struct copy_reference<From&&, To>  { using type = To&&; };
template <typename From, typename To> struct copy_reference<From&, To>  { using type = To&; };

template <typename From, typename To> struct copy_extent { using type = To; };
template <typename From, typename To> struct copy_extent<From[], To> { using type = To[]; };
template <typename From, typename To, size_t N> struct copy_extent<From[N], To> { using type = To[N]; };

template <typename From, typename To> struct copy_all_extents { using type = To; };
template <typename From, typename To> struct copy_all_extents<From[], To> : copy_all_extents<From, To[]> {};
template <typename From, typename To, size_t N> struct copy_all_extents<From[N], To> : copy_all_extents<From, To[N]> {};

template <typename T> struct remove_all_pointers { using type = T; };
template <typename T> struct remove_all_pointers<T*> : remove_all_pointers<T> {};

template <typename From, typename To>  struct copy_pointer { using type = To; };
template <typename From, typename To>  struct copy_pointer<From*, To>  { using type = To*; };

template <typename From, typename To> struct copy_all_pointers { using type = To; };
template <typename From, typename To> struct copy_all_pointers<From*, To> : copy_all_pointers<From, To*> {};

template <typename From, typename To> struct copy_cvref : copy_cv<From, To> {};
template <typename From, typename To> struct copy_cvref<From&, To> { using type = typename copy_cv<From, To>::type&; };
template <typename From, typename To> struct copy_cvref<From&&, To> { using type = typename copy_cv<From, To>::type&&; };

template <typename F, typename T> using copy_const_t = typename copy_const<F, T>::type;
template <typename F, typename T> using copy_volatile_t = typename copy_volatile<F, T>::type;
template <typename F, typename T> using copy_cv_t = typename copy_cv<F, T>::type;
template <typename F, typename T> using copy_reference_t = typename copy_reference<F, T>::type;
template <typename F, typename T> using copy_extent_t = typename copy_extent<F, T>::type;
template <typename F, typename T> using copy_all_extents_t = typename copy_all_extents<F, T>::type;
template <typename F, typename T> using copy_pointer_t = typename copy_pointer<F, T>::type;
template <typename F, typename T> using copy_all_pointers_t = typename copy_all_pointers<F, T>::type;
template <typename F, typename T> using copy_cvref_t = typename copy_cvref<F, T>::type;

UTL_NAMESPACE_END
