// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

#include "utl/concepts/utl_convertible_to.h"
#include "utl/concepts/utl_default_initializable.h"
#include "utl/concepts/utl_different_from.h"
#include "utl/iterator/utl_forward_iterator.h"
#include "utl/iterator/utl_input_iterator.h"
#include "utl/iterator/utl_iter_const_reference_t.h"
#include "utl/iterator/utl_iter_difference_t.h"
#include "utl/iterator/utl_iter_value_t.h"
#include "utl/iterator/utl_iterator_traits.h"
#include "utl/iterator/utl_to_address.h"
#include "utl/memory/utl_addressof.h"
#include "utl/type_traits/utl_add_const.h"
#include "utl/type_traits/utl_declval.h"
#include "utl/type_traits/utl_is_convertible.h"
#include "utl/type_traits/utl_is_nothrow_constructible.h"
#include "utl/type_traits/utl_is_nothrow_convertible.h"
#include "utl/type_traits/utl_is_nothrow_default_constructible.h"
#include "utl/type_traits/utl_is_nothrow_move_constructible.h"
#include "utl/type_traits/utl_is_same.h"
#include "utl/utility/utl_forward.h"
#include "utl/utility/utl_move.h"

UTL_NAMESPACE_BEGIN

namespace details::const_iterator {

#if UTL_CXX20
template <typename It>
struct category {};

template <forward_iterator It>
struct category {
    using iterator_category = typename iterator_traits<It>::iterator_category;
};

template <typename It>
struct concept_type {
    using type UTL_NODEBUG = __UTL input_iterator_tag;
}

template <__UTL contiguous_iterator It>
struct concept_type<It> {
    using type UTL_NODEBUG = __UTL random_access_iterator_tag;
};

template <__UTL random_access_iterator It>
struct concept_type<It> {
    using type UTL_NODEBUG = __UTL random_access_iterator_tag;
};

template <__UTL bidirectional_iterator It>
struct concept_type<It> {
    using type UTL_NODEBUG = __UTL bidirectional_iterator_tag;
};

template <__UTL forward_iterator It>
struct concept_type<It> {
    using type UTL_NODEBUG = __UTL forward_iterator_tag;
};

template <typename It>
using concept_type_t UTL_NODEBUG = typename concept_type<It>::type;

#else
template <typename It, typename = void>
struct category {};

template <typename It>
struct category<It, enable_if_t<UTL_TRAIT_is_forward_iterator(It)>> {
    using iterator_category = typename iterator_traits<It>::iterator_category;
};

template <typename It>
__UTL_HIDE_FROM_ABI auto concept_type(int, int, int) noexcept
    -> __UTL enable_if_t<UTL_TRAIT_is_contiguous_iterator(It), __UTL contiguous_iterator_tag>;
template <typename It>
__UTL_HIDE_FROM_ABI auto concept_type(int, int, float) noexcept
    -> __UTL enable_if_t<UTL_TRAIT_is_random_access_iterator(It), __UTL random_access_iterator_tag>;
template <typename It>
__UTL_HIDE_FROM_ABI auto concept_type(int, float, float) noexcept
    -> __UTL enable_if_t<UTL_TRAIT_is_bidirectional_iterator(It), __UTL bidirectional_iterator_tag>;
template <typename It>
__UTL_HIDE_FROM_ABI auto concept_type(float, float, float) noexcept
    -> __UTL enable_if_t<UTL_TRAIT_is_forward_iterator(It), __UTL forward_iterator_tag>;
template <typename It>
__UTL_HIDE_FROM_ABI auto concept_type(...) noexcept -> __UTL input_iterator_tag;

template <typename It>
using concept_type_t UTL_NODEBUG = decltype(__UTL details::const_iterator::concept_type<It>(0));

template <typename It,
    bool = UTL_TRAIT_is_lvalue_reference(iter_reference_t<It>) &&
        UTL_TRAIT_is_same(remove_cvref_t<iter_reference_t<It>>, iter_value_t<It>),
    bool = UTL_TRAIT_is_contiguous_iterator(It)>
class arrow {};

template <typename>
struct const_pointer;
template <typename T>
struct const_pointer<T*> {
    using type UTL_NODEBUG = add_const_t<T>*;
};

template <typename T>
using const_pointer_t = typename const_pointer<T>::type;

template <typename D, typename It>
class arrow<It, true, true> {
public:
    __UTL_HIDE_FROM_ABI inline constexpr auto operator->() const noexcept
        -> const_pointer_t<decltype(__UTL to_address(__UTL declval<It const&>()))> {
        return __UTL to_address(static_cast<D*>(this)->base());
    }
};

template <typename D, typename It>
class arrow<It, true, false> {
public:
    __UTL_HIDE_FROM_ABI inline constexpr auto operator->() const noexcept
        -> const_pointer_t<decltype(__UTL addressof(*__UTL declval<It const&>()))> {
        return __UTL addressof(*(static_cast<D*>(this)->base()));
    }
};

template <typename D, typename It, bool = UTL_TRAIT_is_bidirectional_iterator(It)>
class decrement {};

template <typename D, typename It>
class decrement<D, It, true> {
public:
    __UTL_HIDE_FROM_ABI friend inline UTL_CONSTEXPR_CXX14 D& operator--(D& derive) noexcept(
        noexcept(--__UTL declval<It&>())) {
        --derive.current;
        return *this;
    }

    __UTL_HIDE_FROM_ABI friend inline UTL_CONSTEXPR_CXX14 D operator--(D& derive, int) noexcept(
        noexcept(__UTL declval<It&>()--)) {
        return D(derive.current--);
    }
};

template <typename D, typename It, bool = UTL_TRAIT_is_forward_iterator(It)>
class post_increment {
    __UTL_HIDE_FROM_ABI friend inline UTL_CONSTEXPR_CXX14 void operator++(D& derive, int) noexcept(
        noexcept(++__UTL declval<It&>())) {
        ++derive.current;
    }
};

template <typename D, typename It>
class post_increment<D, It, true> {
    __UTL_HIDE_FROM_ABI friend inline UTL_CONSTEXPR_CXX14 D operator++(D& derive, int) noexcept(
        noexcept(__UTL declval<It&>()++)) {
        return D(derive.current++);
    }
};

#endif

} // namespace details::const_iterator

template <typename It>
class basic_const_iterator;
#if UTL_CXX20
template <input_iterator It>
class basic_const_iterator<It> : public details::const_iterator::category<It> {
#else
template <typename It>
class basic_const_iterator :
    public details::const_iterator::category<It>,
    public details::const_iterator::arrow<basic_const_iterator<It>, It>,
    public details::const_iterator::decrement<basic_const_iterator<It>, It>,
    public details::const_iterator::post_increment<basic_const_iterator<It>, It> {
    static_assert(UTL_TRAIT_is_input_iterator(It), "Invalid argument");
    friend details::const_iterator::decrement<basic_const_iterator, It>;
    friend details::const_iterator::post_increment<basic_const_iterator, It>;
#endif
public:
    using iterator_concept = details::const_iterator::concept_type_t<T>;
    using value_type = iter_value_t<It>;
    using difference_type = iter_difference_t<It>;

    __UTL_HIDE_FROM_ABI inline constexpr basic_const_iterator() noexcept(
        UTL_TRAIT_is_nothrow_default_constructible(It))
        UTL_CONSTRAINT_CXX20(default_initializable<It>)
    = default;

    __UTL_HIDE_FROM_ABI inline constexpr basic_const_iterator(It i) noexcept(
        UTL_TRAIT_is_nothrow_move_constructible(It))
        : current(__UTL move(it)) {}

    template <UTL_CONCEPT_CXX20(convertible_to<It>) Jt UTL_CONSTRAINT_CXX11(
        UTL_TRAIT_is_convertible(Jt, It))>
    __UTL_HIDE_FROM_ABI inline constexpr basic_const_iterator(
        basic_const_iterator<Jt> other) noexcept(UTL_TRAIT_is_nothrow_convertible(Jt, It))
        : current(other.base()) {}

    template <UTL_CONCEPT_CXX20(different_from<basic_const_iterator>) U UTL_CONSTRAINT_CXX11(
        !UTL_TRAIT_is_same(basic_const_iterator, U) && UTL_TRAIT_is_convertible(U, It))>
    UTL_CONSTRAINT_CXX20(convertible_to<U, It>)
    __UTL_HIDE_FROM_ABI inline constexpr basic_const_iterator(U&& other) noexcept(
        UTL_TRAIT_is_nothrow_convertible(U, It))
        : current(__UTL forward<U>(other)) {}

    __UTL_HIDE_FROM_ABI inline constexpr It const& base() const& noexcept { return current; }
    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 It&& base() && noexcept { return __UTL move(current); }

    __UTL_HIDE_FROM_ABI inline constexpr reference operator*() const
        noexcept(UTL_TRAIT_is_nothrow_convertible(It const&, reference)) {
        return static_cast<reference>(*base());
    }

#if UTL_CXX20
    __UTL_HIDE_FROM_ABI inline constexpr const auto* operator->() const
    requires (UTL_TRAIT_is_lvalue_reference(iter_reference_t<It>) &&
        same_as<remove_cvref_t<iter_reference_t<It>>, value_type> && contiguous_iterator<It>)
    {
        return __UTL to_address(base());
    }

    __UTL_HIDE_FROM_ABI inline constexpr const auto* operator->() const
    requires (UTL_TRAIT_is_lvalue_reference(iter_reference_t<It>) &&
        same_as<remove_cvref_t<iter_reference_t<It>>, value_type>)
    {
        return __UTL addressof(*base());
    }

    __UTL_HIDE_FROM_ABI inline constexpr reference operator[](difference_type n) const
        noexcept(UTL_TRAIT_is_nothrow_convertible(It const&, reference))
    requires (random_access_iterator<It>)
    {
        return static_cast<reference>(base()[n]);
    }

    __UTL_HIDE_FROM_ABI inline constexpr basic_const_iterator& operator++() noexcept(
        noexcept(++current)) {
        ++current;
        return *this;
    }

    __UTL_HIDE_FROM_ABI inline constexpr void operator++(int) noexcept(noexcept(++current)) {
        ++current;
        return *this;
    }

    __UTL_HIDE_FROM_ABI inline constexpr basic_const_iterator operator++(int) noexcept(
        noexcept(current++))
    requires forward_iterator<It>
    {
        return basic_const_iterator(current++);
    }

    __UTL_HIDE_FROM_ABI inline constexpr basic_const_iterator& operator--() noexcept(
        noexcept(--current))
    requires bidirectional_iterator<It>
    {
        --current;
        return *this;
    }

    __UTL_HIDE_FROM_ABI inline constexpr basic_const_iterator operator--(int) noexcept(
        noexcept(current--))
    requires bidirectional_iterator<It>
    {
        return basic_const_iterator(current--);
    }

    __UTL_HIDE_FROM_ABI inline constexpr reference operator+=(difference_type n) const
        noexcept(noexcept(current += 1))
    requires (random_access_iterator<It>)
    {
        current += n;
        return *this;
    }

    __UTL_HIDE_FROM_ABI inline constexpr reference operator-=(difference_type n) const
        noexcept(noexcept(current -= 1))
    requires (random_access_iterator<It>)
    {
        current -= n;
        return *this;
    }

#else
    /**
     * Arrow operator implemented in details::const_iterator::arrow
     */
    template <typename Idx,
        enable_if_t<UTL_TRAIT_is_random_access_iterator(It) &&
                UTL_TRAIT_is_convertible(Idx, difference_type),
            int> = 0>
    __UTL_HIDE_FROM_ABI inline constexpr reference operator[](Idx n) const
        noexcept(UTL_TRAIT_is_nothrow_convertible(It const&, reference)) {
        return static_cast<reference>(base()[static_cast<difference_type>(n)]);
    }

    __UTL_HIDE_FROM_ABI inline constexpr basic_const_iterator& operator++() noexcept(
        noexcept(++current)) {
        ++current;
        return *this;
    }

    template <typename Idx,
        enable_if_t<UTL_TRAIT_is_random_access_iterator(It) &&
                UTL_TRAIT_is_convertible(Idx, difference_type),
            int> = 0>
    __UTL_HIDE_FROM_ABI inline constexpr basic_const_iterator& operator+=(Idx n) const
        noexcept(noexcept(current += 1)) {
        current += static_cast<difference_type>(n);
        return *this;
    }

    template <typename Idx,
        enable_if_t<UTL_TRAIT_is_random_access_iterator(It) &&
                UTL_TRAIT_is_convertible(Idx, difference_type),
            int> = 0>
    __UTL_HIDE_FROM_ABI inline constexpr basic_const_iterator& operator-=(Idx n) const
        noexcept(noexcept(current -= 1)) {
        current -= static_cast<difference_type>(n);
        return *this;
    }
#endif

private:
    using reference = iter_const_reference_t<It>;
    It current;
};

UTL_NAMESPACE_END
