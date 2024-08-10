#pragma once

#include "utl/preprocessor/utl_config.h"

#include "utl/tuple/utl_tuple_fwd.h"

#include "utl/type_traits/utl_copy_cvref.h"
#include "utl/utility/utl_sequence.h"

UTL_NAMESPACE_BEGIN

namespace details {
namespace tuple {
template <size_t I, typename T UTL_REQUIRES_CXX11((I < tuple_size_v<T>))>
UTL_REQUIRES_CXX20(requires {
    typename tuple_element_t<I, T>;
})
auto decl_element() noexcept -> copy_cvref_t<T&&, tuple_element_t<I, T>>;

template <size_t I, typename T UTL_REQUIRES_CXX11((I >= tuple_size_v<T>))>
void decl_element() noexcept = delete;

template <template <typename...> class Target, typename T, size_t... Is>
UTL_HIDE_FROM_ABI Target<decltype(decl_element<Is, T>())...> rebind_references_impl(
    T&&, UTL_SCOPE index_sequence<Is...>) noexcept;

template <template <typename...> class Target, typename T, size_t... Is>
UTL_HIDE_FROM_ABI Target<UTL_SCOPE tuple_element_t<Is, T>...> rebind_elements_impl(
    T&&, UTL_SCOPE index_sequence<Is...>) noexcept;

/**
 * Rebinds the result of `get` for a tuple-like type onto a variadic template type
 *
 * @tparam Target - the target variadic type that will contain the refereces
 * @tparam TupleLike - any tuple-like type or reference, may be cv-qualified
 * @tparam N - number of elements to consider, defaults to tuple_size<TupleLike>::value
 *
 * @return `Target<Ref...>`
 *  where:
 *    `Ref` is `decltype(get<I>(declval<TupleLike>()))` or
 * `decltype(declval<TupleLike>().get<I>())` where I is in the interval [0, N)
 */
template <template <typename...> class Target, typename TupleLike,
    size_t N = tuple_size<TupleLike>::value>
using rebind_references_t UTL_NODEBUG = decltype(rebind_references_impl<Target>(
    UTL_SCOPE declval<TupleLike>(), UTL_SCOPE make_index_sequence<N>{}));

template <template <typename...> class Target, typename TupleLike,
    size_t N = tuple_size<TupleLike>::value>
struct rebind_references {
    using type UTL_NODEBUG = rebind_references_t<Target, TupleLike, N>;
};

/**
 * Rebinds the result of `tuple_element` for a tuple-like type onto a variadic template type
 *
 * @tparam Target - the target variadic type that will contain the refereces
 * @tparam TupleLike - any tuple-like type or reference, may be cv-qualified
 * @tparam N - number of elements to consider, defaults to tuple_size<TupleLike>::value
 *
 * @return `Target<Ref...>`
 *  where:
 *    `Ref` is `decltype(get<I>(declval<TupleLike>()))` or
 * `decltype(declval<TupleLike>().get<I>())` where I is in the interval [0, N)
 */
template <template <typename...> class Target, typename TupleLike,
    size_t N = tuple_size<TupleLike>::value>
using rebind_elements_t UTL_NODEBUG = decltype(rebind_elements_impl<Target>(
    UTL_SCOPE declval<TupleLike>(), UTL_SCOPE make_index_sequence<N>{}));

template <template <typename...> class Target, typename TupleLike,
    size_t N = tuple_size<TupleLike>::value>
struct rebind_elements {
    using type UTL_NODEBUG = rebind_elements_t<Target, TupleLike, N>;
};

} // namespace tuple
} // namespace details

UTL_NAMESPACE_END
