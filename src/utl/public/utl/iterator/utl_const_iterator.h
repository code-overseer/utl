// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

#include "utl/concepts/utl_convertible_to.h"
#include "utl/concepts/utl_default_initializable.h"
#include "utl/concepts/utl_different_from.h"
#include "utl/concepts/utl_three_way_comparable.h"
#include "utl/concepts/utl_totally_ordered.h"
#include "utl/iterator/utl_bidirectional_iterator.h"
#include "utl/iterator/utl_constant_iterator.h"
#include "utl/iterator/utl_contiguous_iterator.h"
#include "utl/iterator/utl_forward_iterator.h"
#include "utl/iterator/utl_indirectly_readable.h"
#include "utl/iterator/utl_input_iterator.h"
#include "utl/iterator/utl_iter_const_reference_t.h"
#include "utl/iterator/utl_iter_difference_t.h"
#include "utl/iterator/utl_iter_move.h"
#include "utl/iterator/utl_iter_reference_t.h"
#include "utl/iterator/utl_iter_rvalue_reference_t.h"
#include "utl/iterator/utl_iter_value_t.h"
#include "utl/iterator/utl_iterator_traits.h"
#include "utl/iterator/utl_random_access_iterator.h"
#include "utl/iterator/utl_sentinel_for.h"
#include "utl/memory/utl_addressof.h"
#include "utl/memory/utl_to_address.h"
#include "utl/type_traits/utl_add_const.h"
#include "utl/type_traits/utl_common_reference.h"
#include "utl/type_traits/utl_declval.h"
#include "utl/type_traits/utl_is_convertible.h"
#include "utl/type_traits/utl_is_nothrow_constructible.h"
#include "utl/type_traits/utl_is_nothrow_convertible.h"
#include "utl/type_traits/utl_is_nothrow_default_constructible.h"
#include "utl/type_traits/utl_is_nothrow_move_constructible.h"
#include "utl/type_traits/utl_is_same.h"
#include "utl/type_traits/utl_is_subordinate_comparable.h"
#include "utl/type_traits/utl_is_superordinate_comparable.h"
#include "utl/type_traits/utl_is_totally_ordered.h"
#include "utl/utility/utl_forward.h"
#include "utl/utility/utl_move.h"

UTL_NAMESPACE_BEGIN

namespace details {
namespace const_iterator {

template <typename D>
class specialization_tag {
private:
    friend D;
    UTL_CONSTEXPR_CXX20 ~specialization_tag() noexcept = default;
};

#if UTL_CXX20
template <typename It>
struct category {};

template <forward_iterator It>
struct category<It> {
    using iterator_category = typename __UTL iterator_traits<It>::iterator_category;
};

template <typename It>
struct concept_type {
    using type UTL_NODEBUG = __UTL input_iterator_tag;
};

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

template <__UTL indirectly_readable I>
using const_rvalue_reference_t UTL_NODEBUG =
    __UTL common_reference_t<__UTL iter_value_t<I> const&&, __UTL iter_rvalue_reference_t<I>>;

#else // UTL_CXX20

template <typename I, typename = void>
struct const_rvalue_reference {};

template <typename I>
struct const_rvalue_reference<I, enable_if_t<UTL_TRAIT_is_indirectly_readable(I)>> {
    using type UTL_NODEBUG =
        __UTL common_reference_t<__UTL iter_value_t<I> const&&, __UTL iter_rvalue_reference_t<I>>;
};

template <typename I>
using const_rvalue_reference_t UTL_NODEBUG = typename const_rvalue_reference<I>::type;

template <typename It, typename = void>
struct category {};

template <typename It>
struct category<It, enable_if_t<UTL_TRAIT_is_forward_iterator(It)>> {
    using iterator_category = typename __UTL iterator_traits<It>::iterator_category;
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

template <typename D, typename It,
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
class arrow<D, It, true, true> {
public:
    __UTL_HIDE_FROM_ABI inline constexpr auto operator->() const noexcept
        -> const_pointer_t<decltype(__UTL to_address(__UTL declval<It const&>()))> {
        return __UTL to_address(static_cast<D*>(this)->base());
    }
};

template <typename D, typename It>
class arrow<D, It, true, false> {
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
    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 D& operator--() noexcept(
        noexcept(--__UTL declval<It&>())) {
        --static_cast<D*>(this)->current;
        return static_cast<D*>(this);
    }

    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 D operator--(int) noexcept(
        noexcept(__UTL declval<It&>()--)) {
        return D(static_cast<D*>(this)->current--);
    }
};

template <typename D, typename It, bool = UTL_TRAIT_is_forward_iterator(It)>
class post_increment {
    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 void operator++(int) noexcept(
        noexcept(++__UTL declval<It&>())) {
        ++static_cast<D*>(this)->current;
    }
};

template <typename D, typename It>
class post_increment<D, It, true> {
    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 D operator++(int) noexcept(
        noexcept(__UTL declval<It&>()++)) {
        return D(static_cast<D*>(this)->current++);
    }
};

template <typename D, typename It, bool = UTL_TRAIT_is_random_access_iterator(It)>
class comparisons {};

template <typename D, typename It>
class comparisons<D, It, true> {
    __UTL_HIDE_FROM_ABI inline constexpr bool operator<(D const& other) const
        noexcept(UTL_TRAIT_is_nothrow_strict_subordinate_comparable(It)) {
        return static_cast<D const*>(this)->base() < other.base();
    }
    __UTL_HIDE_FROM_ABI inline constexpr bool operator<=(D const& other) const
        noexcept(UTL_TRAIT_is_nothrow_subordinate_comparable(It)) {
        return static_cast<D const*>(this)->base() <= other.base();
    }

    __UTL_HIDE_FROM_ABI inline constexpr bool operator>(D const& other) const
        noexcept(UTL_TRAIT_is_nothrow_strict_superordinate_comparable(It)) {
        return static_cast<D const*>(this)->base() > other.base();
    }
    __UTL_HIDE_FROM_ABI inline constexpr bool operator>=(D const& other) const
        noexcept(UTL_TRAIT_is_nothrow_superordinate_comparable(It)) {
        return static_cast<D const*>(this)->base() >= other.base();
    }
};
#endif

} // namespace const_iterator
} // namespace details

#if UTL_CXX20
template <typename I>
concept notconst_iterator = !is_base_of_v<details::const_iterator::specialization_tag<I>, I>;

template <typename I>
struct __UTL_PUBLIC_TEMPLATE is_notconst_iterator : bool_constant<notconst_iterator<I>> {};

template <typename I>
UTL_INLINE_CXX17 constexpr bool is_notconst_iterator_v = notconst_iterator<I>;

#  define UTL_TRAIT_is_notconst_iterator(...) __UTL notconst_iterator<__VA_ARGS__>

#else // UTL_CXX20

template <typename I>
struct __UTL_PUBLIC_TEMPLATE is_notconst_iterator :
    bool_constant<!UTL_TRAIT_is_base_of(details::const_iterator::specialization_tag<I>, I)> {};

#  if UTL_CXX14
template <typename I>
UTL_INLINE_CXX17 constexpr bool is_notconst_iterator_v =
    !is_base_of_v<details::const_iterator::specialization_tag<I>, I>;

#    define UTL_TRAIT_is_notconst_iterator(...) __UTL is_notconst_iterator_v<__VA_ARGS__>
#  else
#    define UTL_TRAIT_is_notconst_iterator(...) __UTL is_notconst_iterator<__VA_ARGS__>::value

#  endif // UTL_CXX20

#endif

template <typename It>
class __UTL_PUBLIC_TEMPLATE basicconst_iterator;

#if UTL_CXX20
template <input_iterator It>
class basicconst_iterator<It> :
    private details::const_iterator::specialization_tag<basicconst_iterator<It>>,
    public details::const_iterator::category<It> {
#else
template <typename It>
class basicconst_iterator :
    private details::const_iterator::specialization_tag<basicconst_iterator<It>>,
    public details::const_iterator::category<It>,
    public details::const_iterator::arrow<basicconst_iterator<It>, It>,
    public details::const_iterator::decrement<basicconst_iterator<It>, It>,
    public details::const_iterator::post_increment<basicconst_iterator<It>, It>,
    public details::const_iterator::comparisons<basicconst_iterator<It>, It> {
    static_assert(UTL_TRAIT_is_input_iterator(It), "Invalid argument");
    friend details::const_iterator::decrement<basicconst_iterator, It>;
    friend details::const_iterator::post_increment<basicconst_iterator, It>;
#endif
    using reference = iter_const_reference_t<It>;
    using rvalue_reference = details::const_iterator::const_rvalue_reference_t<It>;

public:
    using iterator_concept = details::const_iterator::concept_type_t<It>;
    using value_type = iter_value_t<It>;
    using difference_type = iter_difference_t<It>;

    __UTL_HIDE_FROM_ABI inline constexpr basicconst_iterator() noexcept(
        UTL_TRAIT_is_nothrow_default_constructible(It))
        UTL_CONSTRAINT_CXX20(default_initializable<It>)
    = default;

    __UTL_HIDE_FROM_ABI inline constexpr basicconst_iterator(It i) noexcept(
        UTL_TRAIT_is_nothrow_move_constructible(It))
        : current(__UTL move(i)) {}

    template <UTL_CONCEPT_CXX20(convertible_to<It>) Jt UTL_CONSTRAINT_CXX11(
        UTL_TRAIT_is_convertible(Jt, It))>
    __UTL_HIDE_FROM_ABI inline constexpr basicconst_iterator(
        basicconst_iterator<Jt> other) noexcept(UTL_TRAIT_is_nothrow_convertible(Jt, It))
        : current(other.base()) {}

    template <UTL_CONCEPT_CXX20(different_from<basicconst_iterator>) U UTL_CONSTRAINT_CXX11(
        !UTL_TRAIT_is_same(basicconst_iterator, U) && UTL_TRAIT_is_convertible(U, It))>
    UTL_CONSTRAINT_CXX20(convertible_to<U, It>)
    __UTL_HIDE_FROM_ABI inline constexpr basicconst_iterator(U&& other) noexcept(
        UTL_TRAIT_is_nothrow_convertible(U, It))
        : current(__UTL forward<U>(other)) {}

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline constexpr It const& base() const& noexcept {
        return current;
    }
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline UTL_CONSTEXPR_CXX14 It&& base() && noexcept {
        return __UTL move(current);
    }

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline constexpr reference operator*() const
        noexcept(UTL_TRAIT_is_nothrow_convertible(It const&, reference)) {
        return static_cast<reference>(*base());
    }

    template <UTL_CONCEPT_CXX20(sentinel_for<It>) S UTL_CONSTRAINT_CXX11(
        UTL_TRAIT_is_sentinel_for(It, S))>
    __UTL_HIDE_FROM_ABI inline constexpr bool operator==(S const& s) const {
        return base() == s;
    }

    template <UTL_CONCEPT_CXX20(sentinel_for<It>) S UTL_CONSTRAINT_CXX11(
        UTL_TRAIT_is_sentinel_for(It, S))>
    __UTL_HIDE_FROM_ABI inline constexpr bool operator!=(S const& s) const {
        return !(base() == s);
    }

    template <UTL_CONCEPT_CXX20(notconst_iterator) CI UTL_CONSTRAINT_CXX11(
        UTL_TRAIT_conjunction(is_notconst_iterator<CI>, is_convertible<It const&, CI>))>
    UTL_CONSTRAINT_CXX20(constant_iterator<CI> && convertible_to<It const&, CI>)
    __UTL_HIDE_FROM_ABI inline constexpr operator CI() const& noexcept(
        UTL_TRAIT_is_nothrow_convertible(It const&, CI)) {
        return base();
    }

    template <UTL_CONCEPT_CXX20(notconst_iterator) I UTL_CONSTRAINT_CXX11(UTL_TRAIT_conjunction(
        is_notconst_iterator<I>, is_random_access_iterator<It>, is_totally_ordered_with<It, I>))>
    UTL_CONSTRAINT_CXX20(random_access_iterator<It> && totally_ordered_with<It, I>)
    __UTL_HIDE_FROM_ABI friend inline constexpr bool
    operator<(I const& left, basicconst_iterator const& right) noexcept(
        UTL_TRAIT_is_nothrow_strict_subordinate_comparable_with(I, basicconst_iterator)) {
        return left < right.base();
    }

    template <UTL_CONCEPT_CXX20(notconst_iterator) I UTL_CONSTRAINT_CXX11(UTL_TRAIT_conjunction(
        is_notconst_iterator<I>, is_random_access_iterator<It>, is_totally_ordered_with<It, I>))>
    UTL_CONSTRAINT_CXX20(random_access_iterator<It> && totally_ordered_with<It, I>)
    __UTL_HIDE_FROM_ABI friend inline constexpr bool
    operator<=(I const& left, basicconst_iterator const& right) noexcept(
        UTL_TRAIT_is_nothrow_subordinate_comparable_with(I, basicconst_iterator)) {
        return left <= right.base();
    }

    template <UTL_CONCEPT_CXX20(notconst_iterator) I UTL_CONSTRAINT_CXX11(UTL_TRAIT_conjunction(
        is_notconst_iterator<I>, is_random_access_iterator<It>, is_totally_ordered_with<It, I>))>
    UTL_CONSTRAINT_CXX20(random_access_iterator<It> && totally_ordered_with<It, I>)
    __UTL_HIDE_FROM_ABI friend inline constexpr bool
    operator>(I const& left, basicconst_iterator const& right) noexcept(
        UTL_TRAIT_is_nothrow_strict_superordinate_comparable_with(I, basicconst_iterator)) {
        return left > right.base();
    }

    template <UTL_CONCEPT_CXX20(notconst_iterator) I UTL_CONSTRAINT_CXX11(UTL_TRAIT_conjunction(
        is_notconst_iterator<I>, is_random_access_iterator<It>, is_totally_ordered_with<It, I>))>
    UTL_CONSTRAINT_CXX20(random_access_iterator<It> && totally_ordered_with<It, I>)
    __UTL_HIDE_FROM_ABI friend inline constexpr bool
    operator>=(I const& left, basicconst_iterator const& right) noexcept(
        UTL_TRAIT_is_nothrow_superordinate_comparable_with(I, basicconst_iterator)) {
        return left >= right.base();
    }

    template <UTL_CONCEPT_CXX20(notconst_iterator) CI UTL_CONSTRAINT_CXX11(
        UTL_TRAIT_conjunction(is_notconst_iterator<CI>, is_convertible<It, CI>))>
    UTL_CONSTRAINT_CXX20(constant_iterator<CI> && convertible_to<It, CI>)
    __UTL_HIDE_FROM_ABI inline constexpr operator CI() && noexcept(
        UTL_TRAIT_is_nothrow_convertible(It, CI)) {
        return __UTL move(*this).base();
    }

    template <UTL_CONCEPT_CXX20(different_from<basicconst_iterator>) I UTL_CONSTRAINT_CXX11(
        !UTL_TRAIT_is_same(basicconst_iterator, I) && UTL_TRAIT_is_random_access_iterator(It) &&
        UTL_TRAIT_is_totally_ordered_with(It, I))>
    UTL_CONSTRAINT_CXX20(random_access_iterator<It> && totally_ordered_with<It, I>)
    __UTL_HIDE_FROM_ABI inline constexpr bool operator<(I const& i) const {
        return base() < i;
    }

    template <UTL_CONCEPT_CXX20(different_from<basicconst_iterator>) I UTL_CONSTRAINT_CXX11(
        !UTL_TRAIT_is_same(basicconst_iterator, I) && UTL_TRAIT_is_random_access_iterator(It) &&
        UTL_TRAIT_is_totally_ordered_with(It, I))>
    UTL_CONSTRAINT_CXX20(random_access_iterator<It> && totally_ordered_with<It, I>)
    __UTL_HIDE_FROM_ABI inline constexpr bool operator<=(I const& i) const {
        return base() <= i;
    }

    template <UTL_CONCEPT_CXX20(different_from<basicconst_iterator>) I UTL_CONSTRAINT_CXX11(
        !UTL_TRAIT_is_same(basicconst_iterator, I) && UTL_TRAIT_is_random_access_iterator(It) &&
        UTL_TRAIT_is_totally_ordered_with(It, I))>
    UTL_CONSTRAINT_CXX20(random_access_iterator<It> && totally_ordered_with<It, I>)
    __UTL_HIDE_FROM_ABI inline constexpr bool operator>(I const& i) const {
        return base() > i;
    }

    template <UTL_CONCEPT_CXX20(different_from<basicconst_iterator>) I UTL_CONSTRAINT_CXX11(
        !UTL_TRAIT_is_same(basicconst_iterator, I) && UTL_TRAIT_is_random_access_iterator(It) &&
        UTL_TRAIT_is_totally_ordered_with(It, I))>
    UTL_CONSTRAINT_CXX20(random_access_iterator<It> && totally_ordered_with<It, I>)
    __UTL_HIDE_FROM_ABI inline constexpr bool operator>=(I const& i) const {
        return base() >= i;
    }

    template <UTL_CONCEPT_CXX20(sized_sentinel_for<It>) S UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_sentinel_for(It, S))>
    __UTL_HIDE_FROM_ABI inline constexpr difference_type operator-(S const& s) const
        noexcept(noexcept(__UTL declval<It>() - s)) {
        return base() - s;
    }

    template <UTL_CONCEPT_CXX20(notconst_iterator) S UTL_CONSTRAINT_CXX11(
        UTL_TRAIT_conjunction(is_notconst_iterator<It>, is_sized_sentinel_for<It, S>))>
    __UTL_HIDE_FROM_ABI friend inline constexpr difference_type operator-(
        S const& left, basicconst_iterator const& right) noexcept(noexcept(left - right.base())) {
        return left - right.base();
    }

    __UTL_HIDE_FROM_ABI friend inline constexpr rvalue_reference
    iter_move(basicconst_iterator const& i) noexcept(
        noexcept(static_cast<rvalue_reference>(__UTL ranges::iter_move(i.base())))) {
        return static_cast<rvalue_reference>(__UTL ranges::iter_move(i.base()));
    }

#if UTL_CXX20
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline constexpr auto const* operator->() const
    requires (UTL_TRAIT_is_lvalue_reference(iter_reference_t<It>) &&
        same_as<remove_cvref_t<iter_reference_t<It>>, value_type> && contiguous_iterator<It>)
    {
        return __UTL to_address(base());
    }

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline constexpr const auto* operator->() const
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

    __UTL_HIDE_FROM_ABI inline constexpr basicconst_iterator& operator++() noexcept(
        noexcept(++current)) {
        ++current;
        return *this;
    }

    __UTL_HIDE_FROM_ABI inline constexpr void operator++(int) noexcept(noexcept(++current)) {
        ++current;
        return *this;
    }

    __UTL_HIDE_FROM_ABI inline constexpr basicconst_iterator operator++(int) noexcept(
        noexcept(current++))
    requires forward_iterator<It>
    {
        return basicconst_iterator(current++);
    }

    __UTL_HIDE_FROM_ABI inline constexpr basicconst_iterator& operator--() noexcept(
        noexcept(--current))
    requires bidirectional_iterator<It>
    {
        --current;
        return *this;
    }

    __UTL_HIDE_FROM_ABI inline constexpr basicconst_iterator operator--(int) noexcept(
        noexcept(current--))
    requires bidirectional_iterator<It>
    {
        return basicconst_iterator(current--);
    }

    __UTL_HIDE_FROM_ABI inline constexpr reference operator+=(difference_type n) const
        noexcept(noexcept(current += 1))
    requires random_access_iterator<It>
    {
        current += n;
        return *this;
    }

    __UTL_HIDE_FROM_ABI inline constexpr reference operator-=(difference_type n) const
        noexcept(noexcept(current -= 1))
    requires random_access_iterator<It>
    {
        current -= n;
        return *this;
    }

    __UTL_HIDE_FROM_ABI inline constexpr bool operator<(basicconst_iterator const& other) const
        noexcept(UTL_TRAIT_is_nothrow_strict_subordinate_comparable(It))
    requires random_access_iterator<It>
    {
        return base() < other.base();
    }
    __UTL_HIDE_FROM_ABI inline constexpr bool operator<=(basicconst_iterator const& other) const
        noexcept(UTL_TRAIT_is_nothrow_subordinate_comparable(It))
    requires random_access_iterator<It>
    {
        return base() <= other.base();
    }

    __UTL_HIDE_FROM_ABI inline constexpr bool operator>(basicconst_iterator const& other) const
        noexcept(UTL_TRAIT_is_nothrow_strict_superordinate_comparable(It))
    requires random_access_iterator<It>
    {
        return base() > other.base();
    }
    __UTL_HIDE_FROM_ABI inline constexpr bool operator>=(basicconst_iterator const& other) const
        noexcept(UTL_TRAIT_is_nothrow_superordinate_comparable(It))
    requires random_access_iterator<It>
    {
        return base() >= other.base();
    }

    template <typename U>
    requires convertible_to<U const&, basicconst_iterator const&> && random_access_iterator<It>
    __UTL_HIDE_FROM_ABI inline constexpr auto operator<=>(U const& other) const
        noexcept(UTL_TRAIT_is_nothrow_three_way_comparable(It)) {
        return base() <=> static_cast<basicconst_iterator const&>(other).base();
    }

    template <different_from<basicconst_iterator> I>
    requires random_access_iterator<It> && totally_ordered_with<It, I> &&
        three_way_comparable_with<It, I>
    __UTL_HIDE_FROM_ABI inline constexpr bool operator<=>(I const& i) const
        noexcept(UTL_TRAIT_is_nothrow_three_way_comparable_with(It, I)) {
        return base() <=> i;
    }

    __UTL_HIDE_FROM_ABI friend inline constexpr basicconst_iterator operator+(
        basicconst_iterator const& it,
        difference_type n) noexcept(noexcept(__UTL declval<It>() + n))
    requires random_access_iterator<It>
    {
        return basicconst_iterator(it.base() + n);
    }

    __UTL_HIDE_FROM_ABI friend inline constexpr basicconst_iterator operator+(difference_type n,
        basicconst_iterator const& it) noexcept(noexcept(__UTL declval<It>() + n))
    requires random_access_iterator<It>
    {
        return basicconst_iterator(it.base() + n);
    }

    __UTL_HIDE_FROM_ABI friend inline constexpr basicconst_iterator operator-(
        basicconst_iterator const& it,
        difference_type n) noexcept(noexcept(__UTL declval<It>() - n))
    requires random_access_iterator<It>
    {
        return basicconst_iterator(it.base() - n);
    }

#else
    /**
     * Arrow operator implemented in details::const_iterator::arrow
     */
    template <typename Idx,
        enable_if_t<UTL_TRAIT_conjunction(
                        is_random_access_iterator<It>, is_convertible<Idx, difference_type>),
            int> = 0>
    __UTL_HIDE_FROM_ABI inline constexpr reference operator[](Idx n) const
        noexcept(UTL_TRAIT_is_nothrow_convertible(It const&, reference)) {
        return static_cast<reference>(base()[static_cast<difference_type>(n)]);
    }

    __UTL_HIDE_FROM_ABI inline constexpr basicconst_iterator& operator++() noexcept(
        noexcept(++current)) {
        ++current;
        return *this;
    }

    template <typename Idx,
        enable_if_t<UTL_TRAIT_conjunction(
                        is_random_access_iterator<It>, is_convertible<Idx, difference_type>),
            int> = 0>
    __UTL_HIDE_FROM_ABI inline constexpr basicconst_iterator& operator+=(Idx n) const
        noexcept(noexcept(current += 1)) {
        current += static_cast<difference_type>(n);
        return *this;
    }

    template <typename Idx,
        enable_if_t<UTL_TRAIT_conjunction(
                        is_random_access_iterator<It>, is_convertible<Idx, difference_type>),
            int> = 0>
    __UTL_HIDE_FROM_ABI inline constexpr basicconst_iterator& operator-=(Idx n) const
        noexcept(noexcept(current -= 1)) {
        current -= static_cast<difference_type>(n);
        return *this;
    }

    template <typename Diff,
        enable_if_t<UTL_TRAIT_conjunction(
                        is_convertible<Diff, difference_type>, is_random_access_iterator<It>),
            int> = 0>
    __UTL_HIDE_FROM_ABI friend inline constexpr basicconst_iterator operator+(
        basicconst_iterator const& it,
        Diff n) noexcept(noexcept(__UTL declval<It>() + static_cast<difference_type>(n))) {
        return basicconst_iterator(it.base() + static_cast<difference_type>(n));
    }

    template <typename Diff,
        enable_if_t<UTL_TRAIT_conjunction(
                        is_convertible<Diff, difference_type>, is_random_access_iterator<It>),
            int> = 0>
    __UTL_HIDE_FROM_ABI friend inline constexpr basicconst_iterator
    operator+(Diff n, basicconst_iterator const& it) noexcept(
        noexcept(__UTL declval<It>() + static_cast<difference_type>(n))) {
        return basicconst_iterator(it.base() + static_cast<difference_type>(n));
    }

    template <typename Diff,
        enable_if_t<UTL_TRAIT_conjunction(
                        is_convertible<Diff, difference_type>, is_random_access_iterator<It>),
            int> = 0>
    __UTL_HIDE_FROM_ABI friend inline constexpr basicconst_iterator operator-(
        basicconst_iterator const& it,
        Diff n) noexcept(noexcept(__UTL declval<It>() - static_cast<difference_type>(n))) {
        return basicconst_iterator(it.base() - static_cast<difference_type>(n));
    }
#endif

private:
    It current;
};

template <typename...>
struct common_type;

#if UTL_CXX20

template <input_iterator It>
using const_iterator = conditional_t<constant_iterator<It>, It, basicconst_iterator<It>>;
template <semiregular It>
using const_sentinel = conditional_t<input_iterator<It>, const_iterator<It>, It>;

template <typename T, common_with<T> U>
requires input_iterator<typename common_type<T, U>::type>
struct common_type<basicconst_iterator<T>, U> {
    using type = basicconst_iterator<typename common_type<T, U>::type>;
};

template <typename T, common_with<T> U>
requires input_iterator<typename common_type<T, U>::type>
struct common_type<U, basicconst_iterator<T>> {
    using type = basicconst_iterator<typename common_type<T, U>::type>;
};

template <typename T, common_with<T> U>
requires input_iterator<typename common_type<T, U>::type>
struct common_type<basicconst_iterator<T>, basicconst_iterator<U>> {
    using type = basicconst_iterator<typename common_type<T, U>::type>;
};

#else // UTL_CXX20

namespace details {
namespace const_iterator {

template <typename T, typename U>
__UTL_HIDE_FROM_ABI auto common_type_impl(int) noexcept
    -> enable_if<UTL_TRAIT_is_input_iterator(typename __UTL common_type<T, U>::type),
        basicconst_iterator<typename __UTL common_type<T, U>::type>>;

struct common_type_fallback {};

template <typename T, typename U>
__UTL_HIDE_FROM_ABI auto common_type_impl(float) noexcept -> common_type_fallback;

template <typename T, typename U>
using common_type = decltype(common_type_impl<T, U>(0));

} // namespace const_iterator
} // namespace details

template <typename It>
using const_iterator = enable_if_t<UTL_TRAIT_is_input_iterator(It),
    conditional_t<UTL_TRAIT_is_constant_iterator(It), It, basicconst_iterator<It>>>;

template <typename It>
using const_sentinel =
    enable_if_t<UTL_TRAIT_conjunction(is_copyable<It>, is_default_constructible<It>),
        conditional_t<UTL_TRAIT_is_input_iterator(It), const_iterator<It>, It>>;

template <typename T, typename U>
struct common_type<basicconst_iterator<T>, U> : details::const_iterator::common_type<T, U> {};

template <typename T, typename U>
struct common_type<U, basicconst_iterator<T>> : details::const_iterator::common_type<T, U> {};

template <typename T, typename U>
struct common_type<basicconst_iterator<T>, basicconst_iterator<U>> :
    details::const_iterator::common_type<T, U> {};

#endif // UTL_CXX20

UTL_NAMESPACE_END
