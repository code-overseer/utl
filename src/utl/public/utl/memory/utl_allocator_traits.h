// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/compare/utl_compare_traits.h"
#include "utl/memory/utl_allocator_fwd.h"
#include "utl/memory/utl_pointer_traits.h"
#include "utl/string/utl_libc.h"
#include "utl/type_traits/utl_declval.h"
#include "utl/type_traits/utl_is_empty.h"
#include "utl/type_traits/utl_is_nothrow_copy_assignable.h"
#include "utl/type_traits/utl_is_nothrow_copy_constructible.h"
#include "utl/type_traits/utl_is_nothrow_move_assignable.h"
#include "utl/type_traits/utl_is_nothrow_move_constructible.h"
#include "utl/type_traits/utl_is_pointer.h"
#include "utl/type_traits/utl_is_same.h"
#include "utl/type_traits/utl_logical_traits.h"
#include "utl/type_traits/utl_make_unsigned.h"
#include "utl/type_traits/utl_void_t.h"
#include "utl/utility/utl_forward.h"
#include "utl/utility/utl_move.h"
#include "utl/utility/utl_swap.h"

UTL_NAMESPACE_BEGIN

namespace details {
namespace allocator {
template <typename Alloc, typename = void>
struct propagate_on_container_copy_assignment : false_type {};
template <typename Alloc>
struct propagate_on_container_copy_assignment<Alloc,
    void_t<typename Alloc::propagate_on_container_copy_assignment>> :
    Alloc::propagate_on_container_copy_assignment {};

template <typename Alloc, typename = void>
struct propagate_on_container_move_assignment : false_type {};
template <typename Alloc>
struct propagate_on_container_move_assignment<Alloc,
    void_t<typename Alloc::propagate_on_container_move_assignment>> :
    Alloc::propagate_on_container_move_assignment {};

template <typename Alloc, typename = void>
struct propagate_on_container_swap : false_type {};
template <typename Alloc>
struct propagate_on_container_swap<Alloc, void_t<typename Alloc::propagate_on_container_swap>> :
    Alloc::propagate_on_container_swap {};

template <typename Alloc, typename = void>
struct is_always_equal : is_empty<Alloc> {};
template <typename Alloc>
struct is_always_equal<Alloc, void_t<typename Alloc::is_always_equal>> : Alloc::is_always_equal {};

template <typename Alloc, typename = Alloc>
struct selectable_copy_construction : false_type {};
template <typename Alloc>
struct selectable_copy_construction<Alloc,
    decltype(declval<Alloc const&>().select_on_container_copy_construction())> : true_type {};

template <typename Alloc, typename = void>
struct pointer {
    using type = typename Alloc::value_type*;
};
template <typename Alloc>
struct pointer<Alloc, void_t<typename Alloc::pointer>> {
    using type = typename Alloc::pointer;
};
template <typename Alloc>
using pointer_t = typename pointer<Alloc>::type;

template <typename Alloc, typename = void>
struct difference_type {
    using type = typename UTL_SCOPE pointer_traits<pointer_t<Alloc>>::difference_type;
};

template <typename Alloc>
struct difference_type<Alloc, void_t<typename Alloc::difference_type>> {
    using type = typename Alloc::difference_type;
};
template <typename Alloc>
using difference_type_t = typename difference_type<Alloc>::type;

template <typename Alloc, typename = void>
struct size_type {
    using type = make_unsigned_t<difference_type_t<Alloc>>;
};

template <typename Alloc>
struct size_type<Alloc, void_t<typename Alloc::size_type>> {
    using type = typename Alloc::size_type;
};
template <typename Alloc>
using size_type_t = typename size_type<Alloc>::type;

template <typename Alloc>
using result_type_t = allocation_result<pointer_t<Alloc>, size_type_t<Alloc>>;

template <typename Alloc, typename T, typename = void>
struct has_rebind : false_type {};

template <typename Alloc, typename T>
struct has_rebind<Alloc, T, void_t<typename Alloc::template rebind<T>::other>> : true_type {};

template <typename Alloc, typename T, bool = has_rebind<Alloc, T>::value>
struct rebind;

template <typename Alloc, typename T>
struct rebind<Alloc, T, true> {
    using type = typename Alloc::template rebind<T>::other;
};

template <typename To, template <typename, typename...> class Alloc, typename From,
    typename... Args>
struct rebind<Alloc<From, Args...>, To, false> {
    using type = Alloc<To, Args...>;
};

template <typename T, typename U>
constexpr T& assign(T& dst, U&&, false_type) noexcept {
    return dst;
}

template <typename T>
UTL_CONSTEXPR_CXX14 T& assign(T& dst, T&& src, propagate_on_container_move_assignment<T>) noexcept {
    return dst = move(src);
}

template <typename T>
UTL_CONSTEXPR_CXX14 T& assign(T& dst, T const& src, propagate_on_container_copy_assignment<T>) noexcept {
    return dst = src;
}

template <typename T>
constexpr T copy(T const& dst, false_type) noexcept {
    return dst;
}

template <typename T>
constexpr T copy(T const& dst, selectable_copy_construction<T>) noexcept {
    return dst.select_on_container_copy_construction();
}

#if !UTL_CXX20

template <typename T>
auto implements_reallocate_impl(float) noexcept -> false_type;

template <typename T>
auto implements_reallocate_impl(int) noexcept -> is_same<pointer_t<T>,
    decltype(declval<T>().reallocate(result_type_t<T>{}, size_type_t<T>{}))>;

template <typename T>
using implements_reallocate = decltype(implements_reallocate_impl<T>(0));

#else

template <typename T>
concept implements_reallocate = requires(T& alloc, result_type_t<T> r, size_type_t<T> s) {
    { alloc.reallocate(r, s) } -> UTL_SCOPE same_as<pointer_t<T>>;
};

#endif
template <typename T>
UTL_CONSTEXPR_CXX20 pointer_t<T> fallback_reallocate(
    T& allocator, result_type_t<T> arg, size_type_t<T> size) {
    static_assert(
        is_pointer<pointer_t<T>>::value, "Only raw pointers can use the fallback reallocation");
    auto dst = allocator.allocate(size);
    auto blessed = libc::unsafe::memcpy(to_address(dst), to_address(arg.ptr), arg.size);
    allocator.deallocate(arg.ptr, arg.size);
    return blessed;
}

template <UTL_CONCEPT_CXX20(implements_reallocate) T UTL_REQUIRES_CXX11(
    implements_reallocate<T>::value && is_pointer<pointer_t<T>>::value)>
UTL_REQUIRES_CXX20(is_pointer_v<pointer_t<T>>)
UTL_CONSTEXPR_CXX20 pointer_t<T> reallocate(T& allocator, result_type_t<T> arg, size_type_t<T> size) {
#if UTL_CXX20
    if (UTL_BUILTIN_is_constant_evaluated()) {
        return fallback_reallocate(allocator, arg, size);
    }
#endif
    return allocator.reallocate(arg, size);
}

template <UTL_CONCEPT_CXX20(implements_reallocate) T UTL_REQUIRES_CXX11(
    implements_reallocate<T>::value && !is_pointer<pointer_t<T>>::value)>
UTL_REQUIRES_CXX20(!is_pointer_v<pointer_t<T>>)
UTL_CONSTEXPR_CXX20 pointer_t<T> reallocate(T& allocator, result_type_t<T> arg, size_type_t<T> size) {
    return allocator.reallocate(arg, size);
}

template <typename T UTL_REQUIRES_CXX11(!implements_reallocate<T>::value)>
UTL_CONSTEXPR_CXX20 pointer_t<T> reallocate(T& allocator, result_type_t<T> arg, size_type_t<T> size) {
    return fallback_reallocate(allocator, arg, size);
}

#if !UTL_CXX20

template <typename T>
auto implements_allocate_at_least_impl(float) noexcept -> false_type;

template <typename T>
auto implements_allocate_at_least_impl(int) noexcept -> is_same<result_type_t<T>,
    decltype(declval<T>().allocate_at_least(result_type_t<T>{}, size_type_t<T>{}))>;

template <typename T>
using implements_allocate_at_least = decltype(implements_allocate_at_least_impl<T>(0));

#else

template <typename T>
concept implements_allocate_at_least = requires(T& alloc, size_type_t<T> size) {
    { alloc.allocate_at_least(size) } -> UTL_SCOPE same_as<result_type_t<T>>;
};

#endif

template <typename T UTL_REQUIRES_CXX11(!implements_allocate_at_least<T>::value)>
UTL_CONSTEXPR_CXX20 result_type_t<T> allocate_at_least(T& allocator, size_type_t<T> size) {
    return {allocate(allocator, size), size};
}

template <UTL_CONCEPT_CXX20(implements_allocate_at_least) T UTL_REQUIRES_CXX11(
    implements_allocate_at_least<T>::value)>
UTL_CONSTEXPR_CXX20 result_type_t<T> allocate_at_least(T& allocator, size_type_t<T> size) {
    return allocator.allocate_at_least(size);
}

#if !UTL_CXX20

template <typename T>
auto implements_reallocate_at_least_impl(float) noexcept -> false_type;

template <typename T>
auto implements_reallocate_at_least_impl(int) noexcept -> is_same<result_type_t<T>,
    decltype(declval<T>().reallocate_at_least(result_type_t<T>{}, size_type_t<T>{}))>;

template <typename T>
using implements_reallocate_at_least = decltype(implements_reallocate_at_least_impl<T>(0));

#else

template <typename T>
concept implements_reallocate_at_least = requires(T& alloc, result_type_t<T> r, size_type_t<T> s) {
    { alloc.reallocate_at_least(r, s) } -> UTL_SCOPE same_as<result_type_t<T>>;
};

#endif

template <typename T UTL_REQUIRES_CXX11(!implements_reallocate_at_least<T>::value)>
UTL_CONSTEXPR_CXX20 result_type_t<T> reallocate_at_least(
    T& allocator, result_type_t<T> arg, size_type_t<T> new_size) {
    return UTL_SCOPE details::allocator::reallocate(allocator, new_size);
}

template <UTL_CONCEPT_CXX20(implements_reallocate_at_least) T UTL_REQUIRES_CXX11(
    implements_reallocate_at_least<T>::value)>
UTL_CONSTEXPR_CXX20 result_type_t<T> reallocate_at_least(
    T& allocator, result_type_t<T> arg, size_type_t<T> size) {
    return allocator.reallocate_at_least(arg, size);
}

} // namespace allocator
} // namespace details

template <typename Alloc>
struct allocator_traits {
    using allocator_type = Alloc;
    using value_type = typename allocator_type::value_type;
    using pointer = details::allocator::pointer_t<allocator_type>;
    using const_pointer = typename pointer_traits<pointer>::template rebind<value_type const>;
    using difference_type = details::allocator::difference_type_t<allocator_type>;
    using size_type = details::allocator::size_type_t<allocator_type>;
    using propagate_on_container_copy_assignment =
        details::allocator::propagate_on_container_copy_assignment<allocator_type>;
    using propagate_on_container_move_assignment =
        details::allocator::propagate_on_container_move_assignment<allocator_type>;
    using propagate_on_container_swap =
        details::allocator::propagate_on_container_swap<allocator_type>;
    using selectable_copy_construction =
        details::allocator::selectable_copy_construction<allocator_type>;
    using is_always_equal = details::allocator::is_always_equal<allocator_type>;
    using allocation_result = allocation_result<pointer, size_type>;
    using nothrow_move_assignable = bool_constant<propagate_on_container_move_assignment::value ||
        is_always_equal::value && UTL_SCOPE is_nothrow_move_assignable<allocator_type>::value>;

    template <typename T>
    using rebind_alloc = typename details::allocator::rebind<Alloc, T>::type;
    template <typename T>
    using rebind_traits = allocator_traits<rebind_alloc<T>>;

    static_assert(UTL_SCOPE is_nothrow_copy_constructible<allocator_type>::value,
        "Alloc must be nothrow copy constructible");
    static_assert(UTL_SCOPE is_nothrow_move_constructible<allocator_type>::value,
        "Alloc must be nothrow move constructible");
    static_assert(
        is_always_equal::value || UTL_SCOPE is_nothrow_equality_comparable<allocator_type>::value,
        "Alloc must be nothrow equality comparable if it is not always equal");
    static_assert(!propagate_on_container_copy_assignment::value ||
            UTL_SCOPE is_nothrow_copy_assignable<allocator_type>::value,
        "If propogation on copy assignment is required, allocator must be nothrow copy assignable");
    static_assert(!propagate_on_container_move_assignment::value ||
            UTL_SCOPE is_nothrow_move_assignable<allocator_type>::value,
        "If propogation on move assignment is required, allocator must be nothrow move assignable");
    static_assert(!propagate_on_container_swap::value ||
            UTL_SCOPE is_nothrow_swappable<allocator_type>::value,
        "If propogation on swap is required, allocator must be nothrow swappable");

    UTL_NODISCARD static UTL_CONSTEXPR_CXX20 pointer allocate(allocator_type& alloc, size_type size) {
        return alloc.allocate(size);
    }

    UTL_NODISCARD static UTL_CONSTEXPR_CXX20 allocation_result allocate_at_least(
        allocator_type& alloc, size_type size) {
        return details::allocator::allocate_at_least(alloc, size);
    }

    static UTL_CONSTEXPR_CXX20 void deallocate(allocator_type& alloc, pointer ptr, size_type size) {
        alloc.deallocate(ptr, size);
    }

    UTL_NODISCARD static UTL_CONSTEXPR_CXX20 pointer reallocate(
        allocator_type& alloc, allocation_result arg, size_type new_size) {
        return details::allocator::reallocate(alloc, arg, new_size);
    }

    UTL_NODISCARD static UTL_CONSTEXPR_CXX20 allocation_result reallocate_at_least(
        allocator_type& alloc, allocation_result arg, size_type new_size) {
        return details::allocator::reallocate_at_least(alloc, arg, new_size);
    }

    static UTL_CONSTEXPR_CXX14 allocator_type& assign(
        allocator_type& dst, allocator_type const& src) noexcept {
        return details::allocator::assign(dst, src, propagate_on_container_copy_assignment{});
    }

    static UTL_CONSTEXPR_CXX14 allocator_type& assign(allocator_type& dst, allocator_type&& src) noexcept {
        return details::allocator::assign(dst, move(src), propagate_on_container_move_assignment{});
    }

    UTL_NODISCARD static UTL_CONSTEXPR_CXX14 allocator_type select_on_container_copy_construction(
        allocator_type const& p) noexcept {
        return details::allocator::copy(p, selectable_copy_construction{});
    }

    template <typename T = allocator_type>
    UTL_NODISCARD static constexpr enable_if_t<
        is_same<T, allocator_type>::value && is_always_equal::value, bool>
    equals(T const&, T const&) noexcept {
        return true;
    }

    template <typename T = allocator_type>
    UTL_NODISCARD static constexpr enable_if_t<
        is_same<T, allocator_type>::value && !is_always_equal::value, bool>
    equals(T const& left, T const& right) noexcept {
        return left == right;
    }
};

UTL_NAMESPACE_END
