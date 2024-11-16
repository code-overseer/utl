// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

#if !defined(UTL_EXPECTED_PRIVATE_HEADER_GUARD)
#  error "Private header accessed"
#endif

#include "utl/initializer_list/utl_initializer_list_fwd.h"

#include "utl/expected/utl_expected_common.h"
#include "utl/expected/utl_unexpected.h"
#include "utl/memory/utl_addressof.h"
#include "utl/ranges/swap.h"
#include "utl/type_traits/utl_constants.h"
#include "utl/type_traits/utl_enable_if.h"
#include "utl/type_traits/utl_invoke.h"
#include "utl/type_traits/utl_is_complete.h"
#include "utl/type_traits/utl_is_constructible.h"
#include "utl/type_traits/utl_is_convertible.h"
#include "utl/type_traits/utl_is_copy_constructible.h"
#include "utl/type_traits/utl_is_default_constructible.h"
#include "utl/type_traits/utl_is_destructible.h"
#include "utl/type_traits/utl_is_move_constructible.h"
#include "utl/type_traits/utl_is_nothrow_convertible.h"
#include "utl/type_traits/utl_is_nothrow_copy_assignable.h"
#include "utl/type_traits/utl_is_nothrow_copy_constructible.h"
#include "utl/type_traits/utl_is_nothrow_default_constructible.h"
#include "utl/type_traits/utl_is_nothrow_move_assignable.h"
#include "utl/type_traits/utl_is_nothrow_move_constructible.h"
#include "utl/type_traits/utl_is_same.h"
#include "utl/type_traits/utl_is_swappable.h"
#include "utl/type_traits/utl_is_trivially_constructible.h"
#include "utl/type_traits/utl_is_trivially_copy_assignable.h"
#include "utl/type_traits/utl_is_trivially_copy_constructible.h"
#include "utl/type_traits/utl_is_trivially_destructible.h"
#include "utl/type_traits/utl_is_trivially_move_assignable.h"
#include "utl/type_traits/utl_is_trivially_move_constructible.h"
#include "utl/type_traits/utl_logical_traits.h"
#include "utl/type_traits/utl_remove_cvref.h"
#include "utl/utility/utl_forward.h"
#include "utl/utility/utl_in_place.h"
#include "utl/utility/utl_move.h"

#include <new>

UTL_NAMESPACE_BEGIN

namespace details {
namespace expected {

#if UTL_CXX17
template <typename T>
__UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 void invoke_destructor(T* ptr) {
    if constexpr (!UTL_TRAIT_is_trivially_destructible(T)) {
        ptr->~T();
    }
}

template <typename T, typename... Args>
__UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 T* replace_object(T* ptr, Args&&... args) noexcept {
    if constexpr (UTL_TRAIT_conjunction(is_trivially_destructible<T>,
                      is_trivially_move_assignable<T>, is_trivially_constructible<T, Args...>)) {
        *ptr = T{__UTL forward<Args>(args)};
    } else {
        invoke_destructor(ptr);
        return ::new (ptr) T{__UTL forward<Args>(args)};
    }

    return ptr;
}

#else

template <typename T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_trivially_destructible(T))>
__UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 void invoke_destructor(T* ptr) {}

template <typename T UTL_CONSTRAINT_CXX11(!UTL_TRAIT_is_trivially_destructible(T))>
__UTL_HIDE_FROM_ABI inline void invoke_destructor(T* ptr) {
    ptr->~T();
}

template <typename T, typename... Args UTL_CONSTRAINT_CXX11(
    UTL_TRAIT_conjunction(is_trivially_destructible<T>, is_trivially_move_assignable<T>,
        is_trivially_constructible<T, Args...>))>
__UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 T* replace_object(T* ptr, Args&&... args) noexcept {
    *ptr = T{__UTL forward<Args>(args)};
    return ptr;
}

template <typename T, typename... Args UTL_CONSTRAINT_CXX11(
    !UTL_TRAIT_conjunction(is_trivially_destructible<T>, is_trivially_move_assignable<T>,
        is_trivially_constructible<T, Args...>))>
__UTL_HIDE_FROM_ABI inline T* replace_object(T* ptr, Args&&... args) noexcept(
    UTL_TRAIT_is_nothrow_constructible(T, Args...)) {
    invoke_destructor(ptr);
    return ::new (ptr) T{__UTL forward<Args>(args)};
}
#endif

template <typename T, typename U>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline constexpr T make_union(
    bool has_value, U&& arg) noexcept(UTL_TRAIT_is_nothrow_copy_constructible(in_place_t, T) &&
    UTL_TRAIT_is_nothrow_copy_constructible(unexpect_t, E)) {
    return has_value ? T{__UTL in_place, __UTL forward_like<U>(arg.value)}
                     : T{__UTL unexpect, __UTL forward_like<U>(arg.error)};
}

template <typename T, typename E, bool = is_default_constructible_v<T>,
    bool = is_trivially_destructible_v<T> && is_trivially_destructible_v<E>>
union data_union {
    T value;
    E error;

    __UTL_HIDE_FROM_ABI inline constexpr data_union() noexcept(
        UTL_TRAIT_is_nothrow_default_constructible(T))
        : value{} {}
    __UTL_HIDE_FROM_ABI inline constexpr data_union(data_union const&) noexcept(
        UTL_TRAIT_is_nothrow_copy_constructible(T) &&
        UTL_TRAIT_is_nothrow_copy_constructible(E)) = default;
    __UTL_HIDE_FROM_ABI inline constexpr data_union(data_union&&) noexcept(
        UTL_TRAIT_is_nothrow_move_constructible(T) &&
        UTL_TRAIT_is_nothrow_move_constructible(E)) = default;
    __UTL_HIDE_FROM_ABI inline constexpr data_union& operator=(data_union const&) noexcept(
        UTL_TRAIT_is_nothrow_copy_constructible(T) &&
        UTL_TRAIT_is_nothrow_copy_constructible(E)) = default;
    __UTL_HIDE_FROM_ABI inline constexpr data_union& operator=(data_union&&) noexcept(
        UTL_TRAIT_is_nothrow_move_constructible(T) &&
        UTL_TRAIT_is_nothrow_move_constructible(E)) = default;
    template <typename... Args>
    __UTL_HIDE_FROM_ABI inline constexpr explicit data_union(in_place_t, Args&&... args) noexcept(
        UTL_TRAIT_is_nothrow_constructible(T, Args...))
        : value{__UTL forward<Args>(args)...} {}
    template <typename... Args>
    __UTL_HIDE_FROM_ABI inline constexpr explicit data_union(unexpect_t, Args&&... args) noexcept(
        UTL_TRAIT_is_nothrow_constructible(E, Args...))
        : error{__UTL forward<Args>(args)...} {}
    template <typename F, typename... Args>
    __UTL_HIDE_FROM_ABI inline constexpr explicit data_union(
        transforming_t, F&& f, Args&&... args) noexcept(UTL_TRAIT_is_nothrow_invocable(F, Args...))
        : value{__UTL invoke(__UTL forward<F>(f), __UTL forward<Args>(args)...)} {}
    template <typename F, typename... Args>
    __UTL_HIDE_FROM_ABI inline constexpr explicit data_union(transforming_error_t, F&& f,
        Args&&... args) noexcept(UTL_TRAIT_is_nothrow_invocable(F, Args...))
        : error{__UTL invoke(__UTL forward<F>(f), __UTL forward<Args>(args)...)} {}
};

template <typename T, typename E>
union data_union<T, E, false, true> {
    T value;
    E error;

    __UTL_HIDE_FROM_ABI data_union() = delete;
    __UTL_HIDE_FROM_ABI inline constexpr data_union(data_union const&) noexcept(
        UTL_TRAIT_is_nothrow_copy_constructible(T) &&
        UTL_TRAIT_is_nothrow_copy_constructible(E)) = default;
    __UTL_HIDE_FROM_ABI inline constexpr data_union(data_union&&) noexcept(
        UTL_TRAIT_is_nothrow_move_constructible(T) &&
        UTL_TRAIT_is_nothrow_move_constructible(E)) = default;
    __UTL_HIDE_FROM_ABI inline constexpr data_union& operator=(data_union const&) noexcept(
        UTL_TRAIT_is_nothrow_copy_constructible(T) &&
        UTL_TRAIT_is_nothrow_copy_constructible(E)) = default;
    __UTL_HIDE_FROM_ABI inline constexpr data_union& operator=(data_union&&) noexcept(
        UTL_TRAIT_is_nothrow_move_constructible(T) &&
        UTL_TRAIT_is_nothrow_move_constructible(E)) = default;
    template <typename... Args>
    __UTL_HIDE_FROM_ABI inline constexpr explicit data_union(in_place_t, Args&&... args) noexcept(
        UTL_TRAIT_is_nothrow_constructible(T, Args...))
        : value{__UTL forward<Args>(args)...} {}
    template <typename... Args>
    __UTL_HIDE_FROM_ABI inline constexpr explicit data_union(unexpect_t, Args&&... args) noexcept(
        UTL_TRAIT_is_nothrow_constructible(E, Args...))
        : error{__UTL forward<Args>(args)...} {}
    template <typename F, typename... Args>
    __UTL_HIDE_FROM_ABI inline constexpr explicit data_union(
        transforming_t, F&& f, Args&&... args) noexcept(UTL_TRAIT_is_nothrow_invocable(F, Args...))
        : value{__UTL invoke(__UTL forward<F>(f), __UTL forward<Args>(args)...)} {}
    template <typename F, typename... Args>
    __UTL_HIDE_FROM_ABI inline constexpr explicit data_union(transforming_error_t, F&& f,
        Args&&... args) noexcept(UTL_TRAIT_is_nothrow_invocable(F, Args...))
        : error{__UTL invoke(__UTL forward<F>(f), __UTL forward<Args>(args)...)} {}
};

template <typename T, typename E>
union data_union<T, E, true, false> {
    T value;
    E error;

    __UTL_HIDE_FROM_ABI inline constexpr data_union() noexcept(
        UTL_TRAIT_is_nothrow_default_constructible(T))
        : value{} {}
    __UTL_HIDE_FROM_ABI inline constexpr data_union(data_union const&) noexcept(
        UTL_TRAIT_is_nothrow_copy_constructible(T) &&
        UTL_TRAIT_is_nothrow_copy_constructible(E)) = default;
    __UTL_HIDE_FROM_ABI inline constexpr data_union(data_union&&) noexcept(
        UTL_TRAIT_is_nothrow_move_constructible(T) &&
        UTL_TRAIT_is_nothrow_move_constructible(E)) = default;
    __UTL_HIDE_FROM_ABI inline constexpr data_union& operator=(data_union const&) noexcept(
        UTL_TRAIT_is_nothrow_copy_constructible(T) &&
        UTL_TRAIT_is_nothrow_copy_constructible(E)) = default;
    __UTL_HIDE_FROM_ABI inline constexpr data_union& operator=(data_union&&) noexcept(
        UTL_TRAIT_is_nothrow_move_constructible(T) &&
        UTL_TRAIT_is_nothrow_move_constructible(E)) = default;
    template <typename... Args>
    __UTL_HIDE_FROM_ABI inline constexpr explicit data_union(in_place_t, Args&&... args) noexcept(
        UTL_TRAIT_is_nothrow_constructible(T, Args...))
        : value{__UTL forward<Args>(args)...} {}
    template <typename... Args>
    __UTL_HIDE_FROM_ABI inline constexpr explicit data_union(unexpect_t, Args&&... args) noexcept(
        UTL_TRAIT_is_nothrow_constructible(E, Args...))
        : error{__UTL forward<Args>(args)...} {}
    template <typename F, typename... Args>
    __UTL_HIDE_FROM_ABI inline constexpr explicit data_union(
        transforming_t, F&& f, Args&&... args) noexcept(UTL_TRAIT_is_nothrow_invocable(F, Args...))
        : value{__UTL invoke(__UTL forward<F>(f), __UTL forward<Args>(args)...)} {}
    template <typename F, typename... Args>
    __UTL_HIDE_FROM_ABI inline constexpr explicit data_union(transforming_error_t, F&& f,
        Args&&... args) noexcept(UTL_TRAIT_is_nothrow_invocable(F, Args...))
        : error{__UTL invoke(__UTL forward<F>(f), __UTL forward<Args>(args)...)} {}
    __UTL_HIDE_FROM_ABI inline ~data_union() {}
};

template <typename T, typename E>
union data_union<T, E, false, false> {
    T value;
    E error;

    __UTL_HIDE_FROM_ABI data_union() = delete;
    __UTL_HIDE_FROM_ABI inline constexpr data_union(data_union const&) noexcept(
        UTL_TRAIT_is_nothrow_copy_constructible(T) &&
        UTL_TRAIT_is_nothrow_copy_constructible(E)) = default;
    __UTL_HIDE_FROM_ABI inline constexpr data_union(data_union&&) noexcept(
        UTL_TRAIT_is_nothrow_move_constructible(T) &&
        UTL_TRAIT_is_nothrow_move_constructible(E)) = default;
    __UTL_HIDE_FROM_ABI inline constexpr data_union& operator=(data_union const&) noexcept(
        UTL_TRAIT_is_nothrow_copy_constructible(T) &&
        UTL_TRAIT_is_nothrow_copy_constructible(E)) = default;
    __UTL_HIDE_FROM_ABI inline constexpr data_union& operator=(data_union&&) noexcept(
        UTL_TRAIT_is_nothrow_move_constructible(T) &&
        UTL_TRAIT_is_nothrow_move_constructible(E)) = default;
    template <typename... Args>
    __UTL_HIDE_FROM_ABI inline constexpr explicit data_union(in_place_t, Args&&... args) noexcept(
        UTL_TRAIT_is_nothrow_constructible(T, Args...))
        : value{__UTL forward<Args>(args)...} {}
    template <typename... Args>
    __UTL_HIDE_FROM_ABI inline constexpr explicit data_union(unexpect_t, Args&&... args) noexcept(
        UTL_TRAIT_is_nothrow_constructible(E, Args...))
        : error{__UTL forward<Args>(args)...} {}
    template <typename F, typename... Args>
    __UTL_HIDE_FROM_ABI inline constexpr explicit data_union(
        transforming_t, F&& f, Args&&... args) noexcept(UTL_TRAIT_is_nothrow_invocable(F, Args...))
        : value{__UTL invoke(__UTL forward<F>(f), __UTL forward<Args>(args)...)} {}
    template <typename F, typename... Args>
    __UTL_HIDE_FROM_ABI inline constexpr explicit data_union(transforming_error_t, F&& f,
        Args&&... args) noexcept(UTL_TRAIT_is_nothrow_invocable(F, Args...))
        : error{__UTL invoke(__UTL forward<F>(f), __UTL forward<Args>(args)...)} {}
    __UTL_HIDE_FROM_ABI inline ~data_union() {}
};

template <typename T, typename E>
class storage_base {
    using data_type = data_union<T, E>;

protected:
    static constexpr struct protected_tag_t {
        explicit constexpr protected_tag_t() noexcept = default;
    } protected_tag{};

    template <typename... Args>
    __UTL_HIDE_FROM_ABI inline constexpr storage_base(in_place_t, Args&&... args) noexcept(
        is_nothrow_constructible_v<T, Args...>)
        : data_{__UTL in_place, __UTL forward<Args>(args)...}
        , has_value_{true} {}

    template <typename... Args>
    __UTL_HIDE_FROM_ABI inline constexpr storage_base(unexpect_t, Args&&... args) noexcept(
        is_nothrow_constructible_v<E, Args...>)
        : data_{__UTL unexpect, __UTL forward<Args>(args)...}
        , has_value_{false} {}

    template <typename F, typename... Args>
    __UTL_HIDE_FROM_ABI inline constexpr explicit storage_base(
        transforming_t, F&& f, Args&&... args) noexcept(UTL_TRAIT_is_nothrow_invocable(F, Args...))
        : data_{__UTL details::expected::transforming, __UTL forward<F>(f),
              __UTL forward<Args>(args)...}
        , has_value_{true} {}
    template <typename F, typename... Args>
    __UTL_HIDE_FROM_ABI inline constexpr explicit storage_base(transforming_error_t, F&& f,
        Args&&... args) noexcept(UTL_TRAIT_is_nothrow_invocable(F, Args...))
        : data_{__UTL details::expected::transforming_error, __UTL forward<F>(f),
              __UTL forward<Args>(args)...}
        , has_value_{false} {}

    template <typename U>
    __UTL_HIDE_FROM_ABI inline constexpr storage_base(protected_tag_t, bool has_value,
        U&& data) noexcept(noexcept(make_union(has_value, __UTL forward<U>(data))))
        : data_{make_union<data_type>(has_value, __UTL forward<U>(data))}
        , has_value_{has_value} {}

    __UTL_HIDE_FROM_ABI inline constexpr storage_base(storage_base const&) noexcept(
        UTL_TRAIT_is_nothrow_copy_constructible(T) &&
        UTL_TRAIT_is_nothrow_copy_constructible(E)) = default;
    __UTL_HIDE_FROM_ABI inline constexpr storage_base(storage_base&&) noexcept(
        UTL_TRAIT_is_nothrow_move_constructible(T) &&
        UTL_TRAIT_is_nothrow_move_constructible(E)) = default;
    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 storage_base& operator=(storage_base const&) noexcept(
        UTL_TRAIT_is_nothrow_copy_assignable(T) &&
        UTL_TRAIT_is_nothrow_copy_assignable(E)) = default;
    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 storage_base& operator=(storage_base&&) noexcept(
        UTL_TRAIT_is_nothrow_move_assignable(T) &&
        UTL_TRAIT_is_nothrow_move_assignable(E)) = default;

    __UTL_HIDE_FROM_ABI inline ~storage_base() = default;

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline constexpr data_union const&
    data_ref() const& noexcept {
        return data_;
    }

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline UTL_CONSTEXPR_CXX14 data_union& data_ref() & noexcept {
        return data_;
    }
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline constexpr T const* value_ptr() const noexcept {
        return __UTL addressof(data_.value);
    }
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline UTL_CONSTEXPR_CXX14 T* value_ptr() noexcept {
        return __UTL addressof(data_.value);
    }
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline constexpr T const* error_ptr() const noexcept {
        return __UTL addressof(data_.error);
    }
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline UTL_CONSTEXPR_CXX14 T* error_ptr() noexcept {
        return __UTL addressof(data_.error);
    }

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline constexpr T const& value() const& noexcept {
        return data_.value;
    }
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline UTL_CONSTEXPR_CXX14 T& value() & noexcept { return data_.value; }
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline constexpr T const&& value() const&& noexcept {
        return __UTL move(data_.value);
    }
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline UTL_CONSTEXPR_CXX14 T&& value() && noexcept {
        return __UTL move(data_.value);
    }
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline constexpr T const& error() const& noexcept {
        return data_.error;
    }
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline UTL_CONSTEXPR_CXX14 T& error() & noexcept { return data_.error; }
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline constexpr T const&& error() const&& noexcept {
        return __UTL move(data_.error);
    }
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline UTL_CONSTEXPR_CXX14 T&& error() && noexcept {
        return __UTL move(data_.error);
    }

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline constexpr bool has_value() const noexcept {
        return has_value_;
    }

    data_type data_;
    bool has_value_;
};

template <typename T, typename E, bool = is_copy_constructible_v<T> && is_copy_constructible_v<E>,
    bool = is_trivially_copy_constructible_v<T> && is_trivially_copy_constructible_v<E>>
class copy_ctor_t : protected storage_base<T, E> {
protected:
    using storage_base::protected_tag;
    using storage_base::storage_base;
    using storage_base::operator=;
};

template <typename T, typename E>
class copy_ctor_t<T, E, true, false> : protected storage_base<T, E> {
    using data_type = data_union<T, E>;

protected:
    using storage_base::protected_tag;
    using storage_base::storage_base;
    using storage_base::operator=;

    __UTL_HIDE_FROM_ABI inline constexpr copy_ctor_t(copy_ctor_t const& other) noexcept(
        is_nothrow_copy_constructible_v<T> && is_nothrow_copy_constructible_v<E>)
        : storage_base{protected_tag, other.has_value(), other.data_ref()} {}
};

template <typename T, typename E, bool = is_move_constructible_v<T> && is_move_constructible_v<E>,
    bool = is_trivially_move_constructible_v<T> && is_trivially_move_constructible_v<E>>
class move_ctor_t : protected copy_ctor_t<T, E> {
protected:
    using copy_ctor_t::copy_ctor_t;
    using copy_ctor_t::protected_tag;
    using copy_ctor_t::operator=;
};

template <typename T, typename E>
class move_ctor_t<T, E, true, false> : protected copy_ctor_t<T, E> {
    using data_type = data_union<T, E>;

protected:
    using copy_ctor_t::copy_ctor_t;
    using copy_ctor_t::protected_tag;
    using copy_ctor_t::operator=;

    __UTL_HIDE_FROM_ABI inline constexpr move_ctor_t(move_ctor_t&& other) noexcept(
        is_nothrow_move_constructible_v<T> && is_nothrow_move_constructible_v<E>)
        : copy_ctor_t{protected_tag, other.has_value(), __UTL move(other.data_ref())} {}
};

template <typename T, typename E, bool = is_copy_assignable_v<T> && is_copy_assignable_v<E>,
    bool = is_trivially_copy_assignable_v<T> && is_trivially_copy_assignable_v<E>>
class copy_assign_t : move_ctor_t<T, E> {
protected:
    using move_ctor_t::move_ctor_t;
    using move_ctor_t::protected_tag;
    using move_ctor_t::operator=;
};

template <typename T, typename E>
class copy_assign_t<T, E, true, false> : protected move_ctor_t<T, E> {
    using data_type = data_union<T, E>;

protected:
    using move_ctor_t::move_ctor_t;
    using move_ctor_t::protected_tag;
    using move_ctor_t::operator=;

    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 copy_assign_t& operator=(
        copy_assign_t const& other) noexcept(is_nothrow_copy_assignable_v<T> &&
        is_nothrow_copy_assignable_v<E>) {
        if (this != __UTL addressof(other)) {
            assign(other);
        }

        return *this;
    }

private:
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline UTL_CONSTEXPR_CXX14 void assign(
        copy_assign_t const& other) noexcept(is_nothrow_copy_assignable_v<T> &&
        is_nothrow_copy_assignable_v<E>) {
        if (this->has_value() != other.has_value()) {
            if (other.has_value()) {
                ::new (this->value_ptr()) T{other.value()};
            } else {
                ::new (this->error_ptr()) E{other.error()};
            }
        } else if (other.has_value()) {
            this->value() = other.value();
        } else {
            this->error() = other.error();
        }
        has_value_ = other.has_value();
    }
};

template <typename T, typename E, bool = is_move_assignable_v<T> && is_move_assignable_v<E>,
    bool = is_trivially_move_assignable_v<T> && is_trivially_move_assignable_v<E>>
class move_assign_t : protected copy_assign_t<T, E> {
protected:
    using copy_assign_t::copy_assign_t;
    using copy_assign_t::protected_tag;
    using copy_assign_t::operator=;
};

template <typename T, typename E>
class move_assign_t<T, E, true, false> : copy_assign_t<T, E> {
    using data_type = data_union<T, E>;

protected:
    using copy_assign_t::copy_assign_t;
    using copy_assign_t::protected_tag;
    using copy_assign_t::operator=;

    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 move_assign_t& operator=(move_assign_t&& other) noexcept(
        is_nothrow_move_assignable_v<T> && is_nothrow_move_assignable_v<E>) {
        assign(__UTL move(other));
        return *this;
    }

private:
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline UTL_CONSTEXPR_CXX14 void assign(move_assign_t&& other) noexcept(
        is_nothrow_move_assignable_v<T> && is_nothrow_move_assignable_v<E>) {
        if (has_value_ != other.has_value_) {
            if (other.has_value_) {
                ::new (this->value_ptr()) T{__UTL move(other.value())};
            } else {
                ::new (this->error_ptr()) E{__UTL move(other.error())};
            }
        } else if (other.has_value_) {
            this->value() = __UTL move(other.value());
        } else {
            this->error() = __UTL move(other.error());
        }
        has_value_ = other.has_value_;
    }
};

template <typename T, typename E>
using storage = move_assign_t<T, E>;

} // namespace expected
} // namespace details

UTL_NAMESPACE_END
