// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

#if !defined(UTL_EXPECTED_PRIVATE_HEADER_GUARD)
#  error "Private header accessed"
#endif
#if !UTL_CXX17
#  error "Invalid header accessed"
#endif

#include "utl/expected/utl_bad_expected_access.h"
#include "utl/expected/utl_expected_common.h"
#include "utl/expected/utl_unexpected.h"
#include "utl/functional/utl_invoke.h"
#include "utl/memory/utl_addressof.h"
#include "utl/memory/utl_construct_at.h"
#include "utl/memory/utl_destroy_at.h"
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

template <typename T, typename U>
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
class underlying_storage {
    using data_type = data_union<T, E>;

public:
    __UTL_HIDE_FROM_ABI inline constexpr underlying_storage() noexcept(
        UTL_TRAIT_is_nothrow_default_constructible(T)) = default;
    __UTL_HIDE_FROM_ABI inline constexpr underlying_storage(underlying_storage const&) noexcept(
        UTL_TRAIT_is_nothrow_copy_constructible(T) &&
        UTL_TRAIT_is_nothrow_copy_constructible(E)) = default;
    __UTL_HIDE_FROM_ABI inline constexpr underlying_storage(underlying_storage&&) noexcept(
        UTL_TRAIT_is_nothrow_move_constructible(T) &&
        UTL_TRAIT_is_nothrow_move_constructible(E)) = default;

    __UTL_HIDE_FROM_ABI inline ~underlying_storage() = default;

protected:
    __UTL_HIDE_FROM_ABI inline constexpr underlying_storage& operator=(
        underlying_storage const&) noexcept(UTL_TRAIT_is_nothrow_copy_assignable(T) &&
        UTL_TRAIT_is_nothrow_copy_assignable(E)) = default;
    __UTL_HIDE_FROM_ABI inline constexpr underlying_storage& operator=(
        underlying_storage&&) noexcept(UTL_TRAIT_is_nothrow_move_assignable(T) &&
        UTL_TRAIT_is_nothrow_move_assignable(E)) = default;

    template <typename... Args>
    __UTL_HIDE_FROM_ABI inline constexpr underlying_storage(in_place_t, Args&&... args) noexcept(
        UTL_TRAIT_is_nothrow_constructible(T, Args...))
        : data_{__UTL in_place, __UTL forward<Args>(args)...}
        , has_value_{true} {}

    template <typename... Args>
    __UTL_HIDE_FROM_ABI inline constexpr underlying_storage(unexpect_t, Args&&... args) noexcept(
        UTL_TRAIT_is_nothrow_constructible(E, Args...))
        : data_{__UTL unexpect, __UTL forward<Args>(args)...}
        , has_value_{false} {}

    template <typename F, typename... Args>
    __UTL_HIDE_FROM_ABI inline constexpr explicit underlying_storage(
        transforming_t, F&& f, Args&&... args) noexcept(UTL_TRAIT_is_nothrow_invocable(F, Args...))
        : data_{__UTL details::expected::transforming, __UTL forward<F>(f),
              __UTL forward<Args>(args)...}
        , has_value_{true} {}
    template <typename F, typename... Args>
    __UTL_HIDE_FROM_ABI inline constexpr explicit underlying_storage(transforming_error_t, F&& f,
        Args&&... args) noexcept(UTL_TRAIT_is_nothrow_invocable(F, Args...))
        : data_{__UTL details::expected::transforming_error, __UTL forward<F>(f),
              __UTL forward<Args>(args)...}
        , has_value_{false} {}

    template <typename U>
    __UTL_HIDE_FROM_ABI inline constexpr underlying_storage(converting_t, bool has_value,
        U&& data) noexcept(noexcept(make_union<data_type>(has_value, __UTL forward<U>(data))))
        : data_{make_union<data_type>(has_value, __UTL forward<U>(data))}
        , has_value_{has_value} {}

    UTL_ATTRIBUTE(GETTER) inline constexpr data_type const& data_ref() const& noexcept { return data_; }

    UTL_ATTRIBUTE(GETTER) inline constexpr data_type& data_ref() & noexcept { return data_; }
    UTL_ATTRIBUTE(GETTER) inline constexpr T const* value_ptr() const noexcept {
        return __UTL addressof(data_.value);
    }
    UTL_ATTRIBUTE(GETTER) inline constexpr T* value_ptr() noexcept { return __UTL addressof(data_.value); }
    UTL_ATTRIBUTE(GETTER) inline constexpr E const* error_ptr() const noexcept {
        return __UTL addressof(data_.error);
    }
    UTL_ATTRIBUTE(GETTER) inline constexpr E* error_ptr() noexcept { return __UTL addressof(data_.error); }

    UTL_ATTRIBUTE(GETTER) inline constexpr T const& value_ref() const& noexcept { return data_.value; }
    UTL_ATTRIBUTE(GETTER) inline constexpr T& value_ref() & noexcept { return data_.value; }
    UTL_ATTRIBUTE(GETTER) inline constexpr T const&& value_ref() const&& noexcept {
        return __UTL move(data_.value);
    }
    UTL_ATTRIBUTE(GETTER) inline constexpr T&& value_ref() && noexcept { return __UTL move(data_.value); }
    UTL_ATTRIBUTE(GETTER) inline constexpr E const& error_ref() const& noexcept { return data_.error; }
    UTL_ATTRIBUTE(GETTER) inline constexpr E& error_ref() & noexcept { return data_.error; }
    UTL_ATTRIBUTE(GETTER) inline constexpr E const&& error_ref() const&& noexcept {
        return __UTL move(data_.error);
    }
    UTL_ATTRIBUTE(GETTER) inline constexpr E&& error_ref() && noexcept { return __UTL move(data_.error); }

    UTL_ATTRIBUTE(GETTER) inline constexpr bool has_value() const noexcept { return has_value_; }

    template <typename... Args>
    __UTL_HIDE_FROM_ABI inline constexpr T& emplace_value(Args&&... args) noexcept {
        static_assert(UTL_TRAIT_is_nothrow_constructible(T, Args...), "Invalid implementation");
        __UTL destroy_at(value_ptr());
        return *__UTL construct_at(value_ptr(), __UTL forward<Args>(args)...);
    }

    template <typename... Args>
    __UTL_HIDE_FROM_ABI inline T* reinitialize_as_value(Args&&... args) noexcept(
        UTL_TRAIT_is_nothrow_constructible(T, Args...)) {
        UTL_ASSERT(!has_value());
        if constexpr (UTL_TRAIT_is_nothrow_constructible(T, Args...)) {
            __UTL destroy_at(error_ptr());
            auto ptr = ::new (value_ptr()) T{__UTL forward<Args>(args)...};
            has_value_ = true;
            return ptr;
        } else {
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
    }

    template <typename... Args>
    __UTL_HIDE_FROM_ABI inline E* reinitialize_as_error(Args&&... args) noexcept(
        UTL_TRAIT_is_nothrow_constructible(E, Args...)) {
        UTL_ASSERT(has_value());
        if constexpr (UTL_TRAIT_is_nothrow_constructible(E, Args...)) {
            __UTL destroy_at(value_ptr());
            auto ptr = ::new (error_ptr()) E{__UTL forward<Args>(args)...};
            has_value_ = false;
            return ptr;
        } else {
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
    }

private:
    data_type data_;
    bool has_value_;
};

} // namespace expected
} // namespace details

UTL_NAMESPACE_END

#undef __UTL_ATTRIBUTE_GETTER
#undef __UTL_ATTRIBUTE_TYPE_AGGREGATE_GETTER
