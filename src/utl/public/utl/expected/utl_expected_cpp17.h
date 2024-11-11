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
#include "utl/type_traits/utl_constants.h"
#include "utl/type_traits/utl_enable_if.h"
#include "utl/type_traits/utl_invoke.h"
#include "utl/type_traits/utl_is_complete.h"
#include "utl/type_traits/utl_is_constructible.h"
#include "utl/type_traits/utl_is_convertible.h"
#include "utl/type_traits/utl_is_copy_constructible.h"
#include "utl/type_traits/utl_is_destructible.h"
#include "utl/type_traits/utl_is_move_constructible.h"
#include "utl/type_traits/utl_is_nothrow_convertible.h"
#include "utl/type_traits/utl_is_nothrow_copy_assignable.h"
#include "utl/type_traits/utl_is_nothrow_copy_constructible.h"
#include "utl/type_traits/utl_is_nothrow_move_assignable.h"
#include "utl/type_traits/utl_is_nothrow_move_constructible.h"
#include "utl/type_traits/utl_is_same.h"
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

#define __UTL_ATTRIBUTE_EXPECTED_INLINE_PURE \
    (ALWAYS_INLINE)(PURE)(NODISCARD)__UTL_ATTRIBUTE__HIDE_FROM_ABI
#define __UTL_ATTRIBUTE_TYPE_AGGREGATE_EXPECTED_INLINE_PURE
#define __UTL_ATTRIBUTE_EXPECTED_INLINE_CONST \
    (ALWAYS_INLINE)(CONST)(NODISCARD)__UTL_ATTRIBUTE__HIDE_FROM_ABI
#define __UTL_ATTRIBUTE_TYPE_AGGREGATE_EXPECTED_INLINE_CONST

UTL_NAMESPACE_BEGIN

namespace details {
namespace expected {

template <typename F, typename... Args,
    enable_if_t<UTL_TRAIT_is_void(invoke_result_t<F, Args...>), int> = 0>
UTL_ATTRIBUTE(ALWAYS_INLINE) inline constexpr __UTL expected<invoke_result_t<F, Args...>, E> transformation(
    F&& f, Args&&... args) noexcept(UTL_TRAIT_is_nothrow_invocable(F, Args...)) {
    __UTL invoke(__UTL forward<F>(f), __UTL forward<Args>(args)...);
    return __UTL expected<invoke_result_t<F, Args...>, E>{};
}

template <typename F, typename... Args,
    enable_if_t<!UTL_TRAIT_is_void(invoke_result_t<F, Args...>), int> = 1>
UTL_ATTRIBUTE(ALWAYS_INLINE) inline constexpr __UTL expected<invoke_result_t<F, Args...>, E> transformation(
    F&& f, Args&&... args) noexcept(UTL_TRAIT_is_nothrow_invocable(F, Args...)) {
    return __UTL expected<invoke_result_t<F, Args...>, E>{
        __UTL in_place, __UTL invoke(__UTL forward<F>(f), __UTL forward<Args>(args)...)};
}

template <typename T, typename E,
    bool = UTL_TRAIT_is_trivially_destructible(T) && UTL_TRAIT_is_trivially_destructible(E)>
class storage<T, E> {
protected:
    union data_union {
        T value;
        E error;

        data_union(conditional_t<UTL_TRAIT_is_trivially_copy_constructible(T) &&
                UTL_TRAIT_is_trivially_copy_constructible(E),
            inaccessible_t, data_union const&>) = delete;
        data_union(conditional_t<UTL_TRAIT_is_trivially_move_constructible(T) &&
                UTL_TRAIT_is_trivially_move_constructible(E),
            inaccessible_t, data_union&&>) = delete;
        data_union& operator=(conditional_t<UTL_TRAIT_is_trivially_copy_assignable(T) &&
                UTL_TRAIT_is_trivially_copy_assignable(E),
            inaccessible_t, data_union const&>) = delete;
        data_union& operator=(conditional_t<UTL_TRAIT_is_trivially_move_assignable(T) &&
                UTL_TRAIT_is_trivially_move_assignable(E),
            inaccessible_t, data_union&&>) = delete;

        template <typename... Args>
        __UTL_HIDE_FROM_ABI inline constexpr explicit data_union(in_place_t, Args&&... args)
            : value_{__UTL forward<Args>(args)...} {}
        template <typename... Args>
        __UTL_HIDE_FROM_ABI inline constexpr explicit data_union(unexpect_t, Args&&... args)
            : error_{__UTL forward<Args>(args)...} {}
    };

protected:
    __UTL_HIDE_FROM_ABI inline ~storage() noexcept = default;
    __UTL_HIDE_FROM_ABI inline constexpr storage(copy_ctor_delete_argument_t) = delete;
    __UTL_HIDE_FROM_ABI inline constexpr storage(move_ctor_delete_argument_t) = delete;
    __UTL_HIDE_FROM_ABI inline constexpr storage& operator=(copy_assign_delete_argument_t) = delete;
    __UTL_HIDE_FROM_ABI inline constexpr storage& operator=(move_assign_delete_argument_t) = delete;

    template <typename U UTL_CONSTRAINT_CXX11(
        UTL_TRAIT_is_same(U, storage) && !UTL_TRAIT_is_trivially_copy_constructible(T) ||
        !UTL_TRAIT_is_trivially_copy_constructible(E))>
    __UTL_HIDE_FROM_ABI inline constexpr storage(bool has_value, U const& other) noexcept(
        UTL_TRAIT_is_nothrow_copy_constructible(T) && UTL_TRAIT_is_nothrow_copy_constructible(E))
        : data_{make_union(has_value, other.data_)}
        , has_value_{has_value} {}

    template <typename U UTL_CONSTRAINT_CXX11(
        UTL_TRAIT_is_same(U, storage) && !UTL_TRAIT_is_trivially_move_constructible(T) ||
        !UTL_TRAIT_is_trivially_move_constructible(E))>
    __UTL_HIDE_FROM_ABI inline constexpr storage(bool has_value, U&& other) noexcept(
        UTL_TRAIT_is_nothrow_move_constructible(T) && UTL_TRAIT_is_nothrow_move_constructible(E))
        : data_{make_union(has_value, __UTL move(other.data_))}
        , has_value_{has_value} {}

    template <typename U UTL_CONSTRAINT_CXX11(
        UTL_TRAIT_is_same(U, storage) && !UTL_TRAIT_is_trivially_copy_assignable(T) ||
        !UTL_TRAIT_is_trivially_copy_assignable(E))>
    __UTL_HIDE_FROM_ABI inline constexpr storage& operator=(U const& other) noexcept(
        UTL_TRAIT_is_nothrow_copy_assignable(T) && UTL_TRAIT_is_nothrow_copy_assignable(E)) {
        if (this != &other) {
            if (has_value_ != other.has_value_) {
                if (other.has_value_) {
                    ::new (__UTL addressof(data_.value)) T{other.data_.value};
                } else {
                    ::new (__UTL addressof(data_.error)) E{other.data_.error};
                }
            } else if (other.has_value_) {
                data_.value = other.data_.value;
            } else {
                data_.error = other.data_.error;
            }
            has_value_ = other.has_value_;
        }

        return *this;
    }

    template <typename U UTL_CONSTRAINT_CXX11(
        UTL_TRAIT_is_same(U, storage) && !UTL_TRAIT_is_trivially_move_assignable(T) ||
        !UTL_TRAIT_is_trivially_move_assignable(E))>
    __UTL_HIDE_FROM_ABI inline constexpr storage& operator=(U const& other) noexcept(
        UTL_TRAIT_is_nothrow_move_constructible(T) && UTL_TRAIT_is_nothrow_move_constructible(E) &&
        UTL_TRAIT_is_nothrow_move_assignable(T) && UTL_TRAIT_is_nothrow_move_assignable(E)) {
        if (this != &other) {
            if (has_value_ != other.has_value_) {
                if (other.has_value_) {
                    ::new (__UTL addressof(data_.value)) T{__UTL move(other.data_.value)};
                } else {
                    ::new (__UTL addressof(data_.error)) E{__UTL move(other.data_.error)};
                }
            } else if (other.has_value_) {
                data_.value = __UTL move(other.data_.value);
            } else {
                data_.error = __UTL move(other.data_.error);
            }
            has_value_ = other.has_value_;
        }

        return *this;
    }

    __UTL_HIDE_FROM_ABI inline constexpr storage(storage const&) noexcept(
        UTL_TRAIT_is_nothrow_copy_constructible(T) &&
        UTL_TRAIT_is_nothrow_copy_constructible(E)) = default;
    __UTL_HIDE_FROM_ABI inline constexpr storage(storage&&) noexcept(
        UTL_TRAIT_is_nothrow_move_constructible(T) &&
        UTL_TRAIT_is_nothrow_move_constructible(E)) = default;
    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 storage& operator=(storage const&) noexcept(
        UTL_TRAIT_is_nothrow_copy_assignable(T) &&
        UTL_TRAIT_is_nothrow_copy_assignable(E)) = default;
    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 storage& operator=(storage&&) noexcept(
        UTL_TRAIT_is_nothrow_move_assignable(T) &&
        UTL_TRAIT_is_nothrow_move_assignable(E)) = default;

    template <typename... Args>
    __UTL_HIDE_FROM_ABI inline constexpr explicit storage(in_place_t, Args&&... args)
        : data_{__UTL in_place, __UTL forward<Args>(args)...} {}
    template <typename... Args>
    __UTL_HIDE_FROM_ABI inline constexpr explicit storage(unexpect_t, Args&&... args)
        : data_{__UTL unexpect, __UTL forward<Args>(args)...} {}

    data_union data_;
    bool has_value_;

private:
    template <typename U>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline constexpr data_union make_union(
        bool has_value, U&& arg) noexcept(UTL_TRAIT_is_nothrow_copy_constructible(T) &&
        UTL_TRAIT_is_nothrow_copy_constructible(E)) {
        return has_value ? data_union{__UTL in_place, __UTL forward_like<U>(arg.value)}
                         : data_union{__UTL unexpect, __UTL forward_like<U>(arg.error)};
    }
};

template <typename T, typename E>
class storage<T, E, false> {
protected:
    union data_union {
        T value;
        E error;

        data_union(conditional_t<UTL_TRAIT_is_trivially_copy_constructible(T) &&
                UTL_TRAIT_is_trivially_copy_constructible(E),
            inaccessible_t, data_union const&>) = delete;
        data_union(conditional_t<UTL_TRAIT_is_trivially_move_constructible(T) &&
                UTL_TRAIT_is_trivially_move_constructible(E),
            inaccessible_t, data_union&&>) = delete;
        data_union& operator=(conditional_t<UTL_TRAIT_is_trivially_copy_assignable(T) &&
                UTL_TRAIT_is_trivially_copy_assignable(E),
            inaccessible_t, data_union const&>) = delete;
        data_union& operator=(conditional_t<UTL_TRAIT_is_trivially_move_assignable(T) &&
                UTL_TRAIT_is_trivially_move_assignable(E),
            inaccessible_t, data_union&&>) = delete;

        template <typename... Args>
        __UTL_HIDE_FROM_ABI inline constexpr explicit data_union(in_place_t, Args&&... args)
            : value_{__UTL forward<Args>(args)...} {}
        template <typename... Args>
        __UTL_HIDE_FROM_ABI inline constexpr explicit data_union(unexpect_t, Args&&... args)
            : error_{__UTL forward<Args>(args)...} {}
    };

    __UTL_HIDE_FROM_ABI inline void destroy_member() noexcept {
        if (has_value_) {
            value_.~T();
        } else {
            error_.~E();
        }
    }

    __UTL_HIDE_FROM_ABI inline ~storage() noexcept { destroy_member(); }

    template <typename U UTL_CONSTRAINT_CXX11(
        UTL_TRAIT_is_same(U, storage) && !UTL_TRAIT_is_trivially_copy_constructible(T) ||
        !UTL_TRAIT_is_trivially_copy_constructible(E))>
    __UTL_HIDE_FROM_ABI inline constexpr storage(bool has_value, U const& other) noexcept(
        UTL_TRAIT_is_nothrow_copy_constructible(T) && UTL_TRAIT_is_nothrow_copy_constructible(E))
        : data_{make_union(has_value, other.data_)}
        , has_value_{has_value} {}

    template <typename U UTL_CONSTRAINT_CXX11(
        UTL_TRAIT_is_same(U, storage) && !UTL_TRAIT_is_trivially_move_constructible(T) ||
        !UTL_TRAIT_is_trivially_move_constructible(E))>
    __UTL_HIDE_FROM_ABI inline constexpr storage(bool has_value, U&& other) noexcept(
        UTL_TRAIT_is_nothrow_move_constructible(T) && UTL_TRAIT_is_nothrow_move_constructible(E))
        : data_{make_union(has_value, __UTL move(other.data_))}
        , has_value_{has_value} {}

    template <typename U UTL_CONSTRAINT_CXX11(
        UTL_TRAIT_is_same(U, storage) && !UTL_TRAIT_is_trivially_copy_assignable(T) ||
        !UTL_TRAIT_is_trivially_copy_assignable(E))>
    __UTL_HIDE_FROM_ABI inline constexpr storage& operator=(U const& other) noexcept(
        UTL_TRAIT_is_nothrow_copy_assignable(T) && UTL_TRAIT_is_nothrow_copy_assignable(E)) {
        if (this != &other) {
            if (has_value_ != other.has_value_) {
                destroy_member();
                if (other.has_value_) {
                    ::new (__UTL addressof(data_.value)) T{other.data_.value};
                } else {
                    ::new (__UTL addressof(data_.error)) E{other.data_.error};
                }
            } else if (other.has_value_) {
                data_.value = other.data_.value;
            } else {
                data_.error = other.data_.error;
            }
            has_value_ = other.has_value_;
        }

        return *this;
    }

    template <typename U UTL_CONSTRAINT_CXX11(
        UTL_TRAIT_is_same(U, storage) && !UTL_TRAIT_is_trivially_move_assignable(T) ||
        !UTL_TRAIT_is_trivially_move_assignable(E))>
    __UTL_HIDE_FROM_ABI inline constexpr storage& operator=(U const& other) noexcept(
        UTL_TRAIT_is_nothrow_move_constructible(T) && UTL_TRAIT_is_nothrow_move_constructible(E) &&
        UTL_TRAIT_is_nothrow_move_assignable(T) && UTL_TRAIT_is_nothrow_move_assignable(E)) {
        if (this != &other) {
            if (has_value_ != other.has_value_) {
                destroy_member();
                if (other.has_value_) {
                    ::new (__UTL addressof(data_.value)) T{__UTL move(other.data_.value)};
                } else {
                    ::new (__UTL addressof(data_.error)) E{__UTL move(other.data_.error)};
                }
            } else if (other.has_value_) {
                data_.value = __UTL move(other.data_.value);
            } else {
                data_.error = __UTL move(other.data_.error);
            }
            has_value_ = other.has_value_;
        }

        return *this;
    }

    __UTL_HIDE_FROM_ABI inline constexpr storage(storage const&) noexcept(
        UTL_TRAIT_is_nothrow_copy_constructible(T) &&
        UTL_TRAIT_is_nothrow_copy_constructible(E)) = default;
    __UTL_HIDE_FROM_ABI inline constexpr storage(storage&&) noexcept(
        UTL_TRAIT_is_nothrow_move_constructible(T) &&
        UTL_TRAIT_is_nothrow_move_constructible(E)) = default;
    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 storage& operator=(storage const&) noexcept(
        UTL_TRAIT_is_nothrow_copy_assignable(T) &&
        UTL_TRAIT_is_nothrow_copy_assignable(E)) = default;
    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 storage& operator=(storage&&) noexcept(
        UTL_TRAIT_is_nothrow_move_assignable(T) &&
        UTL_TRAIT_is_nothrow_move_assignable(E)) = default;

    template <typename... Args>
    __UTL_HIDE_FROM_ABI inline constexpr explicit storage(in_place_t, Args&&... args)
        : data_{__UTL in_place, __UTL forward<Args>(args)...} {}
    template <typename... Args>
    __UTL_HIDE_FROM_ABI inline constexpr explicit storage(unexpect_t, Args&&... args)
        : data_{__UTL unexpect, __UTL forward<Args>(args)...} {}

    data_union data_;
    bool has_value_;

private:
    template <typename U>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline constexpr data_union make_union(
        bool has_value, U&& arg) noexcept(UTL_TRAIT_is_nothrow_copy_constructible(T) &&
        UTL_TRAIT_is_nothrow_copy_constructible(E)) {
        return has_value ? data_union{__UTL in_place, __UTL forward_like<U>(arg.value)}
                         : data_union{__UTL unexpect, __UTL forward_like<U>(arg.error)};
    }
};

} // namespace expected
} // namespace details

template <typename T, typename E>
class __UTL_PUBLIC_TEMPLATE expected {
    static_assert(!UTL_TRAIT_is_same(remove_cvref_t<T>, unexpect_t), "Invalid type");
    static_assert(!UTL_TRAIT_is_same(remove_cvref_t<T>, in_place_t), "Invalid type");
    static_assert(!__UTL_TRAIT_is_unexpected(remove_cvref_t<T>), "Invalid type");
    static_assert(UTL_TRAIT_is_destructible(T), "Invalid type");
    static_assert(UTL_TRAIT_is_complete(unexpected<E>), "Invalid type");

public:
    using value_type = T;
    using error_type = E;
    using unexpected_type = unexpected<E>;
    template <typename U>
    using rebind = expected<U, error_type>;

    UTL_ATTRIBUTE(EXPECTED_INLINE_CONST)
    UTL_CONSTEXPR_CXX14 T* operator->() noexcept {
        return __UTL addressof(this->data_.value);
    }

    UTL_ATTRIBUTE(EXPECTED_INLINE_CONST) constexpr T const* operator->() const noexcept {
        return __UTL addressof(this->data_.value);
    }

    UTL_ATTRIBUTE(EXPECTED_INLINE_CONST)
    UTL_CONSTEXPR_CXX14 T& operator*() & noexcept { return this->data_.value; }

    UTL_ATTRIBUTE(EXPECTED_INLINE_CONST)
    UTL_CONSTEXPR_CXX14 T&& operator*() && noexcept {
        return __UTL move(this->data_.value);
    }

    UTL_ATTRIBUTE(EXPECTED_INLINE_CONST) constexpr T const& operator*() const& noexcept {
        return this->data_.value;
    }
    UTL_ATTRIBUTE(EXPECTED_INLINE_CONST) constexpr T const&& operator*() const&& noexcept {
        return __UTL move(this->data_.value);
    }

    UTL_ATTRIBUTE(EXPECTED_INLINE_PURE) constexpr explicit operator bool() const noexcept {
        return this->has_value_;
    }
    UTL_ATTRIBUTE(EXPECTED_INLINE_PURE) constexpr bool has_value() const noexcept { return this->has_value_; }

    UTL_ATTRIBUTE(EXPECTED_INLINE_CONST)
    UTL_CONSTEXPR_CXX14 T& value() & noexcept { return this->data_.value; }

    UTL_ATTRIBUTE(EXPECTED_INLINE_CONST)
    UTL_CONSTEXPR_CXX14 T&& value() && noexcept {
        return __UTL move(this->data_.value);
    }

    UTL_ATTRIBUTE(EXPECTED_INLINE_CONST) constexpr T const& value() const& noexcept {
        return this->data_.value;
    }

    UTL_ATTRIBUTE(EXPECTED_INLINE_CONST) constexpr T const&& value() const&& noexcept {
        return __UTL move(this->data_.value);
    }

    UTL_ATTRIBUTE(EXPECTED_INLINE_CONST)
    UTL_CONSTEXPR_CXX14 E& error() & noexcept { return this->data_.error; }

    UTL_ATTRIBUTE(EXPECTED_INLINE_CONST)
    UTL_CONSTEXPR_CXX14 E&& error() && noexcept {
        return __UTL move(this->data_.error);
    }

    UTL_ATTRIBUTE(EXPECTED_INLINE_CONST) constexpr E const& error() const& noexcept {
        return this->data_.error;
    }

    UTL_ATTRIBUTE(EXPECTED_INLINE_CONST) constexpr E const&& error() const&& noexcept {
        return __UTL move(this->data_.error);
    }

    template <UTL_CONCEPT_CXX20(convertible_to<T>) U UTL_CONSTRAINT_CXX11(
        UTL_TRAIT_is_copy_constructible(T) && UTL_TRAIT_is_convertible(U, T))>
    UTL_CONSTRAINT_CXX20(is_copy_constructible_v<T>)
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) constexpr T value_or(U&& default_val) const& noexcept(
        UTL_TRAIT_is_nothrow_convertible(U, T)) {
        return has_value() ? **this : static_cast<T>(__UTL forward<U>(default_val));
    }

    template <UTL_CONCEPT_CXX20(convertible_to<T>) U UTL_CONSTRAINT_CXX11(
        UTL_TRAIT_is_move_constructible(T) && UTL_TRAIT_is_convertible(U, T))>
    UTL_CONSTRAINT_CXX20(is_move_constructible_v<T>)
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) constexpr T value_or(U&& default_val) && noexcept(
        UTL_TRAIT_is_nothrow_convertible(U, T)) {
        return has_value() ? __UTL move(**this) : static_cast<T>(__UTL forward<U>(default_val));
    }

    template <UTL_CONCEPT_CXX20(convertible_to<E>) U = E UTL_CONSTRAINT_CXX11(
        UTL_TRAIT_is_copy_constructible(E) && UTL_TRAIT_is_convertible(U, E))>
    UTL_CONSTRAINT_CXX20(is_copy_constructible_v<E>)
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) constexpr E error_or(U&& default_val) const& noexcept(
        UTL_TRAIT_is_nothrow_convertible(U, E)) {
        return has_value() ? static_cast<E>(__UTL forward<U>(default_val)) : error();
    }

    template <UTL_CONCEPT_CXX20(convertible_to<E>) U = E UTL_CONSTRAINT_CXX11(
        UTL_TRAIT_is_move_constructible(E) && UTL_TRAIT_is_convertible(U, E))>
    UTL_CONSTRAINT_CXX20(is_move_constructible_v<E>)
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) constexpr E error_or(U&& default_val) && noexcept(
        UTL_TRAIT_is_nothrow_convertible(U, E)) {
        return has_value() ? static_cast<E>(__UTL forward<U>(default_val)) : __UTL move(error());
    }

    template <UTL_CONCEPT_CXX20(invocable<T&>) F UTL_CONSTRAINT_CXX11(
        UTL_TRAIT_is_invocable(F, T&) && UTL_TRAIT_is_constructible(E, E&))>
    UTL_CONSTRAINT_CXX20(is_constructible_v<E, E&>)
    UTL_CONSTEXPR_CXX14 auto and_then(F&& f) & noexcept(UTL_TRAIT_is_nothrow_invocable(F, T&))
        -> invoke_result_t<F, T&> {
        using return_type = remove_cvref_t<invoke_result_t<F, T&>>;
        static_assert(__UTL_TRAIT_is_expected(return_type) &&
                UTL_TRAIT_is_same(typename return_type::error_type, E),
            "Invalid return type");
        if (has_value()) {
            return __UTL invoke(f, value());
        }

        return return_type{unexpect, error()};
    }

    template <UTL_CONCEPT_CXX20(invocable<T const&>) F UTL_CONSTRAINT_CXX11(
        UTL_TRAIT_is_invocable(F, T const&) && UTL_TRAIT_is_constructible(E, E const&))>
    UTL_CONSTRAINT_CXX20(is_constructible_v<E, E const&>)
    constexpr auto and_then(F&& f) const& noexcept(UTL_TRAIT_is_nothrow_invocable(F, T const&))
        -> invoke_result_t<F, T const&> {
        using return_type = remove_cvref_t<invoke_result_t<F, T const&>>;
        static_assert(__UTL_TRAIT_is_expected(return_type) &&
                UTL_TRAIT_is_same(typename return_type::error_type, E),
            "Invalid return type");

        return has_value() ? __UTL invoke(f, value()) : return_type{__UTL unexpect, error()};
    }

    template <UTL_CONCEPT_CXX20(invocable<T>) F UTL_CONSTRAINT_CXX11(
        UTL_TRAIT_is_invocable(F, T) && UTL_TRAIT_is_constructible(E, E&&))>
    UTL_CONSTRAINT_CXX20(is_constructible_v<E, E &&>)
    UTL_CONSTEXPR_CXX14 auto and_then(F&& f) && noexcept(UTL_TRAIT_is_nothrow_invocable(F, T&&))
        -> invoke_result_t<F, T> {
        using return_type = remove_cvref_t<invoke_result_t<F, T&&>>;
        static_assert(__UTL_TRAIT_is_expected(return_type) &&
                UTL_TRAIT_is_same(typename return_type::error_type, E),
            "Invalid return type");

        if (has_value()) {
            return __UTL invoke(f, __UTL move(value()));
        }

        return return_type{__UTL unexpect, __UTL move(error())};
    }

    template <UTL_CONCEPT_CXX20(invocable<T const>) F UTL_CONSTRAINT_CXX11(
        UTL_TRAIT_is_invocable(F, T const) && UTL_TRAIT_is_constructible(E, E const&&))>
    UTL_CONSTRAINT_CXX20(is_constructible_v<E, E const &&>)
    constexpr auto and_then(F&& f) const&& noexcept(UTL_TRAIT_is_nothrow_invocable(F, T const&&))
        -> invoke_result_t<F, T const> {
        using return_type = remove_cvref_t<invoke_result_t<F, T const&&>>;
        static_assert(__UTL_TRAIT_is_expected(return_type) &&
                UTL_TRAIT_is_same(typename return_type::error_type, E),
            "Invalid return type");

        return has_value() ? __UTL invoke(f, __UTL move(value()))
                           : return_type{__UTL unexpect, __UTL move(error())};
    }

    template <UTL_CONCEPT_CXX20(invocable<T&>) F UTL_CONSTRAINT_CXX11(
        UTL_TRAIT_is_invocable(F, T&) && UTL_TRAIT_is_constructible(E, E&))>
    UTL_CONSTRAINT_CXX20(is_constructible_v<E, E&>)
    UTL_CONSTEXPR_CXX14 auto transform(F&& f) & noexcept(UTL_TRAIT_is_nothrow_invocable(F, T&))
        -> expected<remove_cv_t<invoke_result_t<F, T&>>, E> {
        using return_type = remove_cv_t<invoke_result_t<F, T&>>;
        static_assert(UTL_TRAIT_is_complete(expected<return_type, E>) &&
                (UTL_TRAIT_is_void(return_type) ||
                    UTL_TRAIT_is_constructible(return_type, invoke_result_t<F, T&>)),
            "Invalid return type");
        if (has_value()) {
            return details::expected::transformation(__UTL forward<F>(f), value());
        }

        return expected<return_type, E>{__UTL unexpect, error()};
    }

    template <UTL_CONCEPT_CXX20(invocable<T const&>) F UTL_CONSTRAINT_CXX11(
        UTL_TRAIT_is_invocable(F, T const&) && UTL_TRAIT_is_constructible(E, E const&))>
    UTL_CONSTRAINT_CXX20(is_constructible_v<E, E const&>)
    constexpr auto transform(F&& f) const& noexcept(UTL_TRAIT_is_nothrow_invocable(F, T const&))
        -> expected<remove_cv_t<invoke_result_t<F, T const&>>, E> {
        using return_type = remove_cv_t<invoke_result_t<F, T const&>>;
        static_assert(UTL_TRAIT_is_complete(expected<return_type, E>) &&
                (UTL_TRAIT_is_void(return_type) ||
                    UTL_TRAIT_is_constructible(return_type, invoke_result_t<F, T const&>)),
            "Invalid return type");

        return has_value() ? details::expected::transformation(__UTL forward<F>(f), value())
                           : expected<return_type, E>{__UTL unexpect, error()};
    }

    template <UTL_CONCEPT_CXX20(invocable<T>) F UTL_CONSTRAINT_CXX11(
        UTL_TRAIT_is_invocable(F, T) && UTL_TRAIT_is_constructible(E, E&&))>
    UTL_CONSTRAINT_CXX20(is_constructible_v<E, E &&>)
    UTL_CONSTEXPR_CXX14 auto transform(F&& f) && noexcept(UTL_TRAIT_is_nothrow_invocable(F, T&&))
        -> expected<remove_cv_t<invoke_result_t<F, T>>, E> {
        using return_type = remove_cv_t<invoke_result_t<F, T>>;
        static_assert(UTL_TRAIT_is_complete(expected<return_type, E>) &&
                (UTL_TRAIT_is_void(return_type) ||
                    UTL_TRAIT_is_constructible(return_type, invoke_result_t<F, T>)),
            "Invalid return type");

        if (has_value()) {
            return details::expected::transformation(__UTL forward<F>(f), __UTL move(value()));
        }

        return expected<return_type, E>{__UTL unexpect, __UTL move(error())};
    }

    template <UTL_CONCEPT_CXX20(invocable<T&&>) F UTL_CONSTRAINT_CXX11(
        UTL_TRAIT_is_invocable(F, T const) && UTL_TRAIT_is_constructible(E, E const&))>
    UTL_CONSTRAINT_CXX20(is_constructible_v<E, E const&>)
    constexpr auto transform(F&& f) const&& noexcept(UTL_TRAIT_is_nothrow_invocable(F, T const&&))
        -> expected<remove_cv_t<invoke_result_t<F, T const>>, E> {
        using return_type = remove_cv_t<invoke_result_t<F, T const>>;
        static_assert(UTL_TRAIT_is_complete(expected<return_type, E>) &&
                (UTL_TRAIT_is_void(return_type) ||
                    UTL_TRAIT_is_constructible(return_type, invoke_result_t<F, T const>)),
            "Invalid return type");

        return has_value()
            ? details::expected::transformation(__UTL forward<F>(f), __UTL move(value()))
            : expected<return_type, E>{__UTL unexpect, __UTL move(error())};
    }

private:
    union {
        value_type value_;
        error_type error_;
    };
    bool has_value_;
};

template <typename E>
class expected<void, E>;

template <typename E>
class expected<void const, E> : expected<void, E> {};
template <typename E>
class expected<void volatile, E> : expected<void, E> {};
template <typename E>
class expected<void const volatile, E> : expected<void, E> {};

UTL_NAMESPACE_END

#undef __UTL_ATTRIBUTE_TYPE_AGGREGATE_EXPECTED_INLINE_PURE
#undef __UTL_ATTRIBUTE_EXPECTED_INLINE_PURE
