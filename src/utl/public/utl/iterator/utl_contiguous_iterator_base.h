// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/base_preprocessor.h"
#include "utl/memory/utl_pointer_traits.h"
#include "utl/type_traits/utl_enable_if.h"
#include "utl/type_traits/utl_is_base_of.h"
#include "utl/type_traits/utl_is_convertible.h"
#include "utl/type_traits/utl_remove_const.h"

UTL_NAMESPACE_BEGIN

template <typename It, typename ValueType>
class contiguous_iterator_base {
    using value_type = ValueType;
    using difference_type = typename pointer_traits<value_type*>::difference_type;
    using size_type = decltype(sizeof(0));

    static constexpr value_type* get_ptr(It it) noexcept {
        static_assert(is_base_of<contiguous_iterator_base, It>::value, "Invalid iterator type");
        return ((contiguous_iterator_base const&)it).ptr_;
    }

    static constexpr void set_ptr(It& it, value_type* value) noexcept {
        static_assert(is_base_of<contiguous_iterator_base, It>::value, "Invalid iterator type");
        ((contiguous_iterator_base&)it).ptr_ = value;
    }

    UTL_ATTRIBUTE(NODISCARD)
    friend constexpr value_type* as_ptr(contiguous_iterator_base it) noexcept { return it.ptr_; }

public:
    UTL_ATTRIBUTE(NODISCARD) constexpr value_type& operator*() const noexcept { return *ptr_; }

    UTL_ATTRIBUTE(NODISCARD) constexpr value_type* operator->() const noexcept { return ptr_; }

    UTL_ATTRIBUTE(NODISCARD)
    friend constexpr It operator+(It it, difference_type offset) noexcept {
        set_ptr(it, get_ptr(it) + offset);
        return it;
    }

    UTL_ATTRIBUTE(NODISCARD)
    friend constexpr It operator+(difference_type offset, It it) noexcept {
        set_ptr(it, get_ptr(it) + offset);
        return it;
    }

    UTL_ATTRIBUTE(NODISCARD)
    friend constexpr It operator-(It it, difference_type offset) noexcept {
        set_ptr(it, get_ptr(it) - offset);
        return it;
    }

    UTL_ATTRIBUTE(NODISCARD)
    friend constexpr difference_type operator-(It left, It right) noexcept {
        return get_ptr(left) - get_ptr(right);
    }

    friend constexpr It& operator+=(It& it, difference_type offset) noexcept {
        set_ptr(it, get_ptr(it) + offset);
        return it;
    }

    friend constexpr It& operator-=(It& it, difference_type offset) noexcept {
        set_ptr(it, get_ptr(it) - offset);
        return it;
    }

    friend constexpr It& operator++(It& it) noexcept {
        set_ptr(it, get_ptr(it) + 1);
        return it;
    }

    friend constexpr It operator++(It& it, int) noexcept {
        It before = it;
        ++it;
        return before;
    }

    friend constexpr It& operator--(It& it) noexcept {
        set_ptr(it, get_ptr(it) - 1);
        return it;
    }

    friend constexpr It operator--(It& it, int) noexcept {
        It before = it;
        --it;
        return before;
    }

    UTL_ATTRIBUTE(NODISCARD)
    constexpr value_type& operator[](difference_type offset) const noexcept {
        return *(it + offset);
    }

    UTL_ATTRIBUTE(NODISCARD)
    friend constexpr bool operator==(It const& left, It const& right) const noexcept {
        return get_ptr(left) == get_ptr(right);
    }

    UTL_ATTRIBUTE(NODISCARD)
    friend constexpr bool operator!=(It const& left, It const& right) const noexcept {
        return get_ptr(left) != get_ptr(right);
    }

    UTL_ATTRIBUTE(NODISCARD)
    friend constexpr bool operator<(It const& left, It const& right) const noexcept {
        return get_ptr(left) < get_ptr(right);
    }

    UTL_ATTRIBUTE(NODISCARD)
    friend constexpr bool operator>(It const& left, It const& right) const noexcept {
        return get_ptr(left) > get_ptr(right);
    }

    UTL_ATTRIBUTE(NODISCARD)
    friend constexpr bool operator<=(It const& left, It const& right) const noexcept {
        return get_ptr(left) <= get_ptr(right);
    }

    UTL_ATTRIBUTE(NODISCARD)
    friend constexpr bool operator>=(It const& left, It const& right) const noexcept {
        return get_ptr(left) >= get_ptr(right);
    }

#ifdef UTL_CXX20
    UTL_ATTRIBUTE(NODISCARD)
    friend constexpr auto operator<=>(It const& left, It const& right) const noexcept {
        return get_ptr(left) <=> get_ptr(right);
    }
#endif

protected:
    explicit constexpr contiguous_iterator_base(value_type* ptr) noexcept : ptr_(ptr) {}
    constexpr contiguous_iterator_base() noexcept = default;
    constexpr contiguous_iterator_base(contiguous_iterator_base const&) noexcept = default;
    constexpr contiguous_iterator_base(contiguous_iterator_base&&) noexcept = default;
    UTL_CONSTEXPR_CXX20 ~contiguous_iterator_base() noexcept = default;

    template <typename T UTL_REQUIRES_CXX11(is_convertible<T*, value_type*>::value)>
    UTL_REQUIRES_CXX20(requires(T* ptr) {
        { value_type* dst = ptr };
    })
    constexpr contiguous_iterator_base(contiguous_iterator_base<It, T> it) noexcept
        : ptr_(as_ptr(it)){};

    template <typename T UTL_REQUIRES_CXX11(is_convertible<T*, value_type*>::value)>
    UTL_REQUIRES_CXX20(requires(value_type*& dst, T* ptr) {
        { dst = ptr };
    })
    UTL_CONSTEXPR_CXX14 contiguous_iterator_base& operator=(
        contiguous_iterator_base<It, T> it) noexcept {
        ptr_ = as_ptr(it);
        return *this;
    }

private:
    remove_const_t<value_type>* ptr_ = nullptr;
};

UTL_NAMESPACE_END
