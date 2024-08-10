#pragma once

#include "utl/preprocessor/utl_config.h"

#include "utl/tuple/utl_tuple_fwd.h"

#include "utl/tuple/utl_tuple_concepts.h"
#include "utl/tuple/utl_tuple_traits.h"
#include "utl/type_traits/utl_remove_reference.h"
#include "utl/utility/utl_customization_point.h"
#include "utl/utility/utl_declval.h"
#include "utl/utility/utl_forward.h"

UTL_NAMESPACE_BEGIN

namespace details {
namespace tuple {

template <size_t, typename T>
void get(T&&) = delete;

template <size_t I>
struct get_element_t {

    constexpr explicit get_element_t() noexcept = default;

    template <typename T UTL_REQUIRES_CXX11(is_tuple_like<remove_reference_t<T>>::value)>
    UTL_REQUIRES_CXX20(tuple_like<remove_reference_t<T>> && requires(T&& t) { get<I>(UTL_SCOPE forward<T>(t)); })
    UTL_ATTRIBUTES(HIDE_FROM_ABI, ALWAYS_INLINE) inline constexpr auto operator()(T&& t) const
        noexcept(noexcept(get<I>(UTL_SCOPE declval<T>())))
            -> decltype(get<I>(UTL_SCOPE declval<T>())) {
        return get<I>(UTL_SCOPE forward<T>(t));
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

template <size_t I, typename T>
UTL_ATTRIBUTES(HIDE_FROM_ABI, ALWAYS_INLINE) inline constexpr auto get_element(T&& t) noexcept(
    noexcept(details::tuple::get_element_t<I>{}(UTL_SCOPE declval<T>())))
    -> decltype(details::tuple::get_element_t<I>{}(UTL_SCOPE declval<T>())) {
    UTL_SCOPE details::customization_point::constant<details::tuple::get_element_t<I>>::value(
        UTL_SCOPE forward<T>(t));
}

#endif
} // namespace cpo

UTL_NAMESPACE_END
