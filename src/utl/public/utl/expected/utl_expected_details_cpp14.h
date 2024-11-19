// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

#if !defined(UTL_EXPECTED_PRIVATE_HEADER_GUARD)
#  error "Private header accessed"
#endif

#include "utl/expected/utl_bad_expected_access.h"
#include "utl/expected/utl_expected_common.h"
#include "utl/expected/utl_unexpected.h"
#include "utl/memory/utl_addressof.h"
#include "utl/memory/utl_destroy_at.h"
#include "utl/type_traits/utl_constants.h"
#include "utl/type_traits/utl_declval.h"
#include "utl/type_traits/utl_enable_if.h"
#include "utl/type_traits/utl_invoke.h"
#include "utl/type_traits/utl_is_copy_constructible.h"
#include "utl/type_traits/utl_is_destructible.h"
#include "utl/type_traits/utl_is_move_constructible.h"
#include "utl/type_traits/utl_is_nothrow_assignable.h"
#include "utl/type_traits/utl_is_nothrow_constructible.h"
#include "utl/type_traits/utl_is_nothrow_copy_assignable.h"
#include "utl/type_traits/utl_is_nothrow_copy_constructible.h"
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

UTL_NAMESPACE_BEGIN

namespace details {
namespace expected {

template <typename T, typename E>
class storage_base;

template <typename T, typename U UTL_CONSTRAINT_CXX11(
    UTL_TRAIT_is_trivially_move_constructible(T) ||
    UTL_TRAIT_is_trivially_copy_constructible(T))>
UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline constexpr T make_union(
    bool has_value, U&& arg) noexcept(UTL_TRAIT_is_nothrow_constructible(T, in_place_t,
                                          decltype(__UTL declval<U>().value)) &&
    UTL_TRAIT_is_nothrow_constructible(T, unexpect_t, decltype(__UTL declval<U>().value))) {
    return has_value ? T{__UTL in_place, __UTL forward_like<U>(arg.value)}
                     : T{__UTL unexpect, __UTL forward_like<U>(arg.error)};
}

template <typename T, typename E, bool = UTL_TRAIT_is_default_constructible(T),
    bool = UTL_TRAIT_is_trivially_destructible(T) && UTL_TRAIT_is_trivially_destructible(E)>
union data_union {
    T value;
    E error;
    empty_t empty;

    __UTL_HIDE_FROM_ABI inline constexpr data_union() noexcept(
        UTL_TRAIT_is_nothrow_default_constructible(T))
        : value{} {}
    __UTL_HIDE_FROM_ABI inline constexpr data_union(empty_t) noexcept : empty{} {}
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
        transforming_t, F&& f, Args&&... args) UTL_THROWS
        : value{__UTL invoke(__UTL forward<F>(f), __UTL forward<Args>(args)...)} {}
    template <typename F, typename... Args>
    __UTL_HIDE_FROM_ABI inline constexpr explicit data_union(
        transforming_error_t, F&& f, Args&&... args) UTL_THROWS
        : error{__UTL invoke(__UTL forward<F>(f), __UTL forward<Args>(args)...)} {}
};

template <typename T, typename E>
union data_union<T, E, false, true> {
    T value;
    E error;
    empty_t empty;

    __UTL_HIDE_FROM_ABI data_union() = delete;
    __UTL_HIDE_FROM_ABI inline constexpr data_union(empty_t) noexcept : empty{} {}
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
        transforming_t, F&& f, Args&&... args) UTL_THROWS
        : value{__UTL invoke(__UTL forward<F>(f), __UTL forward<Args>(args)...)} {}
    template <typename F, typename... Args>
    __UTL_HIDE_FROM_ABI inline constexpr explicit data_union(
        transforming_error_t, F&& f, Args&&... args) UTL_THROWS
        : error{__UTL invoke(__UTL forward<F>(f), __UTL forward<Args>(args)...)} {}
};

template <typename T, typename E>
union data_union<T, E, true, false> {
    T value;
    E error;
    empty_t empty;

    __UTL_HIDE_FROM_ABI inline constexpr data_union() noexcept(
        UTL_TRAIT_is_nothrow_default_constructible(T))
        : value{} {}
    __UTL_HIDE_FROM_ABI inline constexpr data_union(empty_t) noexcept : empty{} {}
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
        transforming_t, F&& f, Args&&... args) UTL_THROWS
        : value{__UTL invoke(__UTL forward<F>(f), __UTL forward<Args>(args)...)} {}
    template <typename F, typename... Args>
    __UTL_HIDE_FROM_ABI inline constexpr explicit data_union(
        transforming_error_t, F&& f, Args&&... args) UTL_THROWS
        : error{__UTL invoke(__UTL forward<F>(f), __UTL forward<Args>(args)...)} {}
    __UTL_HIDE_FROM_ABI inline ~data_union() {}
};

template <typename T, typename E>
union data_union<T, E, false, false> {
    T value;
    E error;
    empty_t empty;

    __UTL_HIDE_FROM_ABI data_union() = delete;
    __UTL_HIDE_FROM_ABI inline constexpr data_union(empty_t) noexcept : empty{} {}
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
        transforming_t, F&& f, Args&&... args) UTL_THROWS
        : value{__UTL invoke(__UTL forward<F>(f), __UTL forward<Args>(args)...)} {}
    template <typename F, typename... Args>
    __UTL_HIDE_FROM_ABI inline constexpr explicit data_union(
        transforming_error_t, F&& f, Args&&... args) UTL_THROWS
        : error{__UTL invoke(__UTL forward<F>(f), __UTL forward<Args>(args)...)} {}
    __UTL_HIDE_FROM_ABI inline ~data_union() {}
};

template <typename T, typename E>
class storage_base {
    using data_type = data_union<T, E>;

public:
    __UTL_HIDE_FROM_ABI inline constexpr storage_base() noexcept(
        UTL_TRAIT_is_nothrow_default_constructible(T)) = default;
    __UTL_HIDE_FROM_ABI inline constexpr storage_base(storage_base const&) noexcept(
        UTL_TRAIT_is_nothrow_copy_constructible(T) &&
        UTL_TRAIT_is_nothrow_copy_constructible(E)) = default;
    __UTL_HIDE_FROM_ABI inline constexpr storage_base(storage_base&&) noexcept(
        UTL_TRAIT_is_nothrow_move_constructible(T) &&
        UTL_TRAIT_is_nothrow_move_constructible(E)) = default;

protected:
    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 storage_base& operator=(storage_base const&) noexcept(
        UTL_TRAIT_is_nothrow_copy_assignable(T) &&
        UTL_TRAIT_is_nothrow_copy_assignable(E)) = default;
    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 storage_base& operator=(storage_base&&) noexcept(
        UTL_TRAIT_is_nothrow_move_assignable(T) &&
        UTL_TRAIT_is_nothrow_move_assignable(E)) = default;

    template <typename... Args>
    __UTL_HIDE_FROM_ABI inline constexpr storage_base(in_place_t, Args&&... args) noexcept(
        UTL_TRAIT_is_nothrow_constructible(T, Args...))
        : data_{__UTL in_place, __UTL forward<Args>(args)...}
        , has_value_{true} {}

    template <typename... Args>
    __UTL_HIDE_FROM_ABI inline constexpr storage_base(unexpect_t, Args&&... args) noexcept(
        UTL_TRAIT_is_nothrow_constructible(E, Args...))
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

    template <typename U UTL_CONSTRAINT_CXX11(
        UTL_TRAIT_is_trivially_move_constructible(data_type) ||
        UTL_TRAIT_is_trivially_copy_constructible(data_type))>
    __UTL_HIDE_FROM_ABI inline constexpr storage_base(converting_t, bool has_value,
        U&& data) noexcept(noexcept(make_union<data_type>(has_value, __UTL declval<U>())))
        : data_{make_union<data_type>(has_value, __UTL forward<U>(data))}
        , has_value_(has_value) {}

    template <typename U UTL_CONSTRAINT_CXX11(
        !UTL_TRAIT_is_trivially_move_constructible(data_type) &&
        !UTL_TRAIT_is_trivially_copy_constructible(data_type))>
    __UTL_HIDE_FROM_ABI inline storage_base(converting_t, bool has_value, U&& data) noexcept(
        UTL_TRAIT_is_nothrow_constructible(T, decltype(__UTL declval<U>().value)) &&
        UTL_TRAIT_is_nothrow_constructible(E, decltype(__UTL declval<U>().error)))
        : data_{__UTL details::expected::empty} {
        if (has_value) {
            ::new (value_ptr()) T{__UTL forward_like<U>(data.value)};
        } else {
            ::new (value_ptr()) T{__UTL forward_like<U>(data.error)};
        }
        has_value_ = has_value;
    }

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

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline constexpr T const& value_ref() const& noexcept {
        return data_.value;
    }
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline UTL_CONSTEXPR_CXX14 T& value_ref() & noexcept {
        return data_.value;
    }
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline constexpr T const&& value_ref() const&& noexcept {
        return __UTL move(data_.value);
    }
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline UTL_CONSTEXPR_CXX14 T&& value_ref() && noexcept {
        return __UTL move(data_.value);
    }
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline constexpr T const& error_ref() const& noexcept {
        return data_.error;
    }
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline UTL_CONSTEXPR_CXX14 T& error_ref() & noexcept {
        return data_.error;
    }
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline constexpr T const&& error_ref() const&& noexcept {
        return __UTL move(data_.error);
    }
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline UTL_CONSTEXPR_CXX14 T&& error_ref() && noexcept {
        return __UTL move(data_.error);
    }

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline constexpr bool has_value() const noexcept {
        return has_value_;
    }

    template <typename... Args>
    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 T& emplace_value(Args&&... args) noexcept {
        static_assert(UTL_TRAIT_is_nothrow_constructible(T, Args...), "Invalid implementation");
        __UTL destroy_at(value_ptr());
        return *__UTL construct_at(value_ptr(), __UTL forward<Args>(args)...);
    }

    template <typename... Args UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_nothrow_constructible(T, Args...))>
    __UTL_HIDE_FROM_ABI inline T* reinitialize_as_value(Args&&... args) noexcept(
        UTL_TRAIT_is_nothrow_constructible(T, Args...)) {
        UTL_ASSERT(!has_value());
        __UTL destroy_at(error_ptr());
        auto ptr = ::new (value_ptr()) T{__UTL forward<Args>(args)...};
        has_value_ = true;
        return ptr;
    }

    template <typename... Args UTL_CONSTRAINT_CXX11(
        !UTL_TRAIT_is_nothrow_constructible(T, Args...))>
    __UTL_HIDE_FROM_ABI inline T* reinitialize_as_value(Args&&... args) noexcept(
        UTL_TRAIT_is_nothrow_constructible(T, Args...)) {
        UTL_ASSERT(!has_value());
        E backup(__UTL move(error_ref()));
        __UTL destroy_at(error_ptr());
        UTL_TRY {
            auto ptr = ::new (value_ptr()) T{__UTL forward<Args>(args)...};
            has_value_ = true;
            return ptr;
        } UTL_CATCH(...) {
            ::new (error_ptr()) E{__UTL move(backup)};
            UTL_RETHROW();
        }
    }

    template <typename... Args UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_nothrow_constructible(E, Args...))>
    __UTL_HIDE_FROM_ABI inline E* reinitialize_as_error(Args&&... args) noexcept(
        UTL_TRAIT_is_nothrow_constructible(E, Args...)) {
        UTL_ASSERT(has_value());
        __UTL destroy_at(value_ptr());
        auto ptr = ::new (error_ptr()) E{__UTL forward<Args>(args)...};
        has_value_ = false;
        return ptr;
    }

    template <typename... Args UTL_CONSTRAINT_CXX11(
        !UTL_TRAIT_is_nothrow_constructible(E, Args...))>
    __UTL_HIDE_FROM_ABI inline E* reinitialize_as_error(Args&&... args) noexcept(
        UTL_TRAIT_is_nothrow_constructible(E, Args...)) {
        UTL_ASSERT(has_value());
        T backup(__UTL move(value_ref()));
        __UTL destroy_at(value_ptr());
        UTL_TRY {
            auto ptr = ::new (error_ptr()) E{__UTL forward<Args>(args)...};
            has_value_ = false;
            return ptr;
        } UTL_CATCH(...) {
            ::new (value_ptr()) T{__UTL move(backup)};
            UTL_RETHROW();
        }
    }

private:
    data_type data_;
    bool has_value_;
};

template <typename T, typename E,
    bool = UTL_TRAIT_is_copy_constructible(T) && UTL_TRAIT_is_copy_constructible(E),
    bool = UTL_TRAIT_is_trivially_copy_constructible(T) &&
        UTL_TRAIT_is_trivially_copy_constructible(E)>
class copy_ctor_t : protected storage_base<T, E> {
public:
    using storage_base<T, E>::storage_base;

protected:
    using storage_base<T, E>::operator=;
};

template <typename T, typename E>
class copy_ctor_t<T, E, true, false> : protected storage_base<T, E> {
public:
    using storage_base<T, E>::storage_base;
    __UTL_HIDE_FROM_ABI inline constexpr copy_ctor_t(copy_ctor_t const& other) noexcept(
        UTL_TRAIT_is_nothrow_copy_constructible(T) && UTL_TRAIT_is_nothrow_copy_constructible(E))
        : storage_base{converting, other.has_value(), other.data_ref()} {}

protected:
    using storage_base<T, E>::operator=;
};

template <typename T, typename E,
    bool = UTL_TRAIT_is_move_constructible(T) && UTL_TRAIT_is_move_constructible(E),
    bool = UTL_TRAIT_is_trivially_move_constructible(T) &&
        UTL_TRAIT_is_trivially_move_constructible(E)>
class move_ctor_t : protected copy_ctor_t<T, E> {
public:
    using copy_ctor_t<T, E>::copy_ctor_t;

protected:
    using copy_ctor_t<T, E>::operator=;
};

template <typename T, typename E>
class move_ctor_t<T, E, true, false> : protected copy_ctor_t<T, E> {
public:
    using copy_ctor_t<T, E>::copy_ctor_t;

    __UTL_HIDE_FROM_ABI inline constexpr move_ctor_t(move_ctor_t&& other) noexcept(
        UTL_TRAIT_is_nothrow_move_constructible(T) && UTL_TRAIT_is_nothrow_move_constructible(E))
        : copy_ctor_t{converting, other.has_value(), __UTL move(other.data_ref())} {}

protected:
    using copy_ctor_t<T, E>::operator=;
};

template <typename T, typename E,
    bool = UTL_TRAIT_is_copy_assignable(T) && UTL_TRAIT_is_copy_assignable(E),
    bool = UTL_TRAIT_is_trivially_copy_assignable(T) && UTL_TRAIT_is_trivially_copy_assignable(E)>
class copy_assign_t : protected move_ctor_t<T, E> {
public:
    using move_ctor_t<T, E>::move_ctor_t;

protected:
    using move_ctor_t<T, E>::operator=;
};

template <typename T, typename E>
class copy_assign_t<T, E, true, false> : protected move_ctor_t<T, E> {
public:
    using move_ctor_t<T, E>::move_ctor_t;

protected:
    using move_ctor_t<T, E>::operator=;
    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 copy_assign_t& operator=(
        copy_assign_t const& other) noexcept(UTL_TRAIT_is_nothrow_copy_assignable(T) &&
        UTL_TRAIT_is_nothrow_copy_assignable(E)) {
        if (this != __UTL addressof(other)) {
            assign(other);
        }

        return *this;
    }

private:
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline UTL_CONSTEXPR_CXX14 void assign(
        copy_assign_t const& other) noexcept(UTL_TRAIT_is_nothrow_copy_assignable(T) &&
        UTL_TRAIT_is_nothrow_copy_assignable(E)) {
        if (this->has_value() != other.has_value()) {
            if (other.has_value()) {
                __UTL destroy_at(this->error_ptr());
                ::new (this->value_ptr()) T{other.value_ref()};
            } else {
                __UTL destroy_at(this->value_ptr());
                ::new (this->error_ptr()) E{other.error_ref()};
            }
        } else if (other.has_value()) {
            this->value_ref() = other.value_ref();
        } else {
            this->error_ref() = other.error_ref();
        }
        has_value_ = other.has_value();
    }
};

template <typename T, typename E,
    bool = UTL_TRAIT_is_move_assignable(T) && UTL_TRAIT_is_move_assignable(E),
    bool = UTL_TRAIT_is_trivially_move_assignable(T) && UTL_TRAIT_is_trivially_move_assignable(E)>
class move_assign_t : protected copy_assign_t<T, E> {
    using copy_assign_t<T, E>::copy_assign_t;

protected:
    using copy_assign_t<T, E>::operator=;
};

template <typename T, typename E>
class move_assign_t<T, E, true, false> : protected copy_assign_t<T, E> {
public:
    using copy_assign_t<T, E>::copy_assign_t;

protected:
    using copy_assign_t<T, E>::operator=;

    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 move_assign_t& operator=(move_assign_t&& other) noexcept(
        UTL_TRAIT_is_nothrow_move_assignable(T) && UTL_TRAIT_is_nothrow_move_assignable(E)) {
        assign(__UTL move(other));
        return *this;
    }

private:
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline UTL_CONSTEXPR_CXX14 void assign(move_assign_t&& other) noexcept(
        UTL_TRAIT_is_nothrow_move_assignable(T) && UTL_TRAIT_is_nothrow_move_assignable(E)) {
        if (this->has_value() != other.has_value()) {
            if (other.has_value()) {
                __UTL destroy_at(this->error_ptr());
                ::new (this->value_ptr()) T{__UTL move(other.value_ref())};
            } else {
                __UTL destroy_at(this->value_ptr());
                ::new (this->error_ptr()) E{__UTL move(other.error_ref())};
            }
        } else if (other.has_value()) {
            this->value_ref() = __UTL move(other.value_ref());
        } else {
            this->error_ref() = __UTL move(other.error_ref());
        }
        has_value_ = other.has_value();
    }
};

template <typename D, typename T, typename E,
    bool = UTL_TRAIT_conjunction(is_swappable<T>, is_swappable<E>, is_move_constructible<T>,
        is_move_constructible<E>,
        disjunction<is_nothrow_move_constructible<T>, is_nothrow_move_constructible<E>>),
    bool = UTL_TRAIT_is_nothrow_move_constructible(E)>
class swap_base : protected move_assign_t<T, E> {
    static_assert(__UTL_TRAIT_is_expected(D));

public:
    using move_assign_t<T, E>::move_assign_t;

    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 void swap(D& other) noexcept(
        UTL_TRAIT_is_nothrow_swappable(T) && UTL_TRAIT_is_nothrow_swappable(E) &&
        UTL_TRAIT_is_nothrow_move_constructible(T)) {
        static_assert(UTL_TRAIT_is_base_of(swap_base, D), "Invalid definition");
        if (this->has_value() == other.has_value()) {
            if (this->has_value()) {
                __UTL ranges::swap(this->value_ref(), other.value_ref());
            } else {
                __UTL ranges::swap(this->error_ref(), other.error_ref());
            }
        } else if (this->has_value()) {
            this->cross_swap(other);
        } else {
            other.cross_swap(*this);
        }
    }

    __UTL_HIDE_FROM_ABI friend inline UTL_CONSTEXPR_CXX14 void swap(D& left, D& right) noexcept(
        UTL_TRAIT_is_nothrow_swappable(T) && UTL_TRAIT_is_nothrow_swappable(E) &&
        UTL_TRAIT_is_nothrow_move_constructible(T)) {
        left.swap(right);
    }

protected:
    using move_assign_t<T, E>::operator=;
    __UTL_HIDE_FROM_ABI inline ~swap_base() noexcept = default;

private:
    __UTL_HIDE_FROM_ABI inline void cross_swap(storage_base& other) noexcept(
        UTL_TRAIT_is_nothrow_move_constructible(T)) {
        UTL_ASSERT(has_value() && !other.has_value());
        E tmp(__UTL move(other.error_ref()));
        __UTL destroy_at(other.error_ptr());
        UTL_TRY {
            ::new (other.value_ptr()) T{__UTL move(this->value_ref())};
            __UTL destroy_at(this->value_ptr());
            ::new (this->error_ptr()) E{__UTL move(tmp)};
        } UTL_CATCH(...) {
            ::new (other.error_ptr()) E{__UTL move(tmp)};
            UTL_RETHROW();
        }
        this->has_value_ = false;
        other.has_value_ = true;
    }
};

template <typename D, typename T, typename E>
class swap_base<D, T, E, true, false> : protected move_assign_t<T, E> {
    static_assert(__UTL_TRAIT_is_expected(D));

public:
    using move_assign_t<T, E>::move_assign_t;
    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 void swap(D& other) {
        static_assert(UTL_TRAIT_is_base_of(swap_base, D), "Invalid definition");
        if (this->has_value() == other.has_value()) {
            if (this->has_value()) {
                __UTL ranges::swap(this->value_ref(), other.value_ref());
            } else {
                __UTL ranges::swap(this->error_ref(), other.error_ref());
            }
        } else if (this->has_value()) {
            this->cross_swap(other);
        } else {
            other.cross_swap(*this);
        }
    }

    __UTL_HIDE_FROM_ABI friend inline UTL_CONSTEXPR_CXX14 void swap(D& left, D& right) { left.swap(right); }

protected:
    using move_assign_t<T, E>::operator=;
    __UTL_HIDE_FROM_ABI inline ~swap_base() noexcept = default;

private:
    __UTL_HIDE_FROM_ABI inline void cross_swap(storage_base& other) {
        UTL_ASSERT(has_value() && !other.has_value());
        T tmp(__UTL move(this->value_ref()));
        __UTL destroy_at(this->value_ptr());
        UTL_TRY {
            ::new (this->error_ptr()) E{__UTL move(other.error_ref())};
            __UTL destroy_at(other.error_ptr());
            ::new (other.value_ptr()) T{__UTL move(tmp)};
        } UTL_CATCH(...) {
            ::new (this->value_ptr()) T{__UTL move(tmp)};
            UTL_RETHROW();
        }
        this->has_value_ = false;
        other.has_value_ = true;
    }
};

template <typename D, typename T, typename E, bool O>
class swap_base<D, T, E, false, O> : protected move_assign_t<T, E> {
    static_assert(__UTL_TRAIT_is_expected(D));

public:
    using move_assign_t<T, E>::move_assign_t;
    __UTL_HIDE_FROM_ABI void swap(D&) = delete;

protected:
    using move_assign_t<T, E>::operator=;
    inline ~swap_base() noexcept = default;
};

template <typename D, typename T, typename E>
class storage : protected swap_base<D, T, E> {
    static_assert(!UTL_TRAIT_is_same(remove_cvref_t<T>, empty_t), "Invalid value type");
    static_assert(!UTL_TRAIT_is_void(T), "Invalid value type");

public:
    using swap_base<D, T, E>::swap_base;

protected:
    using swap_base<D, T, E>::swap;
    using swap_base<D, empty_t, E>::operator=;
    inline ~storage() noexcept = default;
};

template <typename D, typename E>
class void_storage : protected swap_base<D, empty_t, E> {
public:
    using swap_base<D, empty_t, E>::swap_base;
    using swap_base<D, empty_t, E>::swap;

protected:
    using swap_base<D, empty_t, E>::operator=;
    inline ~void_storage() noexcept = default;
};

} // namespace expected
} // namespace details

UTL_NAMESPACE_END
