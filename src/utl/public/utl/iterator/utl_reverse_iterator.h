// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

#include "utl/iterator/utl_iter_difference_t.h"
#include "utl/iterator/utl_iter_move.h"
#include "utl/iterator/utl_iter_reference_t.h"
#include "utl/iterator/utl_iter_rvalue_reference_t.h"
#include "utl/iterator/utl_iter_swap.h"
#include "utl/iterator/utl_iter_value_t.h"
#include "utl/iterator/utl_iterator_concept_t.h"
#include "utl/iterator/utl_iterator_traits.h"
#include "utl/iterator/utl_legacy_bidirectional_iterator.h"
#include "utl/iterator/utl_legacy_random_access_iterator.h"
#include "utl/iterator/utl_next.h"
#include "utl/iterator/utl_prev.h"
#include "utl/iterator/utl_sized_sentinel_for.h"
#include "utl/type_traits/utl_enable_if.h"
#include "utl/type_traits/utl_is_convertible.h"
#include "utl/type_traits/utl_is_copy_assignable.h"
#include "utl/type_traits/utl_is_copy_constructible.h"
#include "utl/type_traits/utl_is_default_constructible.h"
#include "utl/type_traits/utl_is_equality_comparable.h"
#include "utl/type_traits/utl_is_inequality_comparable.h"
#include "utl/type_traits/utl_is_move_assignable.h"
#include "utl/type_traits/utl_is_move_constructible.h"
#include "utl/type_traits/utl_is_nothrow_convertible.h"
#include "utl/type_traits/utl_is_nothrow_copy_assignable.h"
#include "utl/type_traits/utl_is_nothrow_copy_constructible.h"
#include "utl/type_traits/utl_is_nothrow_default_constructible.h"
#include "utl/type_traits/utl_is_nothrow_move_assignable.h"
#include "utl/type_traits/utl_is_nothrow_move_constructible.h"
#include "utl/type_traits/utl_is_nothrow_subscriptable.h"
#include "utl/type_traits/utl_is_subordinate_comparable.h"
#include "utl/type_traits/utl_is_subscriptable.h"
#include "utl/type_traits/utl_is_superordinate_comparable.h"
#include "utl/type_traits/utl_is_three_way_comparable.h"

#if UTL_CXX20

UTL_NAMESPACE_BEGIN

namespace details {
namespace reverse_iterator {
template <typename T>
struct concept_type {
    using type UTL_NODEBUG = __UTL bidirectional_iterator_tag;
};
template <typename T>
using concept_type_t UTL_NODEBUG = typename concept_type<T>::type;
template <__UTL random_access_iterator T>
struct concept_type<T> {
    using type UTL_NODEBUG = __UTL random_access_iterator_tag;
};

template <typename T>
struct category_type {
    using type UTL_NODEBUG = typename __UTL iterator_traits<T>::iterator_category;
};
template <typename T>
using category_type_t UTL_NODEBUG = typename concept_type<T>::type;
template <__UTL details::iterator_concept::implements<__UTL random_access_iterator_tag> T>
struct category_type<T> {
    using type UTL_NODEBUG = __UTL random_access_iterator_tag;
};

} // namespace reverse_iterator
} // namespace details
UTL_NAMESPACE_END

#else

UTL_NAMESPACE_BEGIN
namespace details {
namespace reverse_iterator {
template <typename T>
__UTL_HIDE_FROM_ABI auto get_concept(int) noexcept -> __UTL
    enable_if_t<UTL_TRAIT_is_legacy_random_access_iterator(T), __UTL random_access_iterator_tag>;
template <typename T>
__UTL_HIDE_FROM_ABI auto get_concept(float) noexcept -> __UTL bidirectional_iterator_tag;
template <typename T>
using concept_type_t UTL_NODEBUG = decltype(__UTL details::reverse_iterator::get_concept<T>(0));

template <typename T>
__UTL_HIDE_FROM_ABI auto get_category(int) noexcept -> __UTL enable_if_t<
    __UTL details::iterator_concept::implements<__UTL random_access_iterator_tag, T>::value,
    __UTL random_access_iterator_tag>;
template <typename T>
__UTL_HIDE_FROM_ABI auto get_category(float) noexcept -> T;

template <typename T>
using category_type_t UTL_NODEBUG = decltype(__UTL details::reverse_iterator::get_category<T>(0));
} // namespace reverse_iterator
} // namespace details
UTL_NAMESPACE_END
#endif

UTL_NAMESPACE_BEGIN

template <typename It>
class __UTL_PUBLIC_TEMPLATE reverse_iterator {
private:
    static_assert(UTL_TRAIT_is_legacy_bidirectional_iterator(It), "Non-reversible iterator");
    using traits_type UTL_NODEBUG = iterator_traits<It>;

public:
    using iterator_type = It;
    using iterator_concept = details::reverse_iterator::concept_type_t<iterator_type>;
    using iterator_category =
        details::reverse_iterator::category_type_t<typename traits_type::iterator_category>;
    using value_type = iter_value_t<iterator_type>;
    using difference_type = iter_difference_t<iterator_type>;
    using pointer = typename traits_type::pointer;
    using reference = iter_reference_t<iterator_type>;

private:
    class inaccessible_t {
        ~inaccessible_t() = default;
    };

    using offset_t UTL_NODEBUG =
        __UTL conditional_t<UTL_TRAIT_is_base_of(random_access_iterator_tag, iterator_concept),
            difference_type, inaccessible_t>;
    using invalid_offset_t UTL_NODEBUG =
        __UTL conditional_t<!UTL_TRAIT_is_base_of(random_access_iterator_tag, iterator_concept),
            difference_type, inaccessible_t>;

    template <UTL_CONCEPT_CXX20(legacy_random_access_iterator) I UTL_CONSTRAINT_CXX11(
        UTL_TRAIT_is_legacy_random_access_iterator(I))>
    __UTL_HIDE_FROM_ABI static constexpr auto subscript(I const& current,
        difference_type idx) noexcept(UTL_TRAIT_is_nothrow_subscriptable(I, difference_type))
        -> decltype(__UTL declval<I const&>()[idx]) {
        return current[-idx - 1];
    }

    __UTL_HIDE_FROM_ABI static constexpr int subscript(
        iterator_type const&, inaccessible_t) noexcept {
        return 0;
    }

    template <typename I UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_pointer(I))>
    UTL_CONSTRAINT_CXX20(is_pointer_v<I>)
    __UTL_HIDE_FROM_ABI static constexpr auto redirect(I current, int) noexcept -> I {
        return current - 1;
    }

    template <typename I, bool NoThrow = noexcept(__UTL declval<I>().operator->())>
    __UTL_HIDE_FROM_ABI static constexpr auto redirect(I const& current, unsigned int) noexcept(
        NoThrow) -> decltype(current.operator->()) {
        return __UTL prev(current).operator->();
    }

    template <typename I>
    __UTL_HIDE_FROM_ABI static constexpr pointer redirect(I const&, unsigned short) noexcept {
        return 0;
    }

public:
    __UTL_HIDE_FROM_ABI constexpr reverse_iterator() noexcept(
        UTL_TRAIT_is_nothrow_default_constructible(iterator_type))
        : current() {}
    __UTL_HIDE_FROM_ABI constexpr explicit reverse_iterator(iterator_type it) noexcept(
        UTL_TRAIT_is_nothrow_copy_constructible(iterator_type))
        : current(it) {}
    __UTL_HIDE_FROM_ABI constexpr reverse_iterator(reverse_iterator const&) noexcept(
        UTL_TRAIT_is_nothrow_copy_constructible(iterator_type)) = default;
    __UTL_HIDE_FROM_ABI constexpr reverse_iterator(reverse_iterator&&) noexcept(
        UTL_TRAIT_is_nothrow_move_constructible(iterator_type)) = default;
    template <typename U UTL_CONSTRAINT_CXX11(
        !UTL_TRAIT_is_same(U, iterator_type) && UTL_TRAIT_is_convertible(U const&, iterator_type))>
    UTL_CONSTRAINT_CXX20(!same_as<U, iterator_type> && convertible_to<U const&, iterator_type>)
    __UTL_HIDE_FROM_ABI constexpr reverse_iterator(reverse_iterator<U> const& it) noexcept(
        UTL_TRAIT_is_nothrow_convertible(U const&, iterator_type))
        : current(it.base()) {}

    __UTL_HIDE_FROM_ABI UTL_CONSTEXPR_CXX14 reverse_iterator& operator=(reverse_iterator const&) noexcept(
        UTL_TRAIT_is_nothrow_copy_assignable(iterator_type)) = default;
    __UTL_HIDE_FROM_ABI UTL_CONSTEXPR_CXX14 reverse_iterator& operator=(reverse_iterator&&) noexcept(
        UTL_TRAIT_is_nothrow_move_assignable(iterator_type)) = default;
    template <typename U UTL_CONSTRAINT_CXX11(!UTL_TRAIT_is_same(U, iterator_type) &&
        UTL_TRAIT_is_convertible(U const&, iterator_type) &&
        UTL_TRAIT_is_assignable(iterator_type&, U const&))>
    UTL_CONSTRAINT_CXX20(!same_as<U, iterator_type> && convertible_to<U const&, iterator_type> &&
        assignable_from<iterator_type&, U const&>)
    __UTL_HIDE_FROM_ABI UTL_CONSTEXPR_CXX14 reverse_iterator&
    operator=(reverse_iterator<U> const& it) noexcept(
        UTL_TRAIT_is_nothrow_convertible(U const&, iterator_type) &&
        UTL_TRAIT_is_nothrow_assignable(iterator_type&, U const&)) {
        current = it.base();
        return *this;
    }

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD, FLATTEN) constexpr auto operator[](offset_t idx) const
        noexcept(noexcept(subscript(this->current, idx)))
            -> decltype(subscript(__UTL declval<iterator_type>(), idx)) {
        return subscript(this->current, idx);
    }

    void operator[](invalid_offset_t idx) const = delete;

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD, FLATTEN) constexpr reference operator*() const
        noexcept(noexcept(*__UTL prev(__UTL declval<iterator_type>()))) {
        return *__UTL prev(this->current);
    }

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD, FLATTEN) constexpr pointer operator->() const
        noexcept(noexcept(redirect(this->current, 0u)))
        UTL_CONSTRAINT_CXX20(__UTL is_pointer_v<iterator_type> ||
        requires(iterator_type const i) { i.operator->(); })
    {
        return redirect(this->current, 0u);
    }

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, FLATTEN)
    UTL_CONSTEXPR_CXX14 reverse_iterator& operator++() noexcept(
        noexcept(--this->current)) {
        return --this->current, *this;
    }

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, FLATTEN)
    UTL_CONSTEXPR_CXX14 reverse_iterator operator++(int) noexcept(
        noexcept(reverse_iterator(this->current--))) {
        return reverse_iterator(current--);
    }

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, FLATTEN)
    UTL_CONSTEXPR_CXX14 reverse_iterator& operator--() noexcept(
        noexcept(++this->current)) {
        return ++this->current, *this;
    }

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, FLATTEN)
    UTL_CONSTEXPR_CXX14 reverse_iterator operator--(int) noexcept(
        noexcept(reverse_iterator(this->current++))) {
        return reverse_iterator(this->current++);
    }

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, FLATTEN, NODISCARD) constexpr reverse_iterator operator+(
        difference_type n) const noexcept(noexcept(__UTL prev(this->current))) {
        return reverse_iterator(__UTL prev(this->current, n));
    }

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, FLATTEN, NODISCARD) constexpr reverse_iterator operator-(
        difference_type n) const noexcept(noexcept(__UTL next(this->current))) {
        return reverse_iterator(__UTL next(this->current, n));
    }

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, FLATTEN)
    UTL_CONSTEXPR_CXX14 reverse_iterator& operator+=(difference_type n) noexcept(
        noexcept(__UTL prev(this->current))) {
        current = __UTL prev(this->current, n);
        return *this;
    }

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, FLATTEN)
    UTL_CONSTEXPR_CXX14 reverse_iterator& operator-=(difference_type n) noexcept(
        noexcept(__UTL next(this->current))) {
        current = __UTL next(this->current, n);
        return *this;
    }

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE, NODISCARD) friend constexpr __UTL iter_rvalue_reference_t<iterator_type>
    iter_move(reverse_iterator const& it) noexcept(UTL_TRAIT_is_nothrow_copy_constructible(
        iterator_type) && noexcept(__UTL ranges::iter_move(--__UTL declval<iterator_type&>()))) {
        auto output = it.base();
        return __UTL ranges::iter_move(output--);
    }

    template <typename It2>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) friend constexpr void
    iter_swap(reverse_iterator const& lhs, reverse_iterator<It2> const& rhs) noexcept(
        UTL_TRAIT_is_nothrow_copy_constructible(iterator_type) &&
        UTL_TRAIT_is_nothrow_copy_constructible(It2) && noexcept(
            __UTL ranges::iter_swap(--__UTL declval<iterator_type&>(), --__UTL declval<It2&>()))) {
        auto l = lhs.base();
        auto r = rhs.base();
        __UTL ranges::iter_swap(--l, --r);
    }

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD, PURE) constexpr iterator_type base() const noexcept {
        return current;
    }

protected:
    iterator_type current;
};

template <UTL_CONCEPT_CXX20(__UTL legacy_bidirectional_iterator) It>
UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) constexpr __UTL reverse_iterator<It> make_reverse_iterator(
    It it) noexcept(UTL_TRAIT_is_nothrow_constructible(__UTL reverse_iterator<It>, It)) {
    static_assert(UTL_TRAIT_is_legacy_bidirectional_iterator(It),
        "Iterator type must be a bidirectional iterator");
    return __UTL reverse_iterator<It>(it);
}

#if UTL_CXX17
template <UTL_CONCEPT_CXX20(__UTL legacy_bidirectional_iterator) It>
explicit reverse_iterator(It) -> reverse_iterator<It>;
#endif

template <typename It>
UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD, ALWAYS_INLINE) constexpr reverse_iterator<It> operator+(
    typename reverse_iterator<It>::difference_type n,
    reverse_iterator<It> it) noexcept(noexcept(it + n)) {
    return it + n;
}

template <typename It>
UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD, ALWAYS_INLINE) constexpr reverse_iterator<It> operator-(
    typename reverse_iterator<It>::difference_type n,
    reverse_iterator<It> it) noexcept(noexcept(it - n)) {
    return it - n;
}

template <typename It1, typename It2>
UTL_CONSTRAINT_CXX20(requires(It1 l, It2 r) { l == r; })
UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD, ALWAYS_INLINE) constexpr auto
operator==(reverse_iterator<It1> const& l, reverse_iterator<It1> const& r) noexcept(
    UTL_TRAIT_is_nothrow_equality_comparable_with(decltype(l.base()), decltype(r.base())))
    -> UTL_ENABLE_IF_CXX11(
        bool, UTL_TRAIT_is_equality_comparable_with(decltype(l.base()), decltype(r.base()))) {
    return l.base() == r.base();
}

template <typename It1, typename It2>
UTL_CONSTRAINT_CXX20(requires(It1 l, It2 r) { l != r; })
UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD, ALWAYS_INLINE) constexpr auto
operator!=(reverse_iterator<It1> const& l, reverse_iterator<It1> const& r) noexcept(
    UTL_TRAIT_is_nothrow_inequality_comparable_with(decltype(l.base()), decltype(r.base())))
    -> UTL_ENABLE_IF_CXX11(
        bool, UTL_TRAIT_is_inequality_comparable_with(decltype(l.base()), decltype(r.base()))) {
    return l.base() != r.base();
}

template <typename It1, typename It2>
UTL_CONSTRAINT_CXX20(requires(It1 l, It2 r) { l < r; })
UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD, ALWAYS_INLINE) constexpr auto
operator<(reverse_iterator<It1> const& l, reverse_iterator<It1> const& r) noexcept(
    UTL_TRAIT_is_nothrow_strict_subordinate_comparable_with(decltype(l.base()), decltype(r.base())))
    -> UTL_ENABLE_IF_CXX11(bool,
        UTL_TRAIT_is_strict_subordinate_comparable_with(decltype(l.base()), decltype(r.base()))) {
    return l.base() < r.base();
}

template <typename It1, typename It2>
UTL_CONSTRAINT_CXX20(requires(It1 l, It2 r) { l > r; })
UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD, ALWAYS_INLINE) constexpr auto
operator>(reverse_iterator<It1> const& l, reverse_iterator<It1> const& r) noexcept(
    UTL_TRAIT_is_nothrow_strict_superordinate_comparable_with(
        decltype(l.base()), decltype(r.base())))
    -> UTL_ENABLE_IF_CXX11(bool,
        UTL_TRAIT_is_strict_superordinate_comparable_with(decltype(l.base()), decltype(r.base()))) {
    return l.base() > r.base();
}

template <typename It1, typename It2>
UTL_CONSTRAINT_CXX20(requires(It1 l, It2 r) { l <= r; })
UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD, ALWAYS_INLINE) constexpr auto
operator<=(reverse_iterator<It1> const& l, reverse_iterator<It1> const& r) noexcept(
    UTL_TRAIT_is_nothrow_subordinate_comparable_with(decltype(l.base()), decltype(r.base())))
    -> UTL_ENABLE_IF_CXX11(
        bool, UTL_TRAIT_is_subordinate_comparable_with(decltype(l.base()), decltype(r.base()))) {
    return l.base() <= r.base();
}

template <typename It1, typename It2>
UTL_CONSTRAINT_CXX20(requires(It1 l, It2 r) { l >= r; })
UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD, ALWAYS_INLINE) constexpr auto
operator>=(reverse_iterator<It1> const& l, reverse_iterator<It1> const& r) noexcept(
    UTL_TRAIT_is_nothrow_superordinate_comparable_with(decltype(l.base()), decltype(r.base())))
    -> UTL_ENABLE_IF_CXX11(
        bool, UTL_TRAIT_is_superordinate_comparable_with(decltype(l.base()), decltype(r.base()))) {
    return l.base() >= r.base();
}

#if UTL_CXX20
template <typename It1, typename It2>
UTL_CONSTRAINT_CXX20(requires(It1 l, It2 r) { l <=> r; })
UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD, ALWAYS_INLINE) constexpr auto
operator<=>(reverse_iterator<It1> const& l, reverse_iterator<It1> const& r) noexcept(
    UTL_TRAIT_is_nothrow_three_way_comparable_with(decltype(l.base()), decltype(r.base())))
    -> UTL_ENABLE_IF_CXX11(
        bool, UTL_TRAIT_is_three_way_comparable_with(decltype(l.base()), decltype(r.base()))) {
    return l.base() <=> r.base();
}
#endif

#if UTL_CXX14

template <typename It1, typename It2>
UTL_CONSTRAINT_CXX20(!sized_sentinel_for<It1, It2>)
UTL_INLINE_CXX17 constexpr bool disable_sized_sentinel_for<reverse_iterator<It1>, reverse_iterator<It2>> =
    !is_sized_sentinel_for_v<It1, It2>;

#else
namespace details {
namespace sized_sentinel_for {
template <typename It1, typename It2>
struct is_disabled<__UTL reverse_iterator<It1>, __UTL reverse_iterator<It2>> :
    __UTL negation<__UTL is_sized_sentinel_for<It1, It2>> {};
} // namespace sized_sentinel_for
} // namespace details
#endif

UTL_NAMESPACE_END
