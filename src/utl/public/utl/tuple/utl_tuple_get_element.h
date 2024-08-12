#pragma once

#include "utl/preprocessor/utl_config.h"

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
concept has_adl_get UTL_NODEBUG = requires(T&& t) { get<I>(UTL_SCOPE forward<T>(t)); };
template <typename T, size_t I>
concept has_member_get UTL_NODEBUG = requires(T&& t) { UTL_SCOPE forward<T>(t).template get<I>(); };
#else
template <typename T, size_t I>
__UTL_HIDE_FROM_ABI auto has_adl_get_impl(float) noexcept -> UTL_SCOPE false_type;
template <typename T, size_t I>
__UTL_HIDE_FROM_ABI auto has_adl_get_impl(int) noexcept
    -> UTL_SCOPE always_true_type<decltype(get<I>(UTL_SCOPE declval<T>()))>;
template <typename T, size_t I>
using has_adl_get UTL_NODEBUG = decltype(has_adl_get_impl<T, I>(0));
#endif

template <size_t I>
struct get_element_t {
private:
    template <typename T>
    __UTL_HIDE_FROM_ABI static auto nothrow_member_get_impl(float) noexcept -> UTL_SCOPE false_type;
    template <typename T>
    __UTL_HIDE_FROM_ABI static auto nothrow_member_get_impl(int) noexcept
        -> UTL_SCOPE bool_constant<noexcept(UTL_SCOPE declval<T>().template get<I>())>;
    template <typename T>
    using nothrow_member_get UTL_NODEBUG = decltype(nothrow_member_get_impl<T>(0));
    template <typename T>
    __UTL_HIDE_FROM_ABI static auto nothrow_adl_get_impl(float) noexcept -> UTL_SCOPE false_type;
    template <typename T>
    __UTL_HIDE_FROM_ABI static auto nothrow_adl_get_impl(int) noexcept
        -> UTL_SCOPE bool_constant<noexcept(get<I>(UTL_SCOPE declval<T>()))>;
    template <typename T>
    using nothrow_adl_get UTL_NODEBUG = decltype(nothrow_adl_get_impl<T>(0));

public:
    constexpr explicit get_element_t() noexcept = default;

    template <UTL_CONCEPT_CXX20(has_adl_get<I>) T>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline constexpr auto operator()(
        T&& t UTL_LIFETIMEBOUND) const noexcept(nothrow_adl_get<T>::value)
        -> decltype(get<I>(UTL_SCOPE declval<T>())) {
        return get<I>(UTL_SCOPE forward<T>(t));
    }

    template <UTL_CONCEPT_CXX20(has_member_get<I>) T UTL_REQUIRES_CXX11(!has_adl_get<T, I>::value)>
    UTL_REQUIRES_CXX20(!has_adl_get<T, I>)
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline constexpr auto operator()(
        T&& t UTL_LIFETIMEBOUND) const noexcept(nothrow_member_get<T>::value)
        -> decltype(UTL_SCOPE declval<T>().template get<I>()) {
        return UTL_SCOPE forward<T>(t).template get<I>();
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
    UTL_SCOPE details::customization_point::constant<details::tuple::get_element_t<I>>;
}

#else

/* Unfortunately multi-arity CPOs are not supported in C++11 */
template <size_t I, typename T>
UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline constexpr auto get_element(T&& t) noexcept(
    noexcept(details::tuple::get_element_t<I>{}(UTL_SCOPE declval<T>())))
    -> decltype(details::tuple::get_element_t<I>{}(UTL_SCOPE declval<T>())) {
    UTL_SCOPE details::customization_point::constant<details::tuple::get_element_t<I>>::value(
        UTL_SCOPE forward<T>(t));
}

#endif

UTL_DEFINE_CUSTOMIZATION_POINT(details::tuple::get_element_t<0>, get_key);
UTL_DEFINE_CUSTOMIZATION_POINT(details::tuple::get_element_t<1>, get_value);
} // namespace cpo

namespace details {
namespace tuple {

template <size_t I, typename T>
__UTL_HIDE_FROM_ABI auto nothrow_impl(int)
    -> bool_constant<noexcept(UTL_SCOPE get_element<I>(UTL_SCOPE declval<T>()))>;
template <size_t I, typename>
__UTL_HIDE_FROM_ABI auto nothrow_impl(float) -> UTL_SCOPE false_type;

template <size_t I, typename T>
__UTL_HIDE_FROM_ABI auto gettable_impl(float) -> UTL_SCOPE false_type;
template <size_t I, typename T>
__UTL_HIDE_FROM_ABI auto gettable_impl(int)
    -> UTL_SCOPE always_true_type<decltype(UTL_SCOPE get_element<I>(UTL_SCOPE declval<T>()))>;

template <size_t I, typename T>
using is_nothrow_gettable UTL_NODEBUG = decltype(UTL_SCOPE details::tuple::nothrow_impl<I, T>(0));
template <size_t I, typename T>
using is_gettable UTL_NODEBUG = decltype(UTL_SCOPE details::tuple::gettable_impl<I, T>(0));

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
