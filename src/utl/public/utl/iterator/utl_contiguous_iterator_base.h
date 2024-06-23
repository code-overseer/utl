// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/concepts/utl_assignable_to.h"
#include "utl/concepts/utl_constructible_as.h"
#include "utl/iterator/utl_iterator_tags.h"
#include "utl/memory/utl_pointer_traits.h"
#include "utl/preprocessor/utl_config.h"
#include "utl/type_traits/utl_add_pointer.h"
#include "utl/type_traits/utl_enable_if.h"
#include "utl/type_traits/utl_is_assignable.h"
#include "utl/type_traits/utl_is_base_of.h"
#include "utl/type_traits/utl_is_constructible.h"
#include "utl/type_traits/utl_remove_const.h"

UTL_NAMESPACE_BEGIN

#define UTL_ITERATOR_CONST UTL_ATTRIBUTES(NODISCARD, CONST)
#define UTL_ITERATOR_PURE UTL_ATTRIBUTES(NODISCARD, PURE)

template <typename It, typename ValueType>
class contiguous_iterator_base {
public:
    using value_type = ValueType;
    using difference_type = typename pointer_traits<value_type*>::difference_type;
    using iterator_concept = contiguous_iterator_tag;

private:
    using pointer = remove_const_t<value_type>*;

    UTL_ITERATOR_CONST
    static constexpr value_type* get_ptr(It it) noexcept {
        static_assert(is_base_of<contiguous_iterator_base, It>::value, "Invalid iterator type");
        return ((contiguous_iterator_base const&)it).ptr_;
    }

    static constexpr It& set_ptr(It& it, value_type* value) noexcept {
        static_assert(is_base_of<contiguous_iterator_base, It>::value, "Invalid iterator type");
        return ((contiguous_iterator_base&)it).ptr_ = value, it;
    }

public:
    UTL_ITERATOR_PURE constexpr value_type& operator*() const noexcept { return *ptr_; }

    UTL_ITERATOR_PURE constexpr value_type* operator->() const noexcept { return ptr_; }

    UTL_ITERATOR_CONST
    friend constexpr It operator+(It it, difference_type offset) noexcept {
        return set_ptr(it, get_ptr(it) + offset), it;
    }

    UTL_ITERATOR_CONST
    friend constexpr It operator+(difference_type offset, It it) noexcept {
        return set_ptr(it, get_ptr(it) + offset), it;
    }

    UTL_ITERATOR_CONST
    friend constexpr It operator-(It it, difference_type offset) noexcept {
        return set_ptr(it, get_ptr(it) - offset), it;
    }

    UTL_ITERATOR_CONST
    friend constexpr difference_type operator-(It left, It right) noexcept {
        return get_ptr(left) - get_ptr(right);
    }

    friend constexpr It& operator+=(It& it, difference_type offset) noexcept {
        return set_ptr(it, get_ptr(it) + offset), it;
    }

    friend constexpr It& operator-=(It& it, difference_type offset) noexcept {
        return set_ptr(it, get_ptr(it) - offset), it;
    }

    friend constexpr It& operator++(It& it) noexcept { return set_ptr(it, get_ptr(it) + 1), it; }

    friend UTL_CONSTEXPR_CXX14 It operator++(It& it, int) noexcept {
        It before = it;
        ++it;
        return before;
    }

    friend constexpr It& operator--(It& it) noexcept { return set_ptr(it, get_ptr(it) - 1), it; }

    friend UTL_CONSTEXPR_CXX14 It operator--(It& it, int) noexcept {
        It before = it;
        --it;
        return before;
    }

    UTL_ITERATOR_PURE
    constexpr value_type& operator[](difference_type offset) const noexcept {
        return *(ptr_ + offset);
    }

    UTL_ITERATOR_PURE
    friend constexpr bool operator==(It const& left, It const& right) noexcept {
        return get_ptr(left) == get_ptr(right);
    }

    UTL_ITERATOR_PURE
    friend constexpr bool operator!=(It const& left, It const& right) noexcept {
        return get_ptr(left) != get_ptr(right);
    }

    UTL_ITERATOR_PURE
    friend constexpr bool operator<(It const& left, It const& right) noexcept {
        return get_ptr(left) < get_ptr(right);
    }

    UTL_ITERATOR_PURE
    friend constexpr bool operator>(It const& left, It const& right) noexcept {
        return get_ptr(left) > get_ptr(right);
    }

    UTL_ITERATOR_PURE
    friend constexpr bool operator<=(It const& left, It const& right) noexcept {
        return get_ptr(left) <= get_ptr(right);
    }

    UTL_ITERATOR_PURE
    friend constexpr bool operator>=(It const& left, It const& right) noexcept {
        return get_ptr(left) >= get_ptr(right);
    }

#ifdef UTL_CXX20
    UTL_ITERATOR_PURE
    friend constexpr auto operator<=>(It const& left, It const& right) noexcept {
        return get_ptr(left) <=> get_ptr(right);
    }
#endif

protected:
    explicit constexpr contiguous_iterator_base(value_type* ptr) noexcept : ptr_(ptr) {}
    constexpr contiguous_iterator_base() noexcept = default;
    constexpr contiguous_iterator_base(contiguous_iterator_base const&) noexcept = default;
    constexpr contiguous_iterator_base(contiguous_iterator_base&&) noexcept = default;
    UTL_CONSTEXPR_CXX14 contiguous_iterator_base& operator=(
        contiguous_iterator_base const&) noexcept = default;
    UTL_CONSTEXPR_CXX14 contiguous_iterator_base& operator=(contiguous_iterator_base&&) noexcept = default;
    UTL_CONSTEXPR_CXX20 ~contiguous_iterator_base() noexcept = default;

    template <UTL_CONCEPT_CXX20(constructible_as<pointer, UTL_SCOPE add_pointer>) T
            UTL_REQUIRES_CXX11(UTL_TRAIT_is_constructible(pointer, T*))>
    constexpr contiguous_iterator_base(contiguous_iterator_base<It, T> it) noexcept
        : ptr_(it.operator->()){};

    template <UTL_CONCEPT_CXX20(assignable_to<pointer, UTL_SCOPE add_pointer>) T UTL_REQUIRES_CXX11(
        UTL_TRAIT_is_assignable(pointer&, T*))>
    UTL_CONSTEXPR_CXX14 contiguous_iterator_base& operator=(contiguous_iterator_base<It, T> it) noexcept {
        ptr_ = it.operator->();
        return *this;
    }

private:
    pointer ptr_ = nullptr;
};

#undef UTL_ITERATOR_PURE
#undef UTL_ITERATOR_CONST

UTL_NAMESPACE_END
