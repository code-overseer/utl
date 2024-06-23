// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/iterator/utl_iter_difference_t.h"
#include "utl/iterator/utl_sentinel_for.h"
#include "utl/preprocessor/utl_config.h"
#include "utl/type_traits/utl_constants.h"
#include "utl/type_traits/utl_remove_cv.h"

#if UTL_CXX20
#  include "utl/concepts/utl_same_as.h"

UTL_NAMESPACE_BEGIN

template <typename S, typename I>
inline constexpr bool disable_sized_sentinel_for = false;

template <typename S, typename I>
concept sized_sentinel_for = UTL_SCOPE sentinel_for<S, I> &&
    !UTL_SCOPE disable_sized_sentinel_for<UTL_SCOPE remove_cv_t<S>, UTL_SCOPE remove_cv_t<I>> &&
    requires(I const& i, S const& s) {
        { s - i } -> UTL_SCOPE same_as<UTL_SCOPE iter_difference_t<I>>;
        { i - s } -> UTL_SCOPE same_as<UTL_SCOPE iter_difference_t<I>>;
    };

template <typename S, typename I>
struct is_sized_sentinel_for : UTL_SCOPE bool_constant<sized_sentinel_for<S, I>> {};

template <typename S, typename I>
inline constexpr bool is_sized_sentinel_for_v = sized_sentinel_for<S, I>;

UTL_NAMESPACE_END

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
struct is_disabled : UTL_SCOPE bool_constant<disable_sized_sentinel_for<S, I>> {};
} // namespace sized_sentinel_for
} // namespace details

#  else

namespace details {
namespace sized_sentinel_for {
template <typename S, typename I>
struct is_disabled : UTL_SCOPE false_type {};
} // namespace sized_sentinel_for
} // namespace details

#  endif

namespace details {
namespace sized_sentinel_for {

template <typename S, typename I>
auto subtractible(int) noexcept -> UTL_SCOPE conjunction<
    UTL_SCOPE is_same<decltype(UTL_SCOPE declval<S const&>() - UTL_SCOPE declval<I const&>()),
        UTL_SCOPE iter_difference_t<I>>,
    UTL_SCOPE is_same<decltype(UTL_SCOPE declval<I const&>() - UTL_SCOPE declval<S const&>()),
        UTL_SCOPE iter_difference_t<I>>>;

template <typename S, typename I>
auto subtractible(float) noexcept -> UTL_SCOPE false_type;

template <typename S, typename I>
using is_subtractible = decltype(UTL_SCOPE details::sized_sentinel_for::subtractible<S, I>(0));
} // namespace sized_sentinel_for
} // namespace details

template <typename S, typename I>
struct is_sized_sentinel_for :
    UTL_SCOPE conjunction<UTL_SCOPE is_sentinel_for<S, I>,
        UTL_SCOPE details::sized_sentinel_for::is_disabled<S, I>,
        UTL_SCOPE details::sized_sentinel_for::is_subtractible<S, I>> {};

#  if UTL_CXX14
template <typename S, typename I>
UTL_INLINE_CXX17 constexpr bool is_sized_sentinel_for_v = is_sized_sentinel_for<S, I>::value;
#  endif

UTL_NAMESPACE_END
#endif // UTL_CXX20
