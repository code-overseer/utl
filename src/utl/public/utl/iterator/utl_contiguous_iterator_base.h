// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

#include "utl/concepts/utl_assignable_to.h"
#include "utl/concepts/utl_constructible_as.h"
#include "utl/iterator/utl_iterator_tags.h"
#include "utl/memory/utl_pointer_traits.h"
#include "utl/type_traits/utl_add_pointer.h"
#include "utl/type_traits/utl_enable_if.h"
#include "utl/type_traits/utl_is_assignable.h"
#include "utl/type_traits/utl_is_base_of.h"
#include "utl/type_traits/utl_is_constructible.h"
#include "utl/type_traits/utl_is_convertible.h"
#include "utl/type_traits/utl_is_nothrow_convertible.h"
#include "utl/type_traits/utl_remove_const.h"

UTL_NAMESPACE_BEGIN

template <typename It, typename ValueType>
class contiguous_iterator_base {
public:
    using value_type = ValueType;
    using pointer = value_type*;
    using reference = value_type&;
    using difference_type = typename __UTL pointer_traits<pointer>::difference_type;
    using iterator_concept = __UTL contiguous_iterator_tag;

private:
    using stored_pointer UTL_NODEBUG = __UTL remove_const_t<value_type>*;

    UTL_ATTRIBUTES(NODISCARD, _HIDE_FROM_ABI) static inline constexpr value_type* get_ptr(
        It const& it) noexcept {
        static_assert(UTL_TRAIT_is_base_of(contiguous_iterator_base, It), "Invalid iterator type");
        return ((contiguous_iterator_base const&)it).ptr_;
    }

public:
    UTL_ATTRIBUTES(NODISCARD, ALWAYS_INLINE, _HIDE_FROM_ABI) inline constexpr value_type&
    operator*() const noexcept {
        return *ptr_;
    }

    UTL_ATTRIBUTES(NODISCARD, ALWAYS_INLINE, _HIDE_FROM_ABI) inline constexpr value_type*
    operator->() const noexcept {
        return ptr_;
    }

    UTL_ATTRIBUTES(NODISCARD, FLATTEN, _HIDE_FROM_ABI) inline constexpr It operator+(
        difference_type offset) const noexcept {
        return It(ptr_ + offset);
    }

    UTL_ATTRIBUTES(NODISCARD, FLATTEN, _HIDE_FROM_ABI) friend inline constexpr It operator+(
        difference_type offset, It const& it) noexcept {
        return It(get_ptr(it) + offset);
    }

    UTL_ATTRIBUTES(NODISCARD, FLATTEN, _HIDE_FROM_ABI) inline constexpr It operator-(
        difference_type offset) const noexcept {
        return It(ptr_ - offset);
    }

    UTL_ATTRIBUTES(NODISCARD, FLATTEN, _HIDE_FROM_ABI) friend inline constexpr difference_type operator-(
        It const& left, It const& right) noexcept {
        return get_ptr(left) - get_ptr(right);
    }

    UTL_ATTRIBUTES(FLATTEN, _HIDE_FROM_ABI) inline UTL_CONSTEXPR_CXX14 It& operator+=(difference_type offset) noexcept {
        ptr_ += offset;
        return static_cast<It&>(*this);
    }

    UTL_ATTRIBUTES(FLATTEN, _HIDE_FROM_ABI) inline UTL_CONSTEXPR_CXX14 It& operator-=(difference_type offset) noexcept {
        ptr_ -= offset;
        return static_cast<It&>(*this);
    }

    UTL_ATTRIBUTES(FLATTEN, _HIDE_FROM_ABI) inline UTL_CONSTEXPR_CXX14 It& operator++() noexcept {
        ++ptr_;
        return static_cast<It&>(*this);
    }

    UTL_ATTRIBUTES(FLATTEN, _HIDE_FROM_ABI) inline UTL_CONSTEXPR_CXX14 It operator++(int) noexcept {
        It before = *static_cast<It const*>(this);
        ++ptr_;
        return before;
    }

    UTL_ATTRIBUTES(FLATTEN, _HIDE_FROM_ABI) inline UTL_CONSTEXPR_CXX14 It& operator--() noexcept {
        --ptr_;
        return static_cast<It&>(*this);
    }

    UTL_ATTRIBUTES(FLATTEN, _HIDE_FROM_ABI) inline UTL_CONSTEXPR_CXX14 It operator--(int) noexcept {
        It before = *static_cast<It const*>(this);
        --ptr_;
        return before;
    }

    UTL_ATTRIBUTES(NODISCARD, FLATTEN) inline constexpr value_type& operator[](
        difference_type offset) const noexcept {
        return *(ptr_ + offset);
    }

    UTL_ATTRIBUTES(NODISCARD, FLATTEN, _HIDE_FROM_ABI) inline constexpr bool operator==(
        It const& right) const noexcept {
        return ptr_ == get_ptr(right);
    }

    UTL_ATTRIBUTES(NODISCARD, FLATTEN, _HIDE_FROM_ABI) inline constexpr bool operator!=(
        It const& right) const noexcept {
        return ptr_ != get_ptr(right);
    }

    UTL_ATTRIBUTES(NODISCARD, FLATTEN, _HIDE_FROM_ABI) inline constexpr bool operator<(
        It const& right) const noexcept {
        return ptr_ < get_ptr(right);
    }

    UTL_ATTRIBUTES(NODISCARD, FLATTEN, _HIDE_FROM_ABI) inline constexpr bool operator>(
        It const& right) const noexcept {
        return ptr_ > get_ptr(right);
    }

    UTL_ATTRIBUTES(NODISCARD, FLATTEN, _HIDE_FROM_ABI) inline constexpr bool operator<=(
        It const& right) const noexcept {
        return ptr_ <= get_ptr(right);
    }

    UTL_ATTRIBUTES(NODISCARD, FLATTEN, _HIDE_FROM_ABI) inline constexpr bool operator>=(
        It const& right) const noexcept {
        return ptr_ >= get_ptr(right);
    }

#if UTL_CXX20
    template <typename R>
    requires convertible_to<R const&, It const&>
    UTL_ATTRIBUTES(NODISCARD, FLATTEN, _HIDE_FROM_ABI) inline constexpr auto operator<=>(R const& right) const
        noexcept(noexcept(static_cast<It const&>(right))) {
        return ptr_ <=> get_ptr(static_cast<It const&>(right));
    }
#endif

protected:
    __UTL_HIDE_FROM_ABI explicit inline constexpr contiguous_iterator_base(value_type* ptr) noexcept
        : ptr_(ptr) {}
    __UTL_HIDE_FROM_ABI inline constexpr contiguous_iterator_base() noexcept = default;
    __UTL_HIDE_FROM_ABI inline constexpr contiguous_iterator_base(
        contiguous_iterator_base const&) noexcept = default;
    __UTL_HIDE_FROM_ABI inline constexpr contiguous_iterator_base(
        contiguous_iterator_base&&) noexcept = default;
    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 contiguous_iterator_base& operator=(
        contiguous_iterator_base const&) noexcept = default;
    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 contiguous_iterator_base& operator=(
        contiguous_iterator_base&&) noexcept = default;
    __UTL_HIDE_FROM_ABI UTL_CONSTEXPR_CXX20 ~contiguous_iterator_base() noexcept = default;

private:
    stored_pointer ptr_ = nullptr;
};

#if UTL_CXX20
#  define UTL_INHERIT_CONTIGUOUS_ITERATOR_MEMBERS(...)                        \
      friend contiguous_iterator_base<__VA_ARGS__>;                           \
      using typename contiguous_iterator_base<__VA_ARGS__>::difference_type;  \
      using typename contiguous_iterator_base<__VA_ARGS__>::iterator_concept; \
      using typename contiguous_iterator_base<__VA_ARGS__>::pointer;          \
      using typename contiguous_iterator_base<__VA_ARGS__>::reference;        \
      using typename contiguous_iterator_base<__VA_ARGS__>::value_type;       \
      using contiguous_iterator_base<__VA_ARGS__>::operator+;                 \
      using contiguous_iterator_base<__VA_ARGS__>::operator-;                 \
      using contiguous_iterator_base<__VA_ARGS__>::operator++;                \
      using contiguous_iterator_base<__VA_ARGS__>::operator--;                \
      using contiguous_iterator_base<__VA_ARGS__>::operator+=;                \
      using contiguous_iterator_base<__VA_ARGS__>::operator-=;                \
      using contiguous_iterator_base<__VA_ARGS__>::operator*;                 \
      using contiguous_iterator_base<__VA_ARGS__>::operator->;                \
      using contiguous_iterator_base<__VA_ARGS__>::operator<;                 \
      using contiguous_iterator_base<__VA_ARGS__>::operator<=;                \
      using contiguous_iterator_base<__VA_ARGS__>::operator>;                 \
      using contiguous_iterator_base<__VA_ARGS__>::operator>=;                \
      using contiguous_iterator_base<__VA_ARGS__>::operator==;                \
      using contiguous_iterator_base<__VA_ARGS__>::operator!=;                \
      using contiguous_iterator_base<__VA_ARGS__>::operator[];                \
      using contiguous_iterator_base<__VA_ARGS__>::operator<=>
#else
#  define UTL_INHERIT_CONTIGUOUS_ITERATOR_MEMBERS(...)                        \
      friend contiguous_iterator_base<__VA_ARGS__>;                           \
      using typename contiguous_iterator_base<__VA_ARGS__>::difference_type;  \
      using typename contiguous_iterator_base<__VA_ARGS__>::iterator_concept; \
      using typename contiguous_iterator_base<__VA_ARGS__>::pointer;          \
      using typename contiguous_iterator_base<__VA_ARGS__>::reference;        \
      using typename contiguous_iterator_base<__VA_ARGS__>::value_type;       \
      using contiguous_iterator_base<__VA_ARGS__>::operator+;                 \
      using contiguous_iterator_base<__VA_ARGS__>::operator-;                 \
      using contiguous_iterator_base<__VA_ARGS__>::operator++;                \
      using contiguous_iterator_base<__VA_ARGS__>::operator--;                \
      using contiguous_iterator_base<__VA_ARGS__>::operator+=;                \
      using contiguous_iterator_base<__VA_ARGS__>::operator-=;                \
      using contiguous_iterator_base<__VA_ARGS__>::operator*;                 \
      using contiguous_iterator_base<__VA_ARGS__>::operator->;                \
      using contiguous_iterator_base<__VA_ARGS__>::operator<;                 \
      using contiguous_iterator_base<__VA_ARGS__>::operator<=;                \
      using contiguous_iterator_base<__VA_ARGS__>::operator>;                 \
      using contiguous_iterator_base<__VA_ARGS__>::operator>=;                \
      using contiguous_iterator_base<__VA_ARGS__>::operator==;                \
      using contiguous_iterator_base<__VA_ARGS__>::operator!=;                \
      using contiguous_iterator_base<__VA_ARGS__>::operator[]
#endif
UTL_NAMESPACE_END
