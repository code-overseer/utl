#pragma once

#include "utl/utl_config.h"

#include "utl/tuple/utl_tuple_fwd.h"

#include "utl/type_traits/utl_remove_cvref.h"
#include "utl/type_traits/utl_remove_reference.h"
#include "utl/utility/utl_customization_point.h"
#include "utl/utility/utl_declval.h"
#include "utl/utility/utl_forward.h"

UTL_NAMESPACE_BEGIN

namespace details {
namespace tuple {

template <size_t, typename T>
void get(T&&) = delete;

#if UTL_CXX20
template <typename T, size_t I>
concept has_adl_get UTL_NODEBUG = requires(T&& t) { get<I>(__UTL forward<T>(t)); };
template <typename T, size_t I>
concept has_member_get UTL_NODEBUG = requires(T&& t) { __UTL forward<T>(t).template get<I>(); };
#else
template <typename T, size_t I>
__UTL_HIDE_FROM_ABI auto has_adl_get_impl(float) noexcept -> __UTL false_type;
template <typename T, size_t I>
__UTL_HIDE_FROM_ABI auto has_adl_get_impl(int) noexcept
    -> __UTL always_true_type<decltype(get<I>(__UTL declval<T>()))>;
template <typename T, size_t I>
using has_adl_get UTL_NODEBUG = decltype(has_adl_get_impl<T, I>(0));
#endif

template <size_t I>
struct get_element_t {
private:
    template <typename T>
    __UTL_HIDE_FROM_ABI static auto nothrow_member_get_impl(float) noexcept -> __UTL false_type;
    template <typename T>
    __UTL_HIDE_FROM_ABI static auto nothrow_member_get_impl(int) noexcept
        -> __UTL bool_constant<noexcept(__UTL declval<T>().template get<I>())>;
    template <typename T>
    using nothrow_member_get UTL_NODEBUG = decltype(nothrow_member_get_impl<T>(0));
    template <typename T>
    __UTL_HIDE_FROM_ABI static auto nothrow_adl_get_impl(float) noexcept -> __UTL false_type;
    template <typename T>
    __UTL_HIDE_FROM_ABI static auto nothrow_adl_get_impl(int) noexcept
        -> __UTL bool_constant<noexcept(get<I>(__UTL declval<T>()))>;
    template <typename T>
    using nothrow_adl_get UTL_NODEBUG = decltype(nothrow_adl_get_impl<T>(0));

public:
    constexpr explicit get_element_t() noexcept = default;

    template <UTL_CONCEPT_CXX20(has_adl_get<I>) T>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline constexpr auto operator()(
        T&& t UTL_LIFETIMEBOUND) const noexcept(nothrow_adl_get<T>::value)
        -> decltype(get<I>(__UTL declval<T>())) {
        return get<I>(__UTL forward<T>(t));
    }

    template <UTL_CONCEPT_CXX20(has_member_get<I>) T UTL_CONSTRAINT_CXX11(
        !has_adl_get<T, I>::value)>
    UTL_CONSTRAINT_CXX20(!has_adl_get<T, I>)
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline constexpr auto operator()(
        T&& t UTL_LIFETIMEBOUND) const noexcept(nothrow_member_get<T>::value)
        -> decltype(__UTL declval<T>().template get<I>()) {
        return __UTL forward<T>(t).template get<I>();
    }
};

} // namespace tuple
} // namespace details

inline namespace cpo {

#if UTL_CXX17
template <size_t I>
inline constexpr details::tuple::get_element_t<I> get_element{};
#elif UTL_CXX14

namespace {
template <size_t I>
constexpr auto const& get_element =
    __UTL details::customization_point::constant<details::tuple::get_element_t<I>>;
}

#else

/* Unfortunately multi-arity CPOs are not supported in C++11 */
template <size_t I, typename T>
UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline constexpr auto get_element(T&& t) noexcept(
    noexcept(details::tuple::get_element_t<I>{}(__UTL declval<T>())))
    -> decltype(details::tuple::get_element_t<I>{}(__UTL declval<T>())) {
    __UTL details::customization_point::constant<details::tuple::get_element_t<I>>::value(
        __UTL forward<T>(t));
}

#endif

UTL_DEFINE_CUSTOMIZATION_POINT(details::tuple::get_element_t<0>, get_key);
UTL_DEFINE_CUSTOMIZATION_POINT(details::tuple::get_element_t<1>, get_value);
} // namespace cpo

namespace details {
namespace tuple {

template <size_t I, typename T>
__UTL_HIDE_FROM_ABI auto nothrow_impl(int)
    -> bool_constant<noexcept(__UTL get_element<I>(__UTL declval<T>()))>;
template <size_t I, typename>
__UTL_HIDE_FROM_ABI auto nothrow_impl(float) -> __UTL false_type;

template <size_t I, typename T>
__UTL_HIDE_FROM_ABI auto gettable_impl(float) -> __UTL false_type;
template <size_t I, typename T>
__UTL_HIDE_FROM_ABI auto gettable_impl(int)
    -> __UTL always_true_type<decltype(__UTL get_element<I>(__UTL declval<T>()))>;

template <size_t I, typename T>
using is_nothrow_gettable UTL_NODEBUG = decltype(__UTL details::tuple::nothrow_impl<I, T>(0));
template <size_t I, typename T>
using is_gettable UTL_NODEBUG = decltype(__UTL details::tuple::gettable_impl<I, T>(0));

template <typename TupleLike, size_t N = tuple_size<remove_cvref_t<TupleLike>>::value>
struct is_all_gettable :
    conjunction<is_gettable<N - 1, TupleLike>, is_all_gettable<TupleLike, N - 1>> {};

template <typename TupleLike>
struct is_all_gettable<TupleLike, 0> : is_gettable<0, TupleLike> {};

template <typename TupleLike, size_t N = tuple_size<remove_cvref_t<TupleLike>>::value>
struct is_all_nothrow_gettable :
    conjunction<is_nothrow_gettable<N - 1, TupleLike>, is_all_nothrow_gettable<TupleLike, N - 1>> {
};
template <typename TupleLike>
struct is_all_nothrow_gettable<TupleLike, 0> : is_nothrow_gettable<0, TupleLike> {};

} // namespace tuple
} // namespace details

UTL_NAMESPACE_END
