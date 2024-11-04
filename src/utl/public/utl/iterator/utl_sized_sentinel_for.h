// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

#include "utl/iterator/utl_iter_difference_t.h"
#include "utl/iterator/utl_sentinel_for.h"
#include "utl/type_traits/utl_constants.h"
#include "utl/type_traits/utl_remove_cv.h"

#if UTL_CXX20
#  include "utl/concepts/utl_same_as.h"

UTL_NAMESPACE_BEGIN

template <typename S, typename I>
inline constexpr bool disable_sized_sentinel_for = false;

template <typename S, typename I>
concept sized_sentinel_for =
    sentinel_for<S, I> && !disable_sized_sentinel_for<remove_cv_t<S>, remove_cv_t<I>> &&
    requires(I const& i, S const& s) {
        { s - i } -> same_as<iter_difference_t<I>>;
        { i - s } -> same_as<iter_difference_t<I>>;
    };

template <typename S, typename I>
struct __UTL_PUBLIC_TEMPLATE is_sized_sentinel_for : bool_constant<sized_sentinel_for<S, I>> {};

template <typename S, typename I>
inline constexpr bool is_sized_sentinel_for_v = sized_sentinel_for<S, I>;

UTL_NAMESPACE_END

#  define UTL_TRAIT_is_sized_sentinel_for(...) __UTL sized_sentinel_for<__VA_ARGS__>

#else // UTL_CXX20

#  include "utl/type_traits/utl_declval.h"
#  include "utl/type_traits/utl_is_same.h"
#  include "utl/type_traits/utl_logical_traits.h"

UTL_NAMESPACE_BEGIN

#  if UTL_CXX14
template <typename S, typename I>
UTL_INLINE_CXX17 constexpr bool disable_sized_sentinel_for = false;

namespace details {
namespace sized_sentinel_for {
template <typename S, typename I>
struct is_disabled : __UTL bool_constant<disable_sized_sentinel_for<S, I>> {};
} // namespace sized_sentinel_for
} // namespace details

#  else

namespace details {
namespace sized_sentinel_for {
template <typename S, typename I>
struct is_disabled : __UTL false_type {};
} // namespace sized_sentinel_for
} // namespace details

#  endif

namespace details {
namespace sized_sentinel_for {

template <typename S, typename I>
__UTL_HIDE_FROM_ABI auto subtractible(int) noexcept -> __UTL
    conjunction<__UTL is_same<decltype(__UTL declval<S const&>() - __UTL declval<I const&>()),
                    __UTL iter_difference_t<I>>,
        __UTL is_same<decltype(__UTL declval<I const&>() - __UTL declval<S const&>()),
            __UTL iter_difference_t<I>>>;

template <typename S, typename I>
__UTL_HIDE_FROM_ABI auto subtractible(float) noexcept -> __UTL false_type;

template <typename S, typename I>
using is_subtractible UTL_NODEBUG =
    decltype(__UTL details::sized_sentinel_for::subtractible<S, I>(0));
} // namespace sized_sentinel_for
} // namespace details

template <typename S, typename I>
struct __UTL_PUBLIC_TEMPLATE is_sized_sentinel_for :
    conjunction<is_sentinel_for<S, I>,
        details::sized_sentinel_for::is_disabled<remove_cv_t<S>, remove_cv_t<I>>,
        details::sized_sentinel_for::is_subtractible<S, I>> {};

#  if UTL_CXX14
template <typename S, typename I>
UTL_INLINE_CXX17 constexpr bool is_sized_sentinel_for_v = is_sized_sentinel_for<S, I>::value;
#  endif

UTL_NAMESPACE_END

#  define UTL_TRAIT_is_sized_sentinel_for(...) __UTL is_sized_sentinel_for<__VA_ARGS__>::value

#endif // UTL_CXX20
