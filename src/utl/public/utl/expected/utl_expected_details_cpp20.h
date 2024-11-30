// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

#if !defined(UTL_EXPECTED_PRIVATE_HEADER_GUARD)
#  error "Private header accessed"
#endif
#if !UTL_CXX20
#  error "Invalid header accessed"
#endif

#include "utl/expected/utl_bad_expected_access.h"
#include "utl/expected/utl_expected_common.h"
#include "utl/expected/utl_unexpected.h"
#include "utl/functional/utl_invoke.h"
#include "utl/memory/utl_addressof.h"
#include "utl/memory/utl_construct_at.h"
#include "utl/memory/utl_destroy_at.h"
#include "utl/scope/utl_scope_fail.h"
#include "utl/type_traits/utl_constants.h"
#include "utl/type_traits/utl_declval.h"
#include "utl/type_traits/utl_enable_if.h"
#include "utl/type_traits/utl_invoke.h"
#include "utl/type_traits/utl_is_copy_assignable.h"
#include "utl/type_traits/utl_is_copy_constructible.h"
#include "utl/type_traits/utl_is_destructible.h"
#include "utl/type_traits/utl_is_explicit_constructible.h"
#include "utl/type_traits/utl_is_move_assignable.h"
#include "utl/type_traits/utl_is_move_constructible.h"
#include "utl/type_traits/utl_is_nothrow_assignable.h"
#include "utl/type_traits/utl_is_nothrow_constructible.h"
#include "utl/type_traits/utl_is_nothrow_copy_assignable.h"
#include "utl/type_traits/utl_is_nothrow_copy_constructible.h"
#include "utl/type_traits/utl_is_nothrow_default_constructible.h"
#include "utl/type_traits/utl_is_nothrow_move_assignable.h"
#include "utl/type_traits/utl_is_nothrow_move_constructible.h"
#include "utl/type_traits/utl_is_trivially_constructible.h"
#include "utl/type_traits/utl_is_trivially_copy_assignable.h"
#include "utl/type_traits/utl_is_trivially_copy_constructible.h"
#include "utl/type_traits/utl_is_trivially_destructible.h"
#include "utl/type_traits/utl_is_trivially_move_assignable.h"
#include "utl/type_traits/utl_is_trivially_move_constructible.h"
#include "utl/type_traits/utl_is_void.h"
#include "utl/type_traits/utl_logical_traits.h"
#include "utl/type_traits/utl_remove_cvref.h"
#include "utl/utility/utl_forward.h"
#include "utl/utility/utl_in_place.h"
#include "utl/utility/utl_move.h"

#include <new>

#define __UTL_ATTRIBUTE_GETTER (ALWAYS_INLINE)(NODISCARD) __UTL_ATTRIBUTE__HIDE_FROM_ABI
#define __UTL_ATTRIBUTE_TYPE_AGGREGATE_GETTER

UTL_NAMESPACE_BEGIN

namespace details {
namespace expected {
template <typename T0, class T1>
inline constexpr bool fits_in_tail_padding_v = []() {
    struct test_struct {
    UTL_ATTRIBUTE(NO_UNIQUE_ADDRESS) T0 first;
    UTL_ATTRIBUTE(NO_UNIQUE_ADDRESS) T1 second;
    };
    return sizeof(test_struct) == sizeof(T0);
}();

template <typename T, typename U>
UTL_ATTRIBUTE(GETTER) inline constexpr T make_from_union(bool has_value, U&& arg) noexcept(
    UTL_TRAIT_is_nothrow_constructible(T, in_place_t, decltype(__UTL declval<U>().value)) &&
    UTL_TRAIT_is_nothrow_constructible(T, unexpect_t, decltype(__UTL declval<U>().error))) {
    return has_value ? T{__UTL in_place, __UTL forward_like<U>(arg.value)}
                     : T{__UTL unexpect, __UTL forward_like<U>(arg.error)};
}

template <bool NoUniqueAdress, typename T>
struct conditionally_overlapable {
    __UTL_HIDE_FROM_ABI inline constexpr conditionally_overlapable() = delete;
    __UTL_HIDE_FROM_ABI explicit(is_explicit_constructible_v<
        T>) inline constexpr conditionally_overlapable() noexcept(is_nothrow_default_constructible_v<T>)
    requires (is_default_constructible_v<T>)
    = default;

    template <typename... Args>
    __UTL_HIDE_FROM_ABI inline constexpr explicit conditionally_overlapable(
        in_place_t, Args&&... args)
        : data{__UTL forward<Args>(args)...} {}

    template <typename F, typename... Args>
    __UTL_HIDE_FROM_ABI inline constexpr explicit conditionally_overlapable(
        converting_t, F&& f, Args&&... args)
        : data{__UTL invoke(__UTL forward<F>(f), __UTL forward<Args>(args)...)} {}

    UTL_ATTRIBUTE(NO_UNIQUE_ADDRESS) T data;
};

template <typename T>
struct conditionally_overlapable<false, T> {
    __UTL_HIDE_FROM_ABI inline constexpr conditionally_overlapable() = delete;
    __UTL_HIDE_FROM_ABI explicit(is_explicit_constructible_v<
        T>) inline constexpr conditionally_overlapable() noexcept(is_nothrow_default_constructible_v<T>)
    requires (is_default_constructible_v<T>)
    = default;

    template <typename... Args>
    __UTL_HIDE_FROM_ABI inline constexpr explicit conditionally_overlapable(
        in_place_t, Args&&... args)
        : data{__UTL forward<Args>(args)...} {}

    template <typename F, typename... Args>
    __UTL_HIDE_FROM_ABI inline constexpr explicit conditionally_overlapable(
        converting_t, F&& f, Args&&... args)
        : data{__UTL invoke(__UTL forward<F>(f), __UTL forward<Args>(args)...)} {}

    T data;
};

template <typename T, typename E>
union data_union {
    __UTL_HIDE_FROM_ABI inline constexpr data_union(data_union const&) = delete;
    __UTL_HIDE_FROM_ABI inline constexpr data_union(data_union const&) noexcept(
        is_nothrow_copy_constructible_v<T> && is_nothrow_copy_constructible_v<E>)
    requires (is_copy_constructible_v<T> && is_copy_constructible_v<E> &&
                 is_trivially_copy_constructible_v<T> && is_trivially_copy_constructible_v<E>)
    = default;
    __UTL_HIDE_FROM_ABI inline constexpr data_union(data_union&&) = delete;
    __UTL_HIDE_FROM_ABI inline constexpr data_union(data_union&&) noexcept(
        is_nothrow_move_constructible_v<T> && is_nothrow_move_constructible_v<E>)
    requires (is_move_constructible_v<T> && is_move_constructible_v<E> &&
                 is_trivially_move_constructible_v<T> && is_trivially_move_constructible_v<E>)
    = default;
    __UTL_HIDE_FROM_ABI inline constexpr data_union& operator=(data_union const&) = delete;
    __UTL_HIDE_FROM_ABI inline constexpr data_union& operator=(data_union const&) noexcept(
        is_nothrow_copy_assignable_v<T> && is_nothrow_copy_assignable_v<E>)
    requires (is_copy_assignable_v<T> && is_copy_assignable_v<E> &&
                 is_trivially_copy_assignable_v<T> && is_trivially_copy_assignable_v<E>)
    = default;
    __UTL_HIDE_FROM_ABI inline constexpr data_union& operator=(data_union&&) = delete;
    __UTL_HIDE_FROM_ABI inline constexpr data_union& operator=(data_union&&) noexcept(
        is_nothrow_move_assignable_v<T> && is_nothrow_move_assignable_v<E>)
    requires (is_move_assignable_v<T> && is_move_assignable_v<E> &&
                 is_trivially_move_assignable_v<T> && is_trivially_move_assignable_v<E>)
    = default;
    template <typename... Args>
    __UTL_HIDE_FROM_ABI inline constexpr explicit data_union(in_place_t, Args&&... args) noexcept(
        is_nothrow_constructible_v<T, Args...>)
        : value{__UTL forward<Args>(args)...} {}
    template <typename... Args>
    __UTL_HIDE_FROM_ABI inline constexpr explicit data_union(unexpect_t, Args&&... args) noexcept(
        is_nothrow_constructible_v<E, Args...>)
        : error{__UTL forward<Args>(args)...} {}

    template <typename F, typename... Args>
    __UTL_HIDE_FROM_ABI inline constexpr explicit data_union(
        transforming_t, F&& f, Args&&... args) noexcept(is_nothrow_invocable_v<F, Args...>)
        : value{__UTL invoke(__UTL forward<F>(f), __UTL forward<Args>(args)...)} {}

    template <typename F, typename... Args>
    __UTL_HIDE_FROM_ABI inline constexpr explicit data_union(
        transforming_error_t, F&& f, Args&&... args) noexcept(is_nothrow_invocable_v<F, Args...>)
        : error{__UTL invoke(__UTL forward<F>(f), __UTL forward<Args>(args)...)} {}

    __UTL_HIDE_FROM_ABI inline constexpr ~data_union() noexcept
    requires (is_trivially_destructible_v<T> && is_trivially_destructible_v<E>)
    = default;
    __UTL_HIDE_FROM_ABI inline constexpr ~data_union() noexcept {}

    UTL_ATTRIBUTE(NO_UNIQUE_ADDRESS) T value;
    UTL_ATTRIBUTE(NO_UNIQUE_ADDRESS) E error;
};

/**
 * The implemenation essentially creates 2 ABIs for storage:
 * 1. The boolean flag is placed in the tail padding (if any) of the data_union and
 * the tail padding (if any) of storage_base is not usable by external objects
 * 2. The boolean flag is not placed in the tail padding (if any) of the data_union and
 * the tail padding (if any) of storage_base is usable by external objects
 *
 * In case 1, we can transparently replace the whole of the container that contains both
 * the union and flag when calling swap or emplace, they are under the purview of storage_base
 *
 * In case 2, when calling swap or emplace, we cannot replace the whole container since,
 * some other object may be occupying the tail padding of the container (and in turn storage_base);
 * doing so could overwrite the external data when constructing/destroying the whole container
 * during replacement. As such, we only replace the union itself and manually set the flag.
 */
template <typename T, typename E>
class storage_base {
    static_assert(!is_same_v<remove_cvref_t<T>, empty_t>, "Invalid value type");
    using data_type = data_union<T, E>;
    static constexpr bool place_flag_in_tail = fits_in_tail_padding_v<data_type, bool>;
    static constexpr bool allow_external_overlap = !place_flag_in_tail;

public:
    struct container {
        template <typename... Args>
        __UTL_HIDE_FROM_ABI inline constexpr explicit container(in_place_t, Args&&... args)
            : union_{__UTL in_place, __UTL in_place, __UTL forward<Args>(args)...}
            , has_value_{true} {}

        template <typename... Args>
        __UTL_HIDE_FROM_ABI inline constexpr explicit container(unexpect_t, Args&&... args)
            : union_{__UTL in_place, __UTL unexpect, __UTL forward<Args>(args)...}
            , has_value_{false} {}

        template <typename... Args>
        __UTL_HIDE_FROM_ABI inline constexpr explicit container(transforming_t, Args&&... args)
        requires (allow_external_overlap)
            : union_{__UTL in_place, __UTL details::expected::transforming,
                  __UTL forward<Args>(args)...}
            , has_value_{true} {}

        template <typename... Args>
        __UTL_HIDE_FROM_ABI inline constexpr explicit container(
            transforming_error_t, Args&&... args)
        requires (allow_external_overlap)
            : union_{__UTL in_place, __UTL details::expected::transforming_error,
                  __UTL forward<Args>(args)...}
            , has_value_{false} {}

        template <typename U>
        __UTL_HIDE_FROM_ABI inline constexpr explicit container(converting_t, bool has_value,
            U&& u) noexcept(noexcept(make_from_union<data_type>(has_value, __UTL declval<U>())))
        requires (allow_external_overlap)
            : union_{__UTL details::expected::converting,
                  [&]() { return make_from_union<data_type>(has_value, __UTL forward<U>(u)); }}
            , has_value_(has_value) {}

        __UTL_HIDE_FROM_ABI inline constexpr container(container const&) = delete;
        __UTL_HIDE_FROM_ABI inline constexpr container(container const&) noexcept
        requires (is_copy_constructible_v<T> && is_trivially_copy_constructible_v<T> &&
                     is_copy_constructible_v<E> && is_trivially_copy_constructible_v<E>)
        = default;
        __UTL_HIDE_FROM_ABI inline constexpr container(container&&) = delete;
        __UTL_HIDE_FROM_ABI inline constexpr container(container&&) noexcept
        requires (is_move_constructible_v<T> && is_trivially_move_constructible_v<T> &&
                     is_move_constructible_v<E> && is_trivially_move_constructible_v<E>)
        = default;
        __UTL_HIDE_FROM_ABI inline constexpr container& operator=(container const&) = delete;
        __UTL_HIDE_FROM_ABI inline constexpr container& operator=(container const&) noexcept
        requires (is_copy_assignable_v<T> && is_trivially_copy_assignable_v<T> &&
                     is_copy_assignable_v<E> && is_trivially_copy_assignable_v<E>)
        = default;
        __UTL_HIDE_FROM_ABI inline constexpr container& operator=(container&&) = delete;
        __UTL_HIDE_FROM_ABI inline constexpr container& operator=(container&&) noexcept
        requires (is_move_assignable_v<T> && is_trivially_move_assignable_v<T> &&
                     is_move_assignable_v<E> && is_trivially_move_assignable_v<E>)
        = default;

        __UTL_HIDE_FROM_ABI inline constexpr ~container() noexcept
        requires (is_trivially_destructible_v<T> && is_trivially_destructible_v<E>)
        = default;
        __UTL_HIDE_FROM_ABI inline constexpr ~container() noexcept
        requires (!is_trivially_destructible_v<T> || !is_trivially_destructible_v<E>)
        {
            destroy_member();
        }

        __UTL_HIDE_FROM_ABI inline constexpr void destroy_union() noexcept
        requires (allow_external_overlap && is_trivially_destructible_v<T> &&
            is_trivially_destructible_v<E>)
        {
            __UTL destroy_at(__UTL addressof(union_.data));
        }

        __UTL_HIDE_FROM_ABI inline constexpr void destroy_union() noexcept
        requires (allow_external_overlap &&
            (!is_trivially_destructible_v<T> || !is_trivially_destructible_v<E>))
        {
            destroy_member();
            __UTL destroy_at(__UTL addressof(union_.data));
        }

        template <typename... Args>
        __UTL_HIDE_FROM_ABI inline constexpr void construct_union(
            __UTL in_place_t, Args&&... args) noexcept(is_nothrow_constructible_v<T, Args...>)
        requires (allow_external_overlap)
        {
            __UTL construct_at(
                __UTL addressof(union_.data), __UTL in_place, __UTL forward<Args>(args)...);
            has_value_ = true;
        }

        template <typename... Args>
        __UTL_HIDE_FROM_ABI inline constexpr void construct_union(
            __UTL unexpect_t, Args&&... args) noexcept(is_nothrow_constructible_v<E, Args...>)
        requires (allow_external_overlap)
        {
            __UTL construct_at(
                __UTL addressof(union_.data), __UTL unexpect, __UTL forward<Args>(args)...);
            has_value_ = false;
        }

        UTL_ATTRIBUTES(NO_UNIQUE_ADDRESS) conditionally_overlapable<place_flag_in_tail, data_type> union_;
        UTL_ATTRIBUTES(NO_UNIQUE_ADDRESS) bool has_value_;

    private:
        __UTL_HIDE_FROM_ABI inline constexpr void destroy_member() noexcept {
            if (has_value_) {
                __UTL destroy_at(__UTL addressof(union_.data.value));
            } else {
                __UTL destroy_at(__UTL addressof(union_.data.error));
            }
        }
    };

    template <typename U>
    __UTL_HIDE_FROM_ABI static inline constexpr container
    make_container(bool has_value, U&& u) noexcept(
        is_nothrow_constructible_v<container, in_place_t, decltype(__UTL declval<U>().value)> &&
        is_nothrow_constructible_v<container, unexpect_t, decltype(__UTL declval<U>().error)>)
    requires (place_flag_in_tail)
    {
        return has_value ? container{__UTL in_place, __UTL forward_like<U>(u.value)}
                         : container{__UTL unexpect, __UTL forward_like<U>(u.error)};
    }

public:
    __UTL_HIDE_FROM_ABI inline constexpr ~storage_base() noexcept = default;
    __UTL_HIDE_FROM_ABI inline constexpr storage_base(storage_base const&) = delete;
    __UTL_HIDE_FROM_ABI inline constexpr storage_base(storage_base const&) noexcept
    requires (is_copy_constructible_v<T> && is_copy_constructible_v<E> &&
                 is_trivially_copy_constructible_v<T> && is_trivially_copy_constructible_v<E>)
    = default;

    __UTL_HIDE_FROM_ABI inline constexpr storage_base(storage_base const& other) noexcept(
        is_nothrow_copy_constructible_v<T> && is_nothrow_copy_constructible_v<E>)
    requires (is_copy_constructible_v<T> && is_copy_constructible_v<E> &&
        !(is_trivially_copy_constructible_v<T> && is_trivially_copy_constructible_v<E>))
        : storage_base(__UTL details::expected::converting, other.has_value(), other.data_ref()) {}

    __UTL_HIDE_FROM_ABI inline constexpr storage_base(storage_base&&) = delete;
    __UTL_HIDE_FROM_ABI inline constexpr storage_base(storage_base&&) noexcept
    requires (is_move_constructible_v<T> && is_move_constructible_v<E> &&
                 is_trivially_move_constructible_v<T> && is_trivially_move_constructible_v<E>)
    = default;

    __UTL_HIDE_FROM_ABI inline constexpr storage_base(storage_base&& other) noexcept(
        is_nothrow_move_constructible_v<T> && is_nothrow_move_constructible_v<E>)
    requires (is_move_constructible_v<T> && is_move_constructible_v<E> &&
        !(is_trivially_move_constructible_v<T> && is_trivially_move_constructible_v<E>))
        : storage_base(__UTL details::expected::converting, other.has_value(),
              __UTL move(other.data_ref())) {}

protected:
    template <typename... Args>
    __UTL_HIDE_FROM_ABI inline constexpr explicit storage_base(in_place_t, Args&&... args) noexcept(
        is_nothrow_constructible_v<container, in_place_t, Args...>)
        : container_{__UTL in_place, __UTL in_place, __UTL forward<Args>(args)...} {}
    template <typename... Args>
    __UTL_HIDE_FROM_ABI inline constexpr explicit storage_base(unexpect_t, Args&&... args) noexcept(
        is_nothrow_constructible_v<container, unexpect_t, Args...>)
        : container_{__UTL in_place, __UTL unexpect, __UTL forward<Args>(args)...} {}

    template <typename... Args>
    __UTL_HIDE_FROM_ABI inline constexpr explicit storage_base(transforming_t,
        Args&&... args) noexcept(is_nothrow_constructible_v<container, transforming_t, Args...>)
    requires (allow_external_overlap)
        : container_{__UTL in_place, __UTL details::expected::transforming,
              __UTL forward<Args>(args)...} {}
    template <typename... Args>
    __UTL_HIDE_FROM_ABI inline constexpr explicit storage_base(
        transforming_error_t, Args&&... args) noexcept(is_nothrow_constructible_v<container,
        transforming_error_t, Args...>)
    requires (allow_external_overlap)
        : container_{__UTL in_place, __UTL details::expected::transforming_error,
              __UTL forward<Args>(args)...} {}

    template <typename U>
    __UTL_HIDE_FROM_ABI inline constexpr storage_base(converting_t, bool has_value, U&& u) noexcept(
        is_nothrow_constructible_v<container, converting_t, bool, U>)
    requires (allow_external_overlap)
        : container_{__UTL in_place, __UTL details::expected::converting, has_value,
              __UTL forward<U>(u)} {}

    template <typename F, typename... Args>
    __UTL_HIDE_FROM_ABI inline constexpr explicit storage_base(transforming_t, F&& f,
        Args&&... args) noexcept(is_nothrow_constructible_v<container, in_place_t, in_place_t,
                                     invoke_result_t<F, Args...>> &&
        is_nothrow_invocable_v<F, Args...>)
    requires (place_flag_in_tail)
        : container_{__UTL details::expected::converting, [&]() {
                         return container{__UTL in_place,
                             __UTL invoke(__UTL forward<F>(f), __UTL forward<Args>(args)...)};
                     }} {}

    template <typename F, typename... Args>
    __UTL_HIDE_FROM_ABI inline constexpr explicit storage_base(transforming_error_t, F&& f,
        Args&&... args) noexcept(is_nothrow_constructible_v<container, in_place_t, unexpect_t,
                                     invoke_result_t<F, Args...>> &&
        is_nothrow_invocable_v<F, Args...>)
    requires (place_flag_in_tail)
        : container_{__UTL details::expected::converting, [&]() {
                         return container{__UTL unexpect,
                             __UTL invoke(__UTL forward<F>(f), __UTL forward<Args>(args)...)};
                     }} {}

    template <typename U>
    __UTL_HIDE_FROM_ABI inline constexpr storage_base(converting_t, bool has_value, U&& u) noexcept(
        noexcept(make_container(has_value, __UTL declval<U>())))
    requires (place_flag_in_tail)
        : container_{__UTL details::expected::converting,
              [&]() { return make_container(has_value, __UTL forward<U>(u)); }} {}

    __UTL_HIDE_FROM_ABI inline constexpr storage_base& operator=(storage_base const&) = delete;
    __UTL_HIDE_FROM_ABI inline constexpr storage_base& operator=(storage_base const&) noexcept
    requires (is_copy_assignable_v<T> && is_copy_assignable_v<E> &&
                 is_trivially_copy_assignable_v<T> && is_trivially_copy_assignable_v<E>)
    = default;
    __UTL_HIDE_FROM_ABI inline constexpr storage_base& operator=(
        storage_base const& other) noexcept(is_nothrow_copy_assignable_v<T> &&
        is_nothrow_copy_assignable_v<E>)
    requires (is_copy_assignable_v<T> && is_copy_assignable_v<E> &&
        !(is_trivially_copy_assignable_v<T> && is_trivially_copy_assignable_v<E>))
    {
        if (this->has_value() != other.has_value()) {
            if (other.has_value()) {
                this->destroy();
                this->construct_value(other.value_ref());
            } else {
                this->destroy();
                this->construct_error(other.error_ref());
            }
        } else if (other.has_value()) {
            this->value_ref() = other.value_ref();
        } else {
            this->error_ref() = other.error_ref();
        }

        return *this;
    }

    __UTL_HIDE_FROM_ABI inline constexpr storage_base& operator=(storage_base&&) = delete;
    __UTL_HIDE_FROM_ABI inline constexpr storage_base& operator=(storage_base&&) noexcept
    requires (is_move_assignable_v<T> && is_move_assignable_v<E> &&
                 is_trivially_move_assignable_v<T> && is_trivially_move_assignable_v<E>)
    = default;
    __UTL_HIDE_FROM_ABI inline constexpr storage_base& operator=(storage_base&& other) noexcept(
        is_nothrow_move_assignable_v<T> && is_nothrow_move_assignable_v<E>)
    requires (is_move_assignable_v<T> && is_move_assignable_v<E> &&
        !(is_trivially_move_assignable_v<T> && is_trivially_move_assignable_v<E>))
    {
        if (this->has_value() != other.has_value()) {
            if (other.has_value()) {
                this->destroy();
                this->construct_value(__UTL move(other.value_ref()));
            } else {
                this->destroy();
                this->construct_error(__UTL move(other.error_ref()));
            }
        } else if (other.has_value()) {
            this->value_ref() = __UTL move(other.value_ref());
        } else {
            this->error_ref() = __UTL move(other.error_ref());
        }

        return *this;
    }

    UTL_ATTRIBUTE(GETTER) inline constexpr data_type const& data_ref() const& noexcept {
        return container_.data.union_.data;
    }
    UTL_ATTRIBUTE(GETTER) inline constexpr data_type& data_ref() & noexcept {
        return container_.data.union_.data;
    }
    UTL_ATTRIBUTE(GETTER) inline constexpr data_type const&& data_ref() const&& noexcept {
        return __UTL move(container_.data.union_.data);
    }
    UTL_ATTRIBUTE(GETTER) inline constexpr data_type&& data_ref() && noexcept {
        return __UTL move(container_.data.union_.data);
    }

    UTL_ATTRIBUTE(GETTER) inline constexpr T const* value_ptr() const noexcept {
        return __UTL addressof(container_.data.union_.data.value);
    }
    UTL_ATTRIBUTE(GETTER) inline constexpr T* value_ptr() noexcept {
        return __UTL addressof(container_.data.union_.data.value);
    }
    UTL_ATTRIBUTE(GETTER) inline constexpr E const* error_ptr() const noexcept {
        return __UTL addressof(container_.data.union_.data.error);
    }
    UTL_ATTRIBUTE(GETTER) inline constexpr E* error_ptr() noexcept {
        return __UTL addressof(container_.data.union_.data.error);
    }

    UTL_ATTRIBUTE(GETTER) inline constexpr T const& value_ref() const& noexcept {
        return container_.data.union_.data.value;
    }
    UTL_ATTRIBUTE(GETTER) inline constexpr T& value_ref() & noexcept {
        return container_.data.union_.data.value;
    }
    UTL_ATTRIBUTE(GETTER) inline constexpr T const&& value_ref() const&& noexcept {
        return __UTL move(container_.data.union_.data.value);
    }
    UTL_ATTRIBUTE(GETTER) inline constexpr T&& value_ref() && noexcept {
        return __UTL move(container_.data.union_.data.value);
    }
    UTL_ATTRIBUTE(GETTER) inline constexpr E const& error_ref() const& noexcept {
        return container_.data.union_.data.error;
    }
    UTL_ATTRIBUTE(GETTER) inline constexpr E& error_ref() & noexcept {
        return container_.data.union_.data.error;
    }
    UTL_ATTRIBUTE(GETTER) inline constexpr E const&& error_ref() const&& noexcept {
        return __UTL move(container_.data.union_.data.error);
    }
    UTL_ATTRIBUTE(GETTER) inline constexpr E&& error_ref() && noexcept {
        return __UTL move(container_.data.union_.data.error);
    }

    UTL_ATTRIBUTE(GETTER) inline constexpr bool has_value() const noexcept {
        return container_.data.has_value_;
    }

    template <typename... Args>
    __UTL_HIDE_FROM_ABI inline constexpr T& emplace_value(Args&&... args) noexcept {
        static_assert(is_nothrow_constructible_v<T, Args...>);
        destroy();
        return *construct_value(__UTL forward<Args>(args)...);
    }

    template <typename... Args>
    __UTL_HIDE_FROM_ABI inline constexpr T* reinitialize_as_value(Args&&... args) noexcept(
        UTL_TRAIT_is_nothrow_constructible(T, Args...)) {
        UTL_ASSERT(!has_value());
        if constexpr (UTL_TRAIT_is_nothrow_constructible(T, Args...)) {
            destroy();
            auto ptr = construct_value(__UTL forward<Args>(args)...);
            return ptr;
        } else {
            E backup(__UTL move(error_ref()));
            destroy();
            UTL_TRY {
                auto ptr = construct_value(__UTL forward<Args>(args)...);
                return ptr;
            } UTL_CATCH(...) {
                construct_error(__UTL move(backup));
                UTL_RETHROW();
            }
        }
    }

    template <typename... Args>
    __UTL_HIDE_FROM_ABI inline constexpr E* reinitialize_as_error(Args&&... args) noexcept(
        UTL_TRAIT_is_nothrow_constructible(E, Args...)) {
        UTL_ASSERT(has_value());
        if constexpr (UTL_TRAIT_is_nothrow_constructible(E, Args...)) {
            destroy();
            auto ptr = construct_error(__UTL forward<Args>(args)...);
            return ptr;
        } else {
            T backup(__UTL move(value_ref()));
            destroy();
            UTL_TRY {
                auto ptr = construct_error(__UTL forward<Args>(args)...);
                return ptr;
            } UTL_CATCH(...) {
                construct_value(__UTL move(backup));
                UTL_RETHROW();
            }
        }
    }

    __UTL_HIDE_FROM_ABI inline constexpr void cross_swap(storage_base& other) noexcept(
        UTL_TRAIT_is_nothrow_move_constructible(T) && UTL_TRAIT_is_nothrow_move_constructible(E))
    requires (swappable<T> && swappable<E> && is_move_constructible_v<T> &&
        is_move_constructible_v<E> &&
        (is_nothrow_move_constructible_v<T> || is_nothrow_move_constructible_v<E>))
    {
        UTL_ASSERT(has_value() && !other.has_value());
        if constexpr (is_nothrow_move_constructible_v<E>) {
            E tmp(__UTL move(other.error_ref()));
            other.destroy();
            UTL_TRY {
                other.construct_value(__UTL move(this->value_ref()));
                this->destroy();
                this->construct_error(__UTL move(tmp));
            } UTL_CATCH(...) {
                other.construct_error(__UTL move(tmp));
                UTL_RETHROW();
            }
        } else {
            T tmp(__UTL move(this->value_ref()));
            this->destroy();
            UTL_TRY {
                this->construct_error(__UTL move(other.error_ref()));
                other.destroy();
                other.construct_value(__UTL move(tmp));
            } UTL_CATCH(...) {
                this->construct_value(__UTL move(tmp));
                UTL_RETHROW();
            }
        }
    }

private:
    template <typename... Args>
    __UTL_HIDE_FROM_ABI inline constexpr E* construct_error(Args&&... args) noexcept(
        is_nothrow_constructible_v<E, Args...>) {
        if constexpr (place_flag_in_tail) {
            return __UTL construct_at(
                __UTL addressof(container_.data), __UTL unexpect, __UTL forward<Args>(args)...);
        } else {
            container_.data.construct_union(__UTL unexpect, __UTL forward<Args>(args)...);
        }
    }

    template <typename... Args>
    __UTL_HIDE_FROM_ABI inline constexpr T* construct_value(Args&&... args) noexcept(
        is_nothrow_constructible_v<T, Args...>) {
        if constexpr (place_flag_in_tail) {
            return __UTL construct_at(
                __UTL addressof(container_.data), __UTL in_place, __UTL forward<Args>(args)...);
        } else {
            container_.data.construct_union(__UTL in_place, __UTL forward<Args>(args)...);
        }
    }
    __UTL_HIDE_FROM_ABI inline constexpr void destroy() noexcept {
        if constexpr (place_flag_in_tail) {
            __UTL destroy_at(__UTL addressof(container_.data));
        } else {
            container_.data.destroy_union();
        }
    }
    UTL_ATTRIBUTE(NO_UNIQUE_ADDRESS) conditionally_overlapable<allow_external_overlap, container> container_;
};

template <typename E>
class void_storage_base {
    using data_type = data_union<empty_t, E>;
    static constexpr bool place_flag_in_tail = fits_in_tail_padding_v<data_type, bool>;
    static constexpr bool allow_external_overlap = !place_flag_in_tail;

    struct container {
        __UTL_HIDE_FROM_ABI inline constexpr container() noexcept : union_{}, has_value_{true} {};

        __UTL_HIDE_FROM_ABI inline constexpr explicit container(in_place_t tag)
            : union_{__UTL in_place, tag}
            , has_value_{true} {}

        template <typename... Args>
        __UTL_HIDE_FROM_ABI inline constexpr explicit container(unexpect_t, Args&&... args)
            : union_{__UTL in_place, __UTL unexpect, __UTL forward<Args>(args)...}
            , has_value_{false} {}

        template <typename... Args>
        __UTL_HIDE_FROM_ABI inline constexpr explicit container(
            transforming_error_t, Args&&... args)
            : union_{__UTL in_place, __UTL details::expected::transforming_error,
                  __UTL forward<Args>(args)...}
            , has_value_{false} {}

        template <typename U>
        __UTL_HIDE_FROM_ABI inline constexpr explicit container(bool has_value, U&& u) noexcept(
            noexcept(make_from_union<data_type>(has_value, __UTL declval<U>())))
        requires (allow_external_overlap)
            : union_{__UTL details::expected::converting,
                  [&]() { return make_from_union<data_type>(has_value, __UTL forward<U>(u)); }}
            , has_value_(has_value) {}

        __UTL_HIDE_FROM_ABI inline constexpr container(container const&) = delete;
        __UTL_HIDE_FROM_ABI inline constexpr container(container const&) noexcept
        requires (is_copy_constructible_v<E> && is_trivially_copy_constructible_v<E>)
        = default;
        __UTL_HIDE_FROM_ABI inline constexpr container(container&&) = delete;
        __UTL_HIDE_FROM_ABI inline constexpr container(container&&) noexcept
        requires (is_move_constructible_v<E> && is_trivially_move_constructible_v<E>)
        = default;
        __UTL_HIDE_FROM_ABI inline constexpr container& operator=(container const&) = delete;
        __UTL_HIDE_FROM_ABI inline constexpr container& operator=(container const&) noexcept
        requires (is_copy_assignable_v<E> && is_trivially_copy_assignable_v<E>)
        = default;
        __UTL_HIDE_FROM_ABI inline constexpr container& operator=(container&&) = delete;
        __UTL_HIDE_FROM_ABI inline constexpr container& operator=(container&&) noexcept
        requires (is_move_assignable_v<E> && is_trivially_move_assignable_v<E>)
        = default;

        __UTL_HIDE_FROM_ABI inline constexpr ~container() noexcept
        requires (is_trivially_destructible_v<E>)
        = default;
        __UTL_HIDE_FROM_ABI inline constexpr ~container() noexcept
        requires (!is_trivially_destructible_v<E>)
        {
            destroy_member();
        }

        __UTL_HIDE_FROM_ABI inline constexpr void destroy_union() noexcept
        requires (allow_external_overlap && is_trivially_destructible_v<E>)
        {
            __UTL destroy_at(__UTL addressof(union_.data));
        }

        __UTL_HIDE_FROM_ABI inline constexpr void destroy_union() noexcept
        requires (allow_external_overlap && !is_trivially_destructible_v<E>)
        {
            destroy_member();
            __UTL destroy_at(__UTL addressof(union_.data));
        }

        template <typename... Args>
        __UTL_HIDE_FROM_ABI inline constexpr void construct_union(__UTL in_place_t) noexcept
        requires (allow_external_overlap)
        {
            __UTL construct_at(__UTL addressof(union_.data), __UTL in_place);
            has_value_ = true;
        }

        template <typename... Args>
        __UTL_HIDE_FROM_ABI inline constexpr void construct_union(
            __UTL unexpect_t, Args&&... args) noexcept(is_nothrow_constructible_v<E, Args...>)
        requires (allow_external_overlap)
        {
            __UTL construct_at(
                __UTL addressof(union_.data), __UTL unexpect, __UTL forward<Args>(args)...);
            has_value_ = false;
        }

    private:
        __UTL_HIDE_FROM_ABI inline constexpr void destroy_member() noexcept {
            if (has_value_) {
                __UTL destroy_at(__UTL addressof(union_.data.value));
            } else {
                __UTL destroy_at(__UTL addressof(union_.data.error));
            }
        }

        UTL_ATTRIBUTES(NO_UNIQUE_ADDRESS) conditionally_overlapable<place_flag_in_tail, data_type> union_;
        UTL_ATTRIBUTES(NO_UNIQUE_ADDRESS) bool has_value_;
    };

    template <typename U>
    __UTL_HIDE_FROM_ABI static inline constexpr container
    make_container(bool has_value, U&& u) noexcept(
        is_nothrow_constructible_v<container, unexpect_t, decltype(__UTL declval<U>().error)>)
    requires (place_flag_in_tail)
    {
        return has_value ? container{__UTL in_place}
                         : container{__UTL unexpect, __UTL forward_like<U>(u.error)};
    }

protected:
    __UTL_HIDE_FROM_ABI inline constexpr void_storage_base() noexcept = default;
    __UTL_HIDE_FROM_ABI inline constexpr void_storage_base(void_storage_base const&) = delete;
    __UTL_HIDE_FROM_ABI inline constexpr void_storage_base(void_storage_base const&) noexcept
    requires (is_copy_constructible_v<E> && is_trivially_copy_constructible_v<E>)
    = default;
    __UTL_HIDE_FROM_ABI inline constexpr void_storage_base(void_storage_base const& other) noexcept(
        is_nothrow_copy_constructible_v<E>)
    requires (is_copy_constructible_v<E> && !is_trivially_copy_constructible_v<E>)
        : void_storage_base(
              __UTL details::expected::converting, other.has_value(), other.data_ref()) {}

    __UTL_HIDE_FROM_ABI inline constexpr void_storage_base(void_storage_base&&) = delete;
    __UTL_HIDE_FROM_ABI inline constexpr void_storage_base(void_storage_base&&) noexcept
    requires (is_move_constructible_v<E> && is_trivially_move_constructible_v<E>)
    = default;
    __UTL_HIDE_FROM_ABI inline constexpr void_storage_base(void_storage_base&& other) noexcept(
        is_nothrow_move_constructible_v<E>)
    requires (is_move_constructible_v<E> && !is_trivially_move_constructible_v<E>)
        : void_storage_base(__UTL details::expected::converting, other.has_value(),
              __UTL move(other.data_ref())) {}

    __UTL_HIDE_FROM_ABI inline constexpr explicit void_storage_base(in_place_t) noexcept
        : container_{} {}

    template <typename... Args>
    __UTL_HIDE_FROM_ABI inline constexpr explicit void_storage_base(unexpect_t,
        Args&&... args) noexcept(is_nothrow_constructible_v<container, unexpect_t, Args...>)
        : container_{__UTL in_place, __UTL unexpect, __UTL forward<Args>(args)...} {}

    template <typename... Args>
    __UTL_HIDE_FROM_ABI inline constexpr explicit void_storage_base(
        transforming_error_t, Args&&... args) noexcept(is_nothrow_constructible_v<container,
        transforming_error_t, Args...>)
    requires (allow_external_overlap)
        : container_{__UTL in_place, __UTL details::expected::transforming_error,
              __UTL forward<Args>(args)...} {}

    template <typename U>
    __UTL_HIDE_FROM_ABI inline constexpr void_storage_base(converting_t, bool has_value,
        U&& u) noexcept(is_nothrow_constructible_v<container, converting_t, bool, U>)
    requires (allow_external_overlap)
        : container_{__UTL in_place, __UTL details::expected::converting, has_value,
              __UTL forward<U>(u)} {}

    template <typename F, typename... Args>
    __UTL_HIDE_FROM_ABI inline constexpr explicit void_storage_base(transforming_error_t, F&& f,
        Args&&... args) noexcept(is_nothrow_constructible_v<container, in_place_t, unexpect_t,
                                     invoke_result_t<F, Args...>> &&
        is_nothrow_invocable_v<F, Args...>)
    requires (place_flag_in_tail)
        : container_{__UTL details::expected::converting, [&]() {
                         return container{__UTL unexpect,
                             __UTL invoke(__UTL forward<F>(f), __UTL forward<Args>(args)...)};
                     }} {}

    template <typename U>
    __UTL_HIDE_FROM_ABI inline constexpr void_storage_base(converting_t, bool has_value,
        U&& u) noexcept(noexcept(make_container(has_value, __UTL declval<U>())))
    requires (place_flag_in_tail)
        : container_{__UTL details::expected::converting,
              [&]() { return make_container(has_value, __UTL forward<U>(u)); }} {}

    __UTL_HIDE_FROM_ABI inline constexpr void_storage_base& operator=(
        void_storage_base const&) = delete;
    __UTL_HIDE_FROM_ABI inline constexpr void_storage_base& operator=(
        void_storage_base const&) noexcept
    requires (is_copy_assignable_v<E> && is_trivially_copy_assignable_v<E>)
    = default;
    __UTL_HIDE_FROM_ABI inline constexpr void_storage_base& operator=(
        void_storage_base const& other) noexcept(is_nothrow_copy_assignable_v<E>)
    requires (is_copy_assignable_v<E> && !is_trivially_copy_assignable_v<E>)
    {
        if (this->has_value() != other.has_value()) {
            if (other.has_value()) {
                this->destroy();
                this->construct_value(other.value_ref());
            } else {
                this->destroy();
                this->construct_error(other.error_ref());
            }
        } else if (other.has_value()) {
            this->value_ref() = other.value_ref();
        } else {
            this->error_ref() = other.error_ref();
        }

        return *this;
    }

    __UTL_HIDE_FROM_ABI inline constexpr void_storage_base& operator=(void_storage_base&&) = delete;
    __UTL_HIDE_FROM_ABI inline constexpr void_storage_base& operator=(void_storage_base&&) noexcept
    requires (is_move_assignable_v<E> && is_trivially_move_assignable_v<E>)
    = default;
    __UTL_HIDE_FROM_ABI inline constexpr void_storage_base& operator=(
        void_storage_base&& other) noexcept(is_nothrow_move_assignable_v<E>)
    requires (is_move_assignable_v<E> && !is_trivially_move_assignable_v<E>)
    {
        if (this->has_value() != other.has_value()) {
            if (other.has_value()) {
                this->destroy();
                this->construct_value(__UTL move(other.value_ref()));
            } else {
                this->destroy();
                this->construct_error(__UTL move(other.error_ref()));
            }
        } else if (other.has_value()) {
            this->value_ref() = __UTL move(other.value_ref());
        } else {
            this->error_ref() = __UTL move(other.error_ref());
        }

        return *this;
    }

    __UTL_HIDE_FROM_ABI inline constexpr ~void_storage_base() noexcept = default;

    UTL_ATTRIBUTE(GETTER) inline constexpr data_type const& data_ref() const& noexcept {
        return container_.data.union_.data;
    }
    UTL_ATTRIBUTE(GETTER) inline constexpr data_type& data_ref() & noexcept {
        return container_.data.union_.data;
    }
    UTL_ATTRIBUTE(GETTER) inline constexpr data_type const&& data_ref() const&& noexcept {
        return __UTL move(container_.data.union_.data);
    }
    UTL_ATTRIBUTE(GETTER) inline constexpr data_type&& data_ref() && noexcept {
        return __UTL move(container_.data.union_.data);
    }
    UTL_ATTRIBUTE(GETTER) inline constexpr E const* error_ptr() const noexcept {
        return __UTL addressof(container_.data.union_.data.error);
    }
    UTL_ATTRIBUTE(GETTER) inline constexpr E* error_ptr() noexcept {
        return __UTL addressof(container_.data.union_.data.error);
    }

    UTL_ATTRIBUTE(GETTER) inline constexpr E const& error_ref() const& noexcept {
        return container_.data.union_.data.error;
    }
    UTL_ATTRIBUTE(GETTER) inline constexpr E& error_ref() & noexcept {
        return container_.data.union_.data.error;
    }
    UTL_ATTRIBUTE(GETTER) inline constexpr E const&& error_ref() const&& noexcept {
        return __UTL move(container_.data.union_.data.error);
    }
    UTL_ATTRIBUTE(GETTER) inline constexpr E&& error_ref() && noexcept {
        return __UTL move(container_.data.union_.data.error);
    }

    UTL_ATTRIBUTE(GETTER) inline constexpr bool has_value() const noexcept {
        return container_.data.has_value_;
    }

    __UTL_HIDE_FROM_ABI inline constexpr void emplace_value() noexcept {
        destroy();
        construct_value();
    }

    __UTL_HIDE_FROM_ABI inline constexpr void reinitialize_as_value() noexcept {
        UTL_ASSERT(!has_value());
        destroy();
        construct_value();
    }

    template <typename... Args>
    __UTL_HIDE_FROM_ABI inline constexpr E* reinitialize_as_error(Args&&... args) noexcept(
        is_nothrow_constructible_v<E, Args...>) {
        UTL_ASSERT(has_value());
        if constexpr (UTL_TRAIT_is_nothrow_constructible(E, Args...)) {
            destroy();
            auto ptr = construct_error(__UTL forward<Args>(args)...);
            return ptr;
        } else {
            destroy();
            UTL_TRY {
                auto ptr = construct_error(__UTL forward<Args>(args)...);
                return ptr;
            } UTL_CATCH(...) {
                construct_value();
                UTL_RETHROW();
            }
        }
    }

    __UTL_HIDE_FROM_ABI inline constexpr void cross_swap(void_storage_base& other) noexcept(
        is_nothrow_move_constructible_v<E>)
    requires (is_move_constructible_v<E>)
    {
        UTL_ASSERT(has_value() && !other.has_value());
        this->reinitialize_as_error(__UTL move(other.error_ref()));
        other.reinitialize_as_value();
    }

private:
    template <typename... Args>
    __UTL_HIDE_FROM_ABI inline constexpr E* construct_error(Args&&... args) noexcept(
        is_nothrow_constructible_v<E, Args...>) {
        if constexpr (place_flag_in_tail) {
            return __UTL construct_at(
                __UTL addressof(container_.data), __UTL unexpect, __UTL forward<Args>(args)...);
        } else {
            container_.data.construct_union(__UTL unexpect, __UTL forward<Args>(args)...);
        }
    }

    __UTL_HIDE_FROM_ABI inline constexpr void construct_value() noexcept {
        if constexpr (place_flag_in_tail) {
            __UTL construct_at(__UTL addressof(container_.data), __UTL in_place);
        } else {
            container_.data.construct_union(__UTL in_place);
        }
    }

    __UTL_HIDE_FROM_ABI inline constexpr void destroy() noexcept {
        if constexpr (place_flag_in_tail) {
            __UTL destroy_at(__UTL addressof(container_.data));
        } else {
            container_.data.destroy_union();
        }
    }

    UTL_ATTRIBUTE(NO_UNIQUE_ADDRESS) conditionally_overlapable<allow_external_overlap, container> container_;
};

} // namespace expected
} // namespace details

UTL_NAMESPACE_END

#undef __UTL_ATTRIBUTE_GETTER
#undef __UTL_ATTRIBUTE_TYPE_AGGREGATE_GETTER
