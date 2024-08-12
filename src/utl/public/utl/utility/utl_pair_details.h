// Copyright 2023-2024 Bryan Wong

#include "utl/preprocessor/utl_config.h"

#include "utl/utility/utl_pair_fwd.h"

#include "utl/type_traits/utl_decay.h"
#include "utl/type_traits/utl_enable_if.h"
#include "utl/type_traits/utl_unwrap_reference.h"
#include "utl/utility/utl_declval.h"
#include "utl/utility/utl_forward.h"
#include "utl/utility/utl_move.h"

UTL_STD_NAMESPACE_BEGIN
template <typename T0, typename T1>
struct __UTL_PUBLIC_TEMPLATE tuple_size<UTL_SCOPE pair<T0, T1>> :
    UTL_SCOPE template_size<UTL_SCOPE pair<T0, T1>> {};

template <size_t I, typename T0, typename T1>
struct __UTL_PUBLIC_TEMPLATE tuple_element<I, UTL_SCOPE pair<T0, T1>> :
    UTL_SCOPE template_element<I, UTL_SCOPE pair<T0, T1>> {};
UTL_STD_NAMESPACE_END

UTL_NAMESPACE_BEGIN

template <typename T>
struct __UTL_PUBLIC_TEMPLATE is_pair : false_type {};
template <typename T>
struct __UTL_PUBLIC_TEMPLATE is_pair<T&&> : is_pair<T> {};
template <typename T>
struct __UTL_PUBLIC_TEMPLATE is_pair<T&> : is_pair<T> {};
template <typename T>
struct __UTL_PUBLIC_TEMPLATE is_pair<T const> : is_pair<T> {};
template <typename T>
struct __UTL_PUBLIC_TEMPLATE is_pair<T volatile> : is_pair<T> {};
template <typename T>
struct __UTL_PUBLIC_TEMPLATE is_pair<T const volatile> : is_pair<T> {};
template <typename T, typename U>
struct __UTL_PUBLIC_TEMPLATE is_pair<pair<T, U>> : true_type {};

struct __UTL_ABI_PUBLIC piecewise_construct_t {
    __UTL_HIDE_FROM_ABI explicit constexpr piecewise_construct_t() noexcept = default;
};

UTL_INLINE_CXX17 constexpr piecewise_construct_t piecewise_construct{};

template <typename T, typename U>
UTL_ATTRIBUTES(NODISCARD, _HIDE_FROM_ABI, ALWAYS_INLINE) constexpr pair<unwrap_reference_t<decay_t<T>>, unwrap_reference_t<decay_t<U>>>
make_pair(T&& t, U&& u) {
    return pair<unwrap_reference_t<decay_t<T>>, unwrap_reference_t<decay_t<U>>>{
        UTL_SCOPE forward<T>(t), UTL_SCOPE forward<U>(u)};
}

namespace details {
namespace pair {
template <size_t I, typename P> UTL_REQUIRES_CXX20(I == 0 && requires(P&& p) {
        requires is_pair<P>::value;
        UTL_SCOPE forward<P>(p).first;
    })
UTL_ATTRIBUTES(NODISCARD, CONST, _HIDE_FROM_ABI) constexpr auto get(P&& p UTL_LIFETIMEBOUND) noexcept
    -> UTL_ENABLE_IF_CXX11(decltype(UTL_SCOPE declval<P>().first), (I == 0 && UTL_SCOPE is_pair<P>::value)) {
    return UTL_SCOPE forward<P>(p).first;
}

template <size_t I, typename P> UTL_REQUIRES_CXX20(I == 1 && requires(P&& p) {
        requires is_pair<P>::value;
        UTL_SCOPE forward<P>(p).second;
    })
UTL_ATTRIBUTES(NODISCARD, CONST, _HIDE_FROM_ABI) constexpr auto get(P&& p UTL_LIFETIMEBOUND) noexcept
    -> UTL_ENABLE_IF_CXX11(decltype(UTL_SCOPE declval<P>().second), (I == 0 && UTL_SCOPE is_pair<P>::value)) {
    return UTL_SCOPE forward<P>(p).second;
}
} // namespace pair
} // namespace details

template <size_t I, typename T0, typename T1>
UTL_ATTRIBUTES(NODISCARD, CONST, _HIDE_FROM_ABI, FLATTEN) constexpr auto get(pair<T0, T1>&& pair UTL_LIFETIMEBOUND) noexcept -> decltype(details::pair::get<I>(UTL_SCOPE move(pair))) {
    return details::pair::get<I>(UTL_SCOPE move(pair));
}

template <size_t I, typename T0, typename T1>
UTL_ATTRIBUTES(NODISCARD, CONST, _HIDE_FROM_ABI, FLATTEN) constexpr auto get(pair<T0, T1> const&& pair UTL_LIFETIMEBOUND) noexcept -> decltype(details::pair::get<I>(UTL_SCOPE move(pair))) {
    return details::pair::get<I>(UTL_SCOPE move(pair));
}

template <size_t I, typename T0, typename T1>
UTL_ATTRIBUTES(NODISCARD, CONST, _HIDE_FROM_ABI, FLATTEN) constexpr auto get(pair<T0, T1> const& pair UTL_LIFETIMEBOUND) noexcept -> decltype(details::pair::get<I>(pair)) {
    return details::pair::get<I>(pair);
}

template <size_t I, typename T0, typename T1>
UTL_ATTRIBUTES(NODISCARD, CONST, _HIDE_FROM_ABI, FLATTEN) constexpr auto get(
    pair<T0, T1>& pair UTL_LIFETIMEBOUND) noexcept -> decltype(details::pair::get<I>(pair)) {
    return details::pair::get<I>(pair);
}

template <typename T0, typename T1>
UTL_ATTRIBUTES(NODISCARD, CONST, _HIDE_FROM_ABI, ALWAYS_INLINE) constexpr T0&& get(
    pair<T0, T1>&& pair UTL_LIFETIMEBOUND) noexcept {
    return UTL_SCOPE move(pair).first;
}

template <typename T0, typename T1>
UTL_ATTRIBUTES(NODISCARD, CONST, _HIDE_FROM_ABI, ALWAYS_INLINE) constexpr T0 const&& get(
    pair<T0, T1> const&& pair UTL_LIFETIMEBOUND) noexcept {
    return UTL_SCOPE move(pair).first;
}

template <typename T0, typename T1>
UTL_ATTRIBUTES(NODISCARD, CONST, _HIDE_FROM_ABI, ALWAYS_INLINE) constexpr T0 const& get(
    pair<T0, T1> const& pair UTL_LIFETIMEBOUND) noexcept {
    return pair.first;
}

template <typename T0, typename T1>
UTL_ATTRIBUTES(NODISCARD, CONST, _HIDE_FROM_ABI, ALWAYS_INLINE) constexpr T0& get(
    pair<T0, T1>& pair UTL_LIFETIMEBOUND) noexcept {
    return pair.first;
}

template <typename T1, typename T0>
UTL_ATTRIBUTES(NODISCARD, CONST, _HIDE_FROM_ABI, ALWAYS_INLINE) constexpr T1&& get(
    pair<T0, T1>&& pair UTL_LIFETIMEBOUND) noexcept {
    return UTL_SCOPE move(pair).second;
}

template <typename T1, typename T0>
UTL_ATTRIBUTES(NODISCARD, CONST, _HIDE_FROM_ABI, ALWAYS_INLINE) constexpr T1 const&& get(
    pair<T0, T1> const&& pair UTL_LIFETIMEBOUND) noexcept {
    return UTL_SCOPE move(pair).second;
}

template <typename T1, typename T0>
UTL_ATTRIBUTES(NODISCARD, CONST, _HIDE_FROM_ABI, ALWAYS_INLINE) constexpr T1 const& get(
    pair<T0, T1> const& pair UTL_LIFETIMEBOUND) noexcept {
    return pair.second;
}

template <typename T1, typename T0>
UTL_ATTRIBUTES(NODISCARD, CONST, _HIDE_FROM_ABI, ALWAYS_INLINE) constexpr T1& get(
    pair<T0, T1>& pair UTL_LIFETIMEBOUND) noexcept {
    return pair.second;
}

namespace details {
namespace pair {

template <typename, typename>
struct invalid_swap_t {
    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 void swap(invalid_swap_t& other) const noexcept {}
    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 void swap(invalid_swap_t const& other) const noexcept {}
};

} // namespace pair
} // namespace details

UTL_NAMESPACE_END
