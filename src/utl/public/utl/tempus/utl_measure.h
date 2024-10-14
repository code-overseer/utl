// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

#include "utl/tuple/utl_tuple_fwd.h"

#include "utl/tempus/utl_clock.h"
#include "utl/tuple/utl_tuple_get_element.h"
#include "utl/utility/utl_sequence.h"

UTL_NAMESPACE_BEGIN

namespace tempus {
template <UTL_CONCEPT_CXX20(invocable) F, UTL_CONCEPT_CXX20(clock_type) Clock
UTL_CONSTRAINT_CXX11(is_tempus_clock(Clock))>
inline auto measure(F&& f, Clock clock) noexcept(noexcept(__UTL invoke(f))) ->
    typename clock_traits<Clock>::duration {
    static_assert(is_clock<Clock>::value, "Invalid arguments");
    auto const begin = get_time(clock);
    return __UTL invoke(f), (get_time(clock) - begin);
}

namespace details {
template <typename T, size_t... Is>
auto difference(T const& l, T const& r, __UTL index_sequence<Is...>) noexcept
    -> __UTL tuple<typename clock_traits<tuple_element_t<Is, T>>::duration...> {
    static_assert(tuple_size<T>::value == sizeof...(Is), "Invalid arguments");
    return __UTL tuple<typename clock_traits<tuple_element_t<Is, T>>::duration...>{
        (__UTL get_element<Is>(l) - __UTL get_element<Is>(r))...};
}
} // namespace details

template <UTL_CONCEPT_CXX20(invocable) F, UTL_CONCEPT_CXX20(clock_type) C0,
    UTL_CONCEPT_CXX20(clock_type)... Cs UTL_CONSTRAINT_CXX11(
        UTL_TRAIT_conjunction(is_clock<C0>, is_clock<Cs>...))>
inline auto measure(F&& f, C0 c0, Cs... cs) noexcept(noexcept(__UTL invoke(f)))
    -> __UTL tuple<typename clock_traits<C0>::duration, typename clock_traits<Cs>::duration...> {
    static_assert(is_clock<C0>::value && (... && is_clock<Cs>::value), "Invalid arguments");
    using time_points = __UTL tuple<time_point<C0>, time_point<Cs>...>;
    static constexpr make_index_sequence<sizeof...(Cs)> sequence{};

    time_points const begin{get_time(c0), get_time(cs)...};
    return __UTL invoke(f),
           details::difference(time_points{get_time(c0), get_time(cs)...}, begin, sequence);
}
} // namespace tempus

UTL_NAMESPACE_END
