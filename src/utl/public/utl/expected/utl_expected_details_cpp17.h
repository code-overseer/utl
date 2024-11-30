// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

#if !defined(UTL_EXPECTED_PRIVATE_HEADER_GUARD)
#  error "Private header accessed"
#endif

// TODO: ABI selection
#if UTL_CXX17
#  include "utl/expected/utl_expected_storage_cpp17.h"
#else
#  include "utl/expected/utl_expected_storage_cpp14.h"
#endif

UTL_NAMESPACE_BEGIN

namespace details {
namespace expected {

template <typename T, typename E,
    bool = UTL_TRAIT_is_copy_constructible(T) && UTL_TRAIT_is_copy_constructible(E),
    bool = UTL_TRAIT_is_trivially_copy_constructible(T) &&
        UTL_TRAIT_is_trivially_copy_constructible(E)>
class copy_ctor_t : protected underlying_storage<T, E> {
    using base_type = underlying_storage<T, E>;

public:
    using underlying_storage<T, E>::underlying_storage;

    __UTL_HIDE_FROM_ABI inline constexpr copy_ctor_t() noexcept(
        UTL_TRAIT_is_nothrow_default_constructible(base_type)) = default;
    __UTL_HIDE_FROM_ABI inline constexpr copy_ctor_t(copy_ctor_t const&) noexcept(
        UTL_TRAIT_is_nothrow_copy_constructible(base_type)) = default;
    __UTL_HIDE_FROM_ABI inline constexpr copy_ctor_t(copy_ctor_t&&) noexcept(
        UTL_TRAIT_is_nothrow_move_constructible(base_type)) = default;

    __UTL_HIDE_FROM_ABI inline ~copy_ctor_t() noexcept = default;

protected:
    using underlying_storage<T, E>::operator=;
};

template <typename T, typename E>
class copy_ctor_t<T, E, true, false> : protected underlying_storage<T, E> {
    using base_type = underlying_storage<T, E>;

public:
    using underlying_storage<T, E>::underlying_storage;
    __UTL_HIDE_FROM_ABI inline constexpr copy_ctor_t() noexcept(
        UTL_TRAIT_is_nothrow_default_constructible(base_type)) = default;
    __UTL_HIDE_FROM_ABI inline constexpr copy_ctor_t(copy_ctor_t&&) noexcept(
        UTL_TRAIT_is_nothrow_move_constructible(base_type)) = default;

    __UTL_HIDE_FROM_ABI inline constexpr copy_ctor_t(copy_ctor_t const& other) noexcept(
        UTL_TRAIT_is_nothrow_copy_constructible(T) && UTL_TRAIT_is_nothrow_copy_constructible(E))
        : base_type{converting, other.has_value(), other.data_ref()} {}

    __UTL_HIDE_FROM_ABI inline ~copy_ctor_t() noexcept = default;

protected:
    using underlying_storage<T, E>::operator=;
};

template <typename T, typename E,
    bool = UTL_TRAIT_is_move_constructible(T) && UTL_TRAIT_is_move_constructible(E),
    bool = UTL_TRAIT_is_trivially_move_constructible(T) &&
        UTL_TRAIT_is_trivially_move_constructible(E)>
class move_ctor_t : protected copy_ctor_t<T, E> {
    using base_type = copy_ctor_t<T, E>;

public:
    using copy_ctor_t<T, E>::copy_ctor_t;

    __UTL_HIDE_FROM_ABI inline constexpr move_ctor_t() noexcept(
        UTL_TRAIT_is_nothrow_default_constructible(base_type)) = default;
    __UTL_HIDE_FROM_ABI inline constexpr move_ctor_t(move_ctor_t const&) noexcept(
        UTL_TRAIT_is_nothrow_copy_constructible(base_type)) = default;
    __UTL_HIDE_FROM_ABI inline constexpr move_ctor_t(move_ctor_t&&) noexcept(
        UTL_TRAIT_is_nothrow_move_constructible(base_type)) = default;

    __UTL_HIDE_FROM_ABI inline ~move_ctor_t() noexcept = default;

protected:
    using copy_ctor_t<T, E>::operator=;
};

template <typename T, typename E>
class move_ctor_t<T, E, true, false> : protected copy_ctor_t<T, E> {
    using base_type = copy_ctor_t<T, E>;

public:
    using copy_ctor_t<T, E>::copy_ctor_t;

    __UTL_HIDE_FROM_ABI inline constexpr move_ctor_t() noexcept(
        UTL_TRAIT_is_nothrow_default_constructible(base_type)) = default;
    __UTL_HIDE_FROM_ABI inline constexpr move_ctor_t(move_ctor_t const&) noexcept(
        UTL_TRAIT_is_nothrow_copy_constructible(base_type)) = default;

    __UTL_HIDE_FROM_ABI inline constexpr move_ctor_t(move_ctor_t&& other) noexcept(
        UTL_TRAIT_is_nothrow_move_constructible(T) && UTL_TRAIT_is_nothrow_move_constructible(E))
        : base_type{converting, other.has_value(), __UTL move(other.data_ref())} {}

    __UTL_HIDE_FROM_ABI inline ~move_ctor_t() noexcept = default;

protected:
    using copy_ctor_t<T, E>::operator=;
};

template <typename T, typename E,
    bool = UTL_TRAIT_is_copy_assignable(T) && UTL_TRAIT_is_copy_assignable(E),
    bool = UTL_TRAIT_is_trivially_copy_assignable(T) && UTL_TRAIT_is_trivially_copy_assignable(E)>
class copy_assign_t : protected move_ctor_t<T, E> {
    using base_type = move_ctor_t<T, E>;

public:
    using move_ctor_t<T, E>::move_ctor_t;
    __UTL_HIDE_FROM_ABI inline constexpr copy_assign_t() noexcept(
        UTL_TRAIT_is_nothrow_default_constructible(base_type)) = default;
    __UTL_HIDE_FROM_ABI inline constexpr copy_assign_t(copy_assign_t const&) noexcept(
        UTL_TRAIT_is_nothrow_copy_constructible(base_type)) = default;
    __UTL_HIDE_FROM_ABI inline constexpr copy_assign_t(copy_assign_t&&) noexcept(
        UTL_TRAIT_is_nothrow_move_constructible(base_type)) = default;

    __UTL_HIDE_FROM_ABI inline ~copy_assign_t() noexcept = default;

protected:
    using move_ctor_t<T, E>::operator=;
};

template <typename T, typename E>
class copy_assign_t<T, E, true, false> : protected move_ctor_t<T, E> {
    using base_type = move_ctor_t<T, E>;

public:
    using move_ctor_t<T, E>::move_ctor_t;
    __UTL_HIDE_FROM_ABI inline constexpr copy_assign_t() noexcept(
        UTL_TRAIT_is_nothrow_default_constructible(base_type)) = default;
    __UTL_HIDE_FROM_ABI inline constexpr copy_assign_t(copy_assign_t const&) noexcept(
        UTL_TRAIT_is_nothrow_copy_constructible(base_type)) = default;
    __UTL_HIDE_FROM_ABI inline constexpr copy_assign_t(copy_assign_t&&) noexcept(
        UTL_TRAIT_is_nothrow_move_constructible(base_type)) = default;

    __UTL_HIDE_FROM_ABI inline ~copy_assign_t() noexcept = default;

protected:
    using move_ctor_t<T, E>::operator=;

    __UTL_HIDE_FROM_ABI inline constexpr copy_assign_t& operator=(
        copy_assign_t const& other) noexcept(UTL_TRAIT_is_nothrow_copy_assignable(T) &&
        UTL_TRAIT_is_nothrow_copy_assignable(E) && UTL_TRAIT_is_nothrow_copy_constructible(T) &&
        UTL_TRAIT_is_nothrow_copy_constructible(E)) {
        if (this != __UTL addressof(other)) {
            assign(other);
        }

        return *this;
    }

private:
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline constexpr void assign(copy_assign_t const& other) {
        if (this->has_value() != other.has_value()) {
            if (other.has_value()) {
                this->reinitialize_as_value(other.value_ref());
            } else {
                this->reinitialize_as_error(other.error_ref());
            }
        } else if (other.has_value()) {
            this->value_ref() = other.value_ref();
        } else {
            this->error_ref() = other.error_ref();
        }
    }
};

template <typename T, typename E,
    bool = UTL_TRAIT_is_move_assignable(T) && UTL_TRAIT_is_move_assignable(E),
    bool = UTL_TRAIT_is_trivially_move_assignable(T) && UTL_TRAIT_is_trivially_move_assignable(E)>
class move_assign_t : protected copy_assign_t<T, E> {
    using base_type = copy_assign_t<T, E>;

public:
    using copy_assign_t<T, E>::copy_assign_t;
    __UTL_HIDE_FROM_ABI inline constexpr move_assign_t() noexcept(
        UTL_TRAIT_is_nothrow_default_constructible(base_type)) = default;
    __UTL_HIDE_FROM_ABI inline constexpr move_assign_t(move_assign_t const&) noexcept(
        UTL_TRAIT_is_nothrow_copy_constructible(base_type)) = default;
    __UTL_HIDE_FROM_ABI inline constexpr move_assign_t(move_assign_t&&) noexcept(
        UTL_TRAIT_is_nothrow_move_constructible(base_type)) = default;

    __UTL_HIDE_FROM_ABI inline ~move_assign_t() noexcept = default;

protected:
    using copy_assign_t<T, E>::operator=;
};

template <typename T, typename E>
class move_assign_t<T, E, true, false> : protected copy_assign_t<T, E> {
    using base_type = copy_assign_t<T, E>;

public:
    using copy_assign_t<T, E>::copy_assign_t;
    __UTL_HIDE_FROM_ABI inline constexpr move_assign_t() noexcept(
        UTL_TRAIT_is_nothrow_default_constructible(base_type)) = default;
    __UTL_HIDE_FROM_ABI inline constexpr move_assign_t(move_assign_t const&) noexcept(
        UTL_TRAIT_is_nothrow_copy_constructible(base_type)) = default;
    __UTL_HIDE_FROM_ABI inline constexpr move_assign_t(move_assign_t&&) noexcept(
        UTL_TRAIT_is_nothrow_move_constructible(base_type)) = default;

    __UTL_HIDE_FROM_ABI inline ~move_assign_t() noexcept = default;

protected:
    using copy_assign_t<T, E>::operator=;

    __UTL_HIDE_FROM_ABI inline constexpr move_assign_t& operator=(move_assign_t&& other) noexcept(
        UTL_TRAIT_is_nothrow_move_assignable(T) && UTL_TRAIT_is_nothrow_move_assignable(E) &&
        UTL_TRAIT_is_nothrow_move_constructible(T) && UTL_TRAIT_is_nothrow_move_constructible(E)) {
        assign(__UTL move(other));
        return *this;
    }

private:
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline constexpr void assign(move_assign_t&& other) {
        if (this->has_value() != other.has_value()) {
            if (other.has_value()) {
                this->reinitialize_as_value(__UTL move(other.value_ref()));
            } else {
                this->reinitialize_as_error(__UTL move(other.error_ref()));
            }
        } else if (other.has_value()) {
            this->value_ref() = __UTL move(other.value_ref());
        } else {
            this->error_ref() = __UTL move(other.error_ref());
        }
    }
};

template <typename D, typename T, typename E,
    bool = UTL_TRAIT_conjunction(is_swappable<T>, is_swappable<E>, is_move_constructible<T>,
        is_move_constructible<E>,
        disjunction<is_nothrow_move_constructible<T>, is_nothrow_move_constructible<E>>)>
class swap_base : protected move_assign_t<T, E> {
    static_assert(__UTL_TRAIT_is_expected(D), "Invalid derived type");
    using base_type = move_assign_t<T, E>;

public:
    using move_assign_t<T, E>::move_assign_t;
    __UTL_HIDE_FROM_ABI inline constexpr swap_base() noexcept(
        UTL_TRAIT_is_nothrow_default_constructible(base_type)) = default;
    __UTL_HIDE_FROM_ABI inline constexpr swap_base(swap_base const&) noexcept(
        UTL_TRAIT_is_nothrow_copy_constructible(base_type)) = default;
    __UTL_HIDE_FROM_ABI inline constexpr swap_base(swap_base&&) noexcept(
        UTL_TRAIT_is_nothrow_move_constructible(base_type)) = default;

    __UTL_HIDE_FROM_ABI inline constexpr void swap(D& other) noexcept(
        UTL_TRAIT_is_nothrow_swappable(T) && UTL_TRAIT_is_nothrow_swappable(E) &&
        UTL_TRAIT_is_nothrow_move_constructible(T) && UTL_TRAIT_is_nothrow_move_constructible(E)) {
        static_assert(UTL_TRAIT_is_base_of(swap_base, D), "Invalid definition");
        if (this->has_value() == other.has_value()) {
            if (this->has_value()) {
                __UTL ranges::swap(this->value_ref(), other.value_ref());
            } else {
                __UTL ranges::swap(this->error_ref(), other.error_ref());
            }
        } else if (this->has_value()) {
            this->cross_swap((swap_base&)other);
        } else {
            other.cross_swap((swap_base&)(*this));
        }
    }

    __UTL_HIDE_FROM_ABI friend inline constexpr void swap(D& left, D& right) noexcept(
        UTL_TRAIT_is_nothrow_swappable(T) && UTL_TRAIT_is_nothrow_swappable(E) &&
        UTL_TRAIT_is_nothrow_move_constructible(T) && UTL_TRAIT_is_nothrow_move_constructible(E)) {
        left.swap(right);
    }

    __UTL_HIDE_FROM_ABI inline ~swap_base() noexcept = default;

protected:
    using move_assign_t<T, E>::operator=;

private:
#if UTL_CXX17

    __UTL_HIDE_FROM_ABI inline void cross_swap(swap_base& other) noexcept(
        UTL_TRAIT_is_nothrow_move_constructible(T) && UTL_TRAIT_is_nothrow_move_constructible(E)) {
        UTL_ASSERT(this->has_value() && !other.has_value());
        if constexpr (UTL_TRAIT_is_nothrow_move_constructible(E)) {
            E tmp(__UTL move(other.error_ref()));
            UTL_TRY {
                other.reinitialize_as_value(__UTL move(this->value_ref()));
                this->reinitialize_as_error(__UTL move(tmp));
            } UTL_CATCH(...) {
                other.reinitialize_as_error(__UTL move(tmp));
                UTL_RETHROW();
            }
        } else {
            T tmp(__UTL move(this->value_ref()));
            UTL_TRY {
                this->reinitialize_as_error(__UTL move(other.error_ref()));
                other.reinitialize_as_value(__UTL move(tmp));
            } UTL_CATCH(...) {
                this->reinitialize_as_value(__UTL move(tmp));
                UTL_RETHROW();
            }
        }
    }

#else

    template <typename Self>
    __UTL_HIDE_FROM_ABI inline UTL_ENABLE_IF_CXX11(
        void, UTL_TRAIT_conjunction(is_same<swap_base, Self>, is_nothrow_move_constructible<E>))
    cross_swap(Self& other) noexcept(
        UTL_TRAIT_is_nothrow_move_constructible(T) && UTL_TRAIT_is_nothrow_move_constructible(E)) {
        UTL_ASSERT(this->has_value() && !other.has_value());
        E tmp(__UTL move(other.error_ref()));
        UTL_TRY {
            other.reinitialize_as_value(__UTL move(this->value_ref()));
            this->reinitialize_as_error(__UTL move(tmp));
        } UTL_CATCH(...) {
            other.reinitialize_as_error(__UTL move(tmp));
            UTL_RETHROW();
        }
    }

    template <typename Self>
    __UTL_HIDE_FROM_ABI inline UTL_ENABLE_IF_CXX11(
        void, UTL_TRAIT_is_same(swap_base, Self) && !UTL_TRAIT_is_nothrow_move_constructible(E))
    cross_swap(Self& other) noexcept(
        UTL_TRAIT_is_nothrow_move_constructible(T) && UTL_TRAIT_is_nothrow_move_constructible(E)) {
        UTL_ASSERT(this->has_value() && !other.has_value());
        T tmp(__UTL move(this->value_ref()));
        UTL_TRY {
            this->reinitialize_as_error(__UTL move(other.error_ref()));
            other.reinitialize_as_value(__UTL move(tmp));
        } UTL_CATCH(...) {
            this->reinitialize_as_value(__UTL move(tmp));
            UTL_RETHROW();
        }
    }

#endif
};

template <typename D, typename T, typename E>
class swap_base<D, T, E, false> : protected move_assign_t<T, E> {
    static_assert(__UTL_TRAIT_is_expected(D), "Invalid derived type");
    using base_type = move_assign_t<T, E>;

public:
    using move_assign_t<T, E>::move_assign_t;
    __UTL_HIDE_FROM_ABI inline constexpr swap_base() noexcept(
        UTL_TRAIT_is_nothrow_default_constructible(base_type)) = default;
    __UTL_HIDE_FROM_ABI inline constexpr swap_base(swap_base const&) noexcept(
        UTL_TRAIT_is_nothrow_copy_constructible(base_type)) = default;
    __UTL_HIDE_FROM_ABI inline constexpr swap_base(swap_base&&) noexcept(
        UTL_TRAIT_is_nothrow_move_constructible(base_type)) = default;

    __UTL_HIDE_FROM_ABI void swap(D&) = delete;

    __UTL_HIDE_FROM_ABI inline ~swap_base() noexcept = default;

protected:
    using move_assign_t<T, E>::operator=;
};

template <typename D, typename E>
class swap_base<D, empty_t, E, true> : protected move_assign_t<empty_t, E> {
    static_assert(__UTL_TRAIT_is_expected(D), "Invalid derived type");
    using base_type = move_assign_t<empty_t, E>;

public:
    using move_assign_t<empty_t, E>::move_assign_t;
    __UTL_HIDE_FROM_ABI inline constexpr swap_base() noexcept(
        UTL_TRAIT_is_nothrow_default_constructible(base_type)) = default;
    __UTL_HIDE_FROM_ABI inline constexpr swap_base(swap_base const&) noexcept(
        UTL_TRAIT_is_nothrow_copy_constructible(base_type)) = default;
    __UTL_HIDE_FROM_ABI inline constexpr swap_base(swap_base&&) noexcept(
        UTL_TRAIT_is_nothrow_move_constructible(base_type)) = default;

    __UTL_HIDE_FROM_ABI inline constexpr void swap(D& other) noexcept(
        UTL_TRAIT_is_nothrow_swappable(E) && UTL_TRAIT_is_nothrow_move_constructible(E)) {
        static_assert(UTL_TRAIT_is_base_of(swap_base, D), "Invalid definition");
        if (this->has_value() == other.has_value()) {
            if (!this->has_value()) {
                __UTL ranges::swap(this->error_ref(), other.error_ref());
            }
        } else if (this->has_value()) {
            this->cross_swap(other);
        } else {
            other.cross_swap(*this);
        }
    }

    using move_assign_t<empty_t, E>::operator=;

    __UTL_HIDE_FROM_ABI inline ~swap_base() noexcept = default;

private:
    __UTL_HIDE_FROM_ABI inline void cross_swap(swap_base& other) noexcept(
        UTL_TRAIT_is_nothrow_move_constructible(E)) {
        UTL_ASSERT(this->has_value() && !other.has_value());
        this->reinitialize_as_error(__UTL move(other.error_ref()));
        other->reinitialize_as_value();
    }
};

template <typename D, typename T, typename E>
class storage_base : protected swap_base<D, T, E> {
    static_assert(!UTL_TRAIT_is_same(remove_cvref_t<T>, empty_t), "Invalid value type");
    static_assert(!UTL_TRAIT_is_void(T), "Invalid value type");
    using base_type = swap_base<D, T, E>;

public:
    using swap_base<D, T, E>::swap_base;
    __UTL_HIDE_FROM_ABI inline constexpr storage_base() noexcept(
        UTL_TRAIT_is_nothrow_default_constructible(base_type)) = default;
    __UTL_HIDE_FROM_ABI inline constexpr storage_base(storage_base const&) noexcept(
        UTL_TRAIT_is_nothrow_copy_constructible(base_type)) = default;
    __UTL_HIDE_FROM_ABI inline constexpr storage_base(storage_base&&) noexcept(
        UTL_TRAIT_is_nothrow_move_constructible(base_type)) = default;

    __UTL_HIDE_FROM_ABI inline ~storage_base() noexcept = default;

protected:
    using swap_base<D, T, E>::operator=;
    using swap_base<D, T, E>::swap;
};

template <typename D, typename E>
class void_storage_base : protected swap_base<D, empty_t, E> {
    using base_type = swap_base<D, empty_t, E>;

public:
    using swap_base<D, empty_t, E>::swap_base;

    __UTL_HIDE_FROM_ABI inline constexpr void_storage_base() noexcept(
        UTL_TRAIT_is_nothrow_default_constructible(base_type)) = default;
    __UTL_HIDE_FROM_ABI inline constexpr void_storage_base(void_storage_base const&) noexcept(
        UTL_TRAIT_is_nothrow_copy_constructible(base_type)) = default;
    __UTL_HIDE_FROM_ABI inline constexpr void_storage_base(void_storage_base&&) noexcept(
        UTL_TRAIT_is_nothrow_move_constructible(base_type)) = default;

    __UTL_HIDE_FROM_ABI inline ~void_storage_base() noexcept = default;

protected:
    using swap_base<D, empty_t, E>::operator=;
    using swap_base<D, empty_t, E>::swap;
};

} // namespace expected
} // namespace details

UTL_NAMESPACE_END
