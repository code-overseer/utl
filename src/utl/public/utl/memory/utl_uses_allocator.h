// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/concepts/utl_same_as.h"
#include "utl/preprocessor/utl_namespace.h"
#include "utl/preprocessor/utl_standard.h"
#include "utl/type_traits/utl_enable_if.h"
#include "utl/type_traits/utl_is_constructible.h"
#include "utl/type_traits/utl_is_explicit_constructible.h"
#include "utl/type_traits/utl_is_nothrow_constructible.h"
#include "utl/type_traits/utl_is_same.h"
#include "utl/type_traits/utl_logical_traits.h"

UTL_STD_NAMESPACE_BEGIN
/* UTL_UNDEFINED_BEHAVIOUR */
template <typename T, typename Alloc>
struct uses_allocator;

struct allocator_arg_t;
UTL_STD_NAMESPACE_END

UTL_NAMESPACE_BEGIN

namespace details {
namespace uses_allocator {

template <typename T, typename Alloc, size_t = sizeof(::std::uses_allocator<T, Alloc>)>
::std::uses_allocator<T, Alloc> resolver(int);

template <typename T, typename Alloc,
    typename R = is_convertible<Alloc, typename T::allocator_type>>
R resolver(float);

template <typename T, typename Alloc>
false_type resolver(...);

template <typename T, typename Alloc, typename R = decltype(resolver<T, Alloc>(0))>
using impl = R;

} // namespace uses_allocator
} // namespace details

struct allocator_arg_t {
    explicit constexpr allocator_arg_t() noexcept = default;
    template <typename T UTL_REQUIRES_CXX11(UTL_TRAIT_VALUE(is_same, T, ::std::allocator_arg_t))>
    UTL_REQUIRES_CXX20(same_as<T, ::std::allocator_arg_t>)
    constexpr allocator_arg_t(T) noexcept {}
    template <typename T UTL_REQUIRES_CXX11(UTL_TRAIT_VALUE(is_same, T, ::std::allocator_arg_t))>
    UTL_REQUIRES_CXX20(same_as<T, ::std::allocator_arg_t>)
    constexpr operator T() const noexcept {
        return {};
    }
};

UTL_INLINE_CXX17 constexpr allocator_arg_t allocator_arg = allocator_arg_t{};

/**
 * Specializing std will work on both utl and std, specializing utl will only work on utl
 */
template <typename T, typename Alloc>
struct uses_allocator : details::uses_allocator::impl<T, Alloc> {};

template <typename T, typename Alloc, typename... Args>
struct is_constructible_with_allocator :
    conjunction<uses_allocator<T, Alloc>,
        disjunction<is_constructible<T, allocator_arg_t, Alloc const&, Args...>,
            is_constructible<T, Args..., Alloc const&>,
            is_constructible<T, ::std::allocator_arg_t, Alloc const&, Args...>>> {};

template <typename T, typename Alloc, typename... Args>
struct is_nothrow_constructible_with_allocator :
    conjunction<uses_allocator<T, Alloc>,
        disjunction<is_nothrow_constructible<T, allocator_arg_t, Alloc const&, Args...>,
            is_nothrow_constructible<T, Args..., Alloc const&>,
            is_nothrow_constructible<T, ::std::allocator_arg_t, Alloc const&, Args...>>> {};

template <typename T, typename Alloc, typename... Args>
struct is_explicit_constructible_with_allocator :
    conjunction<uses_allocator<T, Alloc>,
        disjunction<is_explicit_constructible<T, allocator_arg_t, Alloc const&, Args...>,
            is_explicit_constructible<T, Args..., Alloc const&>,
            is_explicit_constructible<T, ::std::allocator_arg_t, Alloc const&, Args...>>> {};

template <typename T, typename Alloc, typename... Args>
struct is_implicit_constructible_with_allocator :
    conjunction<uses_allocator<T, Alloc>,
        negation<is_explicit_constructible_with_allocator<T, Alloc, Args...>>,
        is_constructible_with_allocator<T, Alloc, Args...>> {};

UTL_NAMESPACE_END
