// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_config.h"

#include "utl/concepts/utl_assignable_to.h"
#include "utl/concepts/utl_constructible_as.h"
#include "utl/iterator/utl_iterator_tags.h"
#include "utl/memory/utl_pointer_traits.h"
#include "utl/type_traits/utl_add_pointer.h"
#include "utl/type_traits/utl_enable_if.h"
#include "utl/type_traits/utl_is_assignable.h"
#include "utl/type_traits/utl_is_base_of.h"
#include "utl/type_traits/utl_is_constructible.h"
#include "utl/type_traits/utl_remove_const.h"

UTL_NAMESPACE_BEGIN

template <typename It, typename ValueType>
class contiguous_iterator_base {
public:
    using value_type = ValueType;
    using pointer = value_type*;
    using reference = value_type&;
    using difference_type = typename UTL_SCOPE pointer_traits<pointer>::difference_type;
    using iterator_concept = UTL_SCOPE contiguous_iterator_tag;

private:
    using stored_pointer UTL_NODEBUG = UTL_SCOPE remove_const_t<value_type>*;

    UTL_ATTRIBUTES(NODISCARD, PURE, _HIDE_FROM_ABI) static constexpr value_type* get_ptr(
        It const& it) noexcept {
        static_assert(
            UTL_SCOPE is_base_of<contiguous_iterator_base, It>::value, "Invalid iterator type");
        return ((contiguous_iterator_base const&)it).ptr_;
    }

    __UTL_HIDE_FROM_ABI static constexpr It& set_ptr(It& it, value_type* value) noexcept {
        static_assert(
            UTL_SCOPE is_base_of<contiguous_iterator_base, It>::value, "Invalid iterator type");
        return ((contiguous_iterator_base&)it).ptr_ = value, it;
    }

public:
    UTL_ATTRIBUTES(NODISCARD, PURE, ALWAYS_INLINE, _HIDE_FROM_ABI) constexpr value_type&
    operator*() const noexcept {
        return *ptr_;
    }

    UTL_ATTRIBUTES(NODISCARD, PURE, ALWAYS_INLINE, _HIDE_FROM_ABI) constexpr value_type*
    operator->() const noexcept {
        return ptr_;
    }

    UTL_ATTRIBUTES(NODISCARD, CONST, FLATTEN, _HIDE_FROM_ABI) friend constexpr It operator+(
        It it, difference_type offset) noexcept {
        return set_ptr(it, get_ptr(it) + offset), it;
    }

    UTL_ATTRIBUTES(NODISCARD, CONST, FLATTEN, _HIDE_FROM_ABI) friend constexpr It operator+(
        difference_type offset, It it) noexcept {
        return set_ptr(it, get_ptr(it) + offset), it;
    }

    UTL_ATTRIBUTES(NODISCARD, CONST, FLATTEN, _HIDE_FROM_ABI) friend constexpr It operator-(
        It it, difference_type offset) noexcept {
        return set_ptr(it, get_ptr(it) - offset), it;
    }

    UTL_ATTRIBUTES(NODISCARD, CONST, FLATTEN, _HIDE_FROM_ABI) friend constexpr difference_type operator-(
        It left, It right) noexcept {
        return get_ptr(left) - get_ptr(right);
    }

    UTL_ATTRIBUTES(FLATTEN, _HIDE_FROM_ABI) friend constexpr It& operator+=(
        It& it, difference_type offset) noexcept {
        return set_ptr(it, get_ptr(it) + offset), it;
    }

    UTL_ATTRIBUTES(FLATTEN, _HIDE_FROM_ABI) friend constexpr It& operator-=(
        It& it, difference_type offset) noexcept {
        return set_ptr(it, get_ptr(it) - offset), it;
    }

    UTL_ATTRIBUTES(FLATTEN, _HIDE_FROM_ABI) friend constexpr It& operator++(It& it) noexcept {
        return set_ptr(it, get_ptr(it) + 1), it;
    }

    UTL_ATTRIBUTES(FLATTEN, _HIDE_FROM_ABI) friend UTL_CONSTEXPR_CXX14 It operator++(It& it, int) noexcept {
        It before = it;
        ++it;
        return before;
    }

    UTL_ATTRIBUTES(FLATTEN, _HIDE_FROM_ABI) friend constexpr It& operator--(It& it) noexcept {
        return set_ptr(it, get_ptr(it) - 1), it;
    }

    UTL_ATTRIBUTES(FLATTEN, _HIDE_FROM_ABI) friend UTL_CONSTEXPR_CXX14 It operator--(It& it, int) noexcept {
        It before = it;
        --it;
        return before;
    }

    UTL_ATTRIBUTES(NODISCARD, PURE, FLATTEN) constexpr value_type& operator[](
        difference_type offset) const noexcept {
        return *(ptr_ + offset);
    }

    UTL_ATTRIBUTES(NODISCARD, PURE, FLATTEN, _HIDE_FROM_ABI) friend constexpr bool operator==(
        It const& left, It const& right) noexcept {
        return get_ptr(left) == get_ptr(right);
    }

    UTL_ATTRIBUTES(NODISCARD, PURE, FLATTEN, _HIDE_FROM_ABI) friend constexpr bool operator!=(
        It const& left, It const& right) noexcept {
        return get_ptr(left) != get_ptr(right);
    }

    UTL_ATTRIBUTES(NODISCARD, PURE, FLATTEN, _HIDE_FROM_ABI) friend constexpr bool operator<(
        It const& left, It const& right) noexcept {
        return get_ptr(left) < get_ptr(right);
    }

    UTL_ATTRIBUTES(NODISCARD, PURE, FLATTEN, _HIDE_FROM_ABI) friend constexpr bool operator>(
        It const& left, It const& right) noexcept {
        return get_ptr(left) > get_ptr(right);
    }

    UTL_ATTRIBUTES(NODISCARD, PURE, FLATTEN, _HIDE_FROM_ABI) friend constexpr bool operator<=(
        It const& left, It const& right) noexcept {
        return get_ptr(left) <= get_ptr(right);
    }

    UTL_ATTRIBUTES(NODISCARD, PURE, FLATTEN, _HIDE_FROM_ABI) friend constexpr bool operator>=(
        It const& left, It const& right) noexcept {
        return get_ptr(left) >= get_ptr(right);
    }

#ifdef UTL_CXX20
    UTL_ATTRIBUTES(NODISCARD, PURE, FLATTEN, _HIDE_FROM_ABI) friend constexpr auto operator<=>(
        It const& left, It const& right) noexcept {
        return get_ptr(left) <=> get_ptr(right);
    }
#endif

protected:
    __UTL_HIDE_FROM_ABI explicit constexpr contiguous_iterator_base(value_type* ptr) noexcept
        : ptr_(ptr) {}
    __UTL_HIDE_FROM_ABI constexpr contiguous_iterator_base() noexcept = default;
    __UTL_HIDE_FROM_ABI constexpr contiguous_iterator_base(
        contiguous_iterator_base const&) noexcept = default;
    __UTL_HIDE_FROM_ABI constexpr contiguous_iterator_base(
        contiguous_iterator_base&&) noexcept = default;
    __UTL_HIDE_FROM_ABI UTL_CONSTEXPR_CXX14 contiguous_iterator_base& operator=(
        contiguous_iterator_base const&) noexcept = default;
    __UTL_HIDE_FROM_ABI UTL_CONSTEXPR_CXX14 contiguous_iterator_base& operator=(
        contiguous_iterator_base&&) noexcept = default;
    __UTL_HIDE_FROM_ABI UTL_CONSTEXPR_CXX20 ~contiguous_iterator_base() noexcept = default;

    template <UTL_CONCEPT_CXX20(constructible_as<stored_pointer, UTL_SCOPE add_pointer>) T
            UTL_REQUIRES_CXX11(UTL_TRAIT_is_constructible(stored_pointer, T*))>
    __UTL_HIDE_FROM_ABI constexpr contiguous_iterator_base(
        contiguous_iterator_base<It, T> it) noexcept
        : ptr_(it.operator->()){};

    template <UTL_CONCEPT_CXX20(assignable_to<stored_pointer, UTL_SCOPE add_pointer>) T
            UTL_REQUIRES_CXX11(UTL_TRAIT_is_assignable(stored_pointer&, T*))>
    __UTL_HIDE_FROM_ABI UTL_CONSTEXPR_CXX14 contiguous_iterator_base& operator=(
        contiguous_iterator_base<It, T> it) noexcept {
        ptr_ = it.operator->();
        return *this;
    }

private:
    stored_pointer ptr_ = nullptr;
};

UTL_NAMESPACE_END
