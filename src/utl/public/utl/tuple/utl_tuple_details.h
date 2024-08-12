#pragma once

#include "utl/preprocessor/utl_config.h"

#include "utl/tuple/utl_tuple_fwd.h"

#include "utl/memory/utl_uses_allocator.h"
#include "utl/tuple/utl_tuple_get_element.h"
#include "utl/type_traits/utl_copy_cvref.h"
#include "utl/type_traits/utl_decay.h"
#include "utl/type_traits/utl_declval.h"
#include "utl/type_traits/utl_is_base_of.h"
#include "utl/type_traits/utl_is_move_assignable.h"
#include "utl/type_traits/utl_is_move_constructible.h"
#include "utl/type_traits/utl_is_nothrow_copy_assignable.h"
#include "utl/type_traits/utl_is_nothrow_copy_constructible.h"
#include "utl/type_traits/utl_is_nothrow_default_constructible.h"
#include "utl/type_traits/utl_is_nothrow_move_assignable.h"
#include "utl/type_traits/utl_is_nothrow_move_constructible.h"
#include "utl/type_traits/utl_is_standard_layout.h"
#include "utl/type_traits/utl_is_swappable.h"
#include "utl/type_traits/utl_remove_reference.h"
#include "utl/type_traits/utl_unwrap_reference.h"
#include "utl/utility/utl_sequence.h"

#include <cstddef>

#if UTL_CXX20
#  include "utl/concepts/utl_constructible_as.h"
#  include "utl/concepts/utl_constructible_from.h"
#  include "utl/concepts/utl_same_as.h"
#else
#  include "utl/type_traits/utl_is_constructible.h"
#  include "utl/type_traits/utl_is_same.h"
#  include "utl/type_traits/utl_logical_traits.h"
#endif

UTL_NAMESPACE_BEGIN

template <typename...>
struct variadic_traits;

namespace details {
namespace tuple {
template <size_t I, typename T UTL_REQUIRES_CXX11((I < tuple_size_v<T>))>
UTL_REQUIRES_CXX20(requires {
    UTL_SCOPE get_element<I>(UTL_SCOPE declval<T>()); })
auto decl_element() noexcept -> decltype(UTL_SCOPE get_element<I>(UTL_SCOPE declval<T>()));

template <size_t I, typename T UTL_REQUIRES_CXX11((I >= tuple_size_v<T>))>
void decl_element() noexcept = delete;

template <size_t I, typename T UTL_REQUIRES_CXX11((I < tuple_size_v<T>))>
UTL_REQUIRES_CXX20(requires {
    UTL_SCOPE get_element<I>(UTL_SCOPE declval<T>()); })
using get_type_t = decltype(UTL_SCOPE get_element<I>(UTL_SCOPE declval<T>()));

template <template <typename...> class Target, typename T, size_t... Is>
__UTL_HIDE_FROM_ABI Target<decltype(decl_element<Is, T>())...> rebind_references_impl(
    T&&, UTL_SCOPE index_sequence<Is...>) noexcept;

template <template <typename...> class Target, typename T, size_t... Is>
__UTL_HIDE_FROM_ABI Target<UTL_SCOPE tuple_element_t<Is, T>...> rebind_elements_impl(
    T&&, UTL_SCOPE index_sequence<Is...>) noexcept;

/**
 * Rebinds the result of `get` for a tuple-like type onto a variadic template type
 *
 * @tparam Target - the target variadic type that will contain the refereces
 * @tparam TupleLike - any tuple-like type or reference, may be cv-qualified
 * @tparam N - number of elements to consider, defaults to tuple_size<TupleLike>::value
 *
 * @return `Target<Ref...>`
 *  where:
 *    `Ref` is `decltype(get<I>(declval<TupleLike>()))` or
 * `decltype(declval<TupleLike>().get<I>())` where I is in the interval [0, N)
 */
template <template <typename...> class Target, typename TupleLike,
    size_t N = tuple_size<TupleLike>::value>
using rebind_references_t UTL_NODEBUG = decltype(rebind_references_impl<Target>(
    UTL_SCOPE declval<TupleLike>(), UTL_SCOPE make_index_sequence<N>{}));

template <template <typename...> class Target, typename TupleLike,
    size_t N = tuple_size<TupleLike>::value>
struct rebind_references {
    using type UTL_NODEBUG = rebind_references_t<Target, TupleLike, N>;
};

/**
 * Rebinds the result of `tuple_element` for a tuple-like type onto a variadic template type
 *
 * @tparam Target - the target variadic type that will contain the refereces
 * @tparam TupleLike - any tuple-like type or reference, may be cv-qualified
 * @tparam N - number of elements to consider, defaults to tuple_size<TupleLike>::value
 *
 * @return `Target<Ref...>`
 *  where:
 *    `Ref` is `decltype(get<I>(declval<TupleLike>()))` or
 * `decltype(declval<TupleLike>().get<I>())` where I is in the interval [0, N)
 */
template <template <typename...> class Target, typename TupleLike,
    size_t N = tuple_size<TupleLike>::value>
using rebind_elements_t UTL_NODEBUG = decltype(rebind_elements_impl<Target>(
    UTL_SCOPE declval<TupleLike>(), UTL_SCOPE make_index_sequence<N>{}));

template <template <typename...> class Target, typename TupleLike,
    size_t N = tuple_size<TupleLike>::value>
struct rebind_elements {
    using type UTL_NODEBUG = rebind_elements_t<Target, TupleLike, N>;
};

template <typename...>
struct invalid_swap_t {
    __UTL_HIDE_FROM_ABI inline constexpr void swap(invalid_swap_t& other) const noexcept {}
    __UTL_HIDE_FROM_ABI inline constexpr void swap(invalid_swap_t const& other) const noexcept {}

private:
    __UTL_HIDE_FROM_ABI ~invalid_swap_t() = default;
};

template <typename...>
struct storage;
template <size_t I, typename T, typename = void>
struct offset_impl;
struct invalid_t {
private:
    __UTL_HIDE_FROM_ABI ~invalid_t() = default;
};

template <>
struct storage<> {
    __UTL_HIDE_FROM_ABI constexpr storage() noexcept = default;
    __UTL_HIDE_FROM_ABI constexpr storage& operator=(storage const&) noexcept = default;
    __UTL_HIDE_FROM_ABI constexpr storage& operator=(storage&&) noexcept = default;
    __UTL_HIDE_FROM_ABI UTL_CONSTEXPR_CXX14 void swap(storage<>& other) noexcept {}
};

template <typename T>
struct storage<T> {
public:
    using traits UTL_NODEBUG = variadic_traits<T>;
    using head_type UTL_NODEBUG = T;
    using move_assign_t UTL_NODEBUG =
        conditional_t<UTL_TRAIT_is_move_assignable(T), invalid_t, storage>;
    using move_construct_t UTL_NODEBUG =
        conditional_t<UTL_TRAIT_is_move_constructible(T), invalid_t, storage>;

    __UTL_HIDE_FROM_ABI constexpr storage() noexcept(
        UTL_TRAIT_is_nothrow_default_constructible(T)) = default;
    __UTL_HIDE_FROM_ABI constexpr storage(storage const&) noexcept(
        UTL_TRAIT_is_nothrow_copy_constructible(T)) = default;
    __UTL_HIDE_FROM_ABI UTL_CONSTEXPR_CXX14 storage& operator=(storage const&) noexcept(
        UTL_TRAIT_is_nothrow_copy_assignable(T)) = default;

#if UTL_ENFORCE_NONMOVABILIITY
    __UTL_HIDE_FROM_ABI constexpr storage(move_construct_t&&) noexcept(
        UTL_TRAIT_is_nothrow_move_constructible(T)) = delete;
    __UTL_HIDE_FROM_ABI UTL_CONSTEXPR_CXX14 storage& operator=(move_assign_t&&) noexcept(
        UTL_TRAIT_is_nothrow_move_assignable(T)) = delete;
#else
    __UTL_HIDE_FROM_ABI constexpr storage(storage&&) noexcept(
        UTL_TRAIT_is_nothrow_move_constructible(T)) = default;
    __UTL_HIDE_FROM_ABI UTL_CONSTEXPR_CXX14 storage& operator=(storage&&) noexcept(
        UTL_TRAIT_is_nothrow_move_assignable(T)) = default;
#endif

    template <UTL_CONCEPT_CXX20(constructible_as<T>) U UTL_REQUIRES_CXX11(
        !UTL_SCOPE is_same<U, storage>::value && UTL_SCOPE is_constructible<T, U>::value)>
    UTL_REQUIRES_CXX20(!same_as<remove_reference_t<U>, storage>)
    __UTL_HIDE_FROM_ABI constexpr storage(U&& head) noexcept(
        UTL_TRAIT_is_nothrow_constructible(T, U))
        : head(UTL_SCOPE forward<U>(head)) {}

    template <UTL_CONCEPT_CXX20(allocator_usable_with<T>) Alloc UTL_REQUIRES_CXX11(
        conjunction<UTL_SCOPE uses_allocator<T, Alloc>,
            UTL_SCOPE is_constructible<T, allocator_arg_t, Alloc const&>>::value)>
    UTL_REQUIRES_CXX20(constructible_from<T, allocator_arg_t, Alloc const&>)
    __UTL_HIDE_FROM_ABI constexpr storage(allocator_arg_t, Alloc const& alloc) noexcept(
        UTL_TRAIT_is_nothrow_constructible(T, allocator_arg_t, Alloc const&))
        : head(allocator_arg, alloc) {}

    template <UTL_CONCEPT_CXX20(allocator_usable_with<T>) Alloc UTL_REQUIRES_CXX11(
        conjunction<UTL_SCOPE uses_allocator<T, Alloc>,
            negation<UTL_SCOPE is_constructible<T, allocator_arg_t, Alloc const&>>,
            UTL_SCOPE is_constructible<T, Alloc const&>>::value)>
    UTL_REQUIRES_CXX20(!constructible_from<T, allocator_arg_t, Alloc const&> &&
        constructible_from<T, Alloc const&>)
    __UTL_HIDE_FROM_ABI constexpr storage(allocator_arg_t, Alloc const& alloc) noexcept(
        UTL_TRAIT_is_nothrow_constructible(T, Alloc const&))
        : head(alloc) {}

    template <UTL_CONCEPT_CXX20(allocator_type) Alloc UTL_REQUIRES_CXX11(
        !UTL_TRAIT_uses_allocator(T, Alloc))>
    UTL_REQUIRES_CXX20(constructible_from<T>)
    __UTL_HIDE_FROM_ABI constexpr storage(allocator_arg_t, Alloc const& alloc) noexcept(
        UTL_TRAIT_is_nothrow_constructible(T))
        : head() {}

    template <UTL_CONCEPT_CXX20(allocator_usable_with<T>) Alloc,
        typename U UTL_REQUIRES_CXX11(conjunction<UTL_SCOPE uses_allocator<T, Alloc>,
            UTL_SCOPE is_constructible<T, allocator_arg_t, Alloc const&, U>>::value)>
    UTL_REQUIRES_CXX20(constructible_from<T, allocator_arg_t, Alloc const&, U>)
    __UTL_HIDE_FROM_ABI constexpr storage(allocator_arg_t, Alloc const& alloc, U&& u) noexcept(
        UTL_TRAIT_is_nothrow_constructible(T, allocator_arg_t, Alloc const&, U))
        : head(allocator_arg, alloc, UTL_SCOPE forward<U>(u)) {}

    template <UTL_CONCEPT_CXX20(allocator_usable_with<T>) Alloc,
        typename U UTL_REQUIRES_CXX11(conjunction<UTL_SCOPE uses_allocator<T, Alloc>,
            negation<UTL_SCOPE is_constructible<T, allocator_arg_t, Alloc const&, U>>,
            UTL_SCOPE is_constructible<T, U, Alloc const&>>::value)>
    UTL_REQUIRES_CXX20(!constructible_from<T, allocator_arg_t, Alloc const&, U> &&
        constructible_from<T, U, Alloc const&>)
    __UTL_HIDE_FROM_ABI constexpr storage(allocator_arg_t, Alloc const& alloc, U&& u) noexcept(
        UTL_TRAIT_is_nothrow_constructible(T, U, Alloc const&))
        : head(UTL_SCOPE forward<U>(u), alloc) {}

    template <UTL_CONCEPT_CXX20(allocator_usable_with<T>) Alloc, typename U UTL_REQUIRES_CXX11(
        !UTL_TRAIT_uses_allocator(T, Alloc))>
    UTL_REQUIRES_CXX20(constructible_from<T, U>)
    __UTL_HIDE_FROM_ABI constexpr storage(allocator_arg_t, Alloc const& alloc, U&& u) noexcept(
        UTL_TRAIT_is_nothrow_constructible(T, U))
        : head(UTL_SCOPE forward<U>(u)) {}

    template <typename U = T>
    __UTL_HIDE_FROM_ABI UTL_CONSTEXPR_CXX14 void swap(storage<U>& other) noexcept(
        UTL_TRAIT_is_nothrow_swappable_with(T&, U&)) {
        UTL_SCOPE ranges::swap(head, other.head);
    }

    template <typename U = T>
    __UTL_HIDE_FROM_ABI UTL_CONSTEXPR_CXX14 void swap(storage<U> const& other) noexcept(
        UTL_TRAIT_is_nothrow_swappable_with(T&, U const&)) {
        UTL_SCOPE ranges::swap(head, other.head);
    }

    template <typename U = T>
    __UTL_HIDE_FROM_ABI UTL_CONSTEXPR_CXX14 void swap(storage<U>& other) const
        noexcept(UTL_TRAIT_is_nothrow_swappable_with(T const&, U&)) {
        UTL_SCOPE ranges::swap(head, other.head);
    }

    template <typename U = T>
    __UTL_HIDE_FROM_ABI UTL_CONSTEXPR_CXX14 void swap(storage<U> const& other) const
        noexcept(UTL_TRAIT_is_nothrow_swappable_with(T const&, U const&)) {
        UTL_SCOPE ranges::swap(head, other.head);
    }

    template <typename U>
    __UTL_HIDE_FROM_ABI UTL_CONSTEXPR_CXX14 storage& assign(U&& other) noexcept(
        UTL_TRAIT_is_nothrow_assignable(T&, U)) {
        head = UTL_SCOPE forward<U>(other);
        return *this;
    }

    template <typename U>
    __UTL_HIDE_FROM_ABI constexpr storage const& assign(U&& other) const
        noexcept(UTL_TRAIT_is_nothrow_assignable(T const&, U)) {
        head = UTL_SCOPE forward<U>(other);
        return *this;
    }

    template <size_t I>
    UTL_REQUIRES_CXX20(I == 0)
    UTL_ATTRIBUTES(NODISCARD, CONST, _HIDE_FROM_ABI) UTL_CONSTEXPR_CXX14 auto get() && noexcept UTL_LIFETIMEBOUND
    -> UTL_ENABLE_IF_CXX11(T&&, I == 0) {
        return UTL_SCOPE move(head);
    }

    template <size_t I>
    UTL_REQUIRES_CXX20(I == 0)
    UTL_ATTRIBUTES(NODISCARD, CONST, _HIDE_FROM_ABI) UTL_CONSTEXPR_CXX14 auto get() & noexcept UTL_LIFETIMEBOUND
    -> UTL_ENABLE_IF_CXX11(T&, I == 0) {
        return head;
    }

    template <size_t I>
    UTL_REQUIRES_CXX20(I == 0)
    UTL_ATTRIBUTES(NODISCARD, CONST, _HIDE_FROM_ABI) constexpr auto get() const&& noexcept UTL_LIFETIMEBOUND -> UTL_ENABLE_IF_CXX11(T const&&, I == 0) {
        return UTL_SCOPE move(head);
    }

    template <size_t I>
    UTL_REQUIRES_CXX20(I == 0)
    UTL_ATTRIBUTES(NODISCARD, CONST, _HIDE_FROM_ABI) constexpr auto get() const& noexcept UTL_LIFETIMEBOUND -> UTL_ENABLE_IF_CXX11(T const&, I == 0) {
        return head;
    }

    UTL_ATTRIBUTE(NO_UNIQUE_ADDRESS) head_type head;
};

template <typename T>
__UTL_HIDE_FROM_ABI constexpr size_t head_offset() noexcept {
    return offsetof(T, head);
}

template <typename T>
__UTL_HIDE_FROM_ABI constexpr size_t tail_offset() noexcept {
    return offsetof(T, tail);
}

template <typename... Ts>
struct offset_impl<0, storage<Ts...>> : size_constant<head_offset<storage<Ts...>>()> {
    static_assert(is_standard_layout<storage<Ts...>>::value, "Must be standard layout");
};

template <size_t I, typename T0, typename... Ts>
struct offset_impl<I, storage<T0, Ts...>, enable_if_t<(I > 0)>> :
    size_constant<offset_impl<I - 1, storage<Ts...>>::value + tail_offset<storage<T0, Ts...>>()> {
    static_assert(is_standard_layout<storage<T0, Ts...>>::value, "Must be standard layout");
    static_assert(I < (1 + sizeof...(Ts)), "Index out of bounds");
};

} // namespace tuple
} // namespace details

template <>
class UTL_ATTRIBUTES(_PUBLIC_TEMPLATE, EMPTY_BASES) tuple<> : private details::tuple::storage<> {
public:
    using storage::storage;
    using storage::operator=;
    __UTL_HIDE_FROM_ABI UTL_CONSTEXPR_CXX14 void swap(tuple const& other) const noexcept {}
    __UTL_HIDE_FROM_ABI friend inline UTL_CONSTEXPR_CXX14 void swap(tuple const&, tuple const&) noexcept {}
};

template <typename... Ts>
UTL_ATTRIBUTES(NODISCARD, _HIDE_FROM_ABI, ALWAYS_INLINE) constexpr tuple<unwrap_reference_t<decay_t<Ts>>...>
make_tuple(Ts&&... ts) noexcept(
    UTL_TRAIT_is_nothrow_constructible(tuple<unwrap_reference_t<decay_t<Ts>>...>, Ts...)) {
    return tuple<unwrap_reference_t<decay_t<Ts>>...>{UTL_SCOPE forward<Ts>(ts)...};
}

template <typename... Args>
UTL_ATTRIBUTES(NODISCARD, _HIDE_FROM_ABI) constexpr tuple<Args&...> tie(
    Args&... args UTL_LIFETIMEBOUND) noexcept {
    return {args...};
}

template <typename... Args>
UTL_ATTRIBUTES(NODISCARD, _HIDE_FROM_ABI) constexpr tuple<Args&&...> forward_as_tuple(
    Args&&... args UTL_LIFETIMEBOUND) noexcept {
    return {UTL_SCOPE forward<Args>(args)...};
}

template <size_t I, typename T>
struct tuple_element_offset;

template <size_t I, typename... Ts>
struct __UTL_PUBLIC_TEMPLATE tuple_element_offset<I, tuple<Ts...>> :
    details::tuple::offset_impl<I, details::tuple::storage<Ts...>> {
    static_assert(UTL_TRAIT_is_base_of(details::tuple::storage<Ts...>, tuple<Ts...>),
        "invalid tuple implementation");
};

UTL_NAMESPACE_END
