// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

#include "utl/iterator/utl_legacy_forward_iterator.h"
#include "utl/type_traits/utl_constants.h"

#if UTL_CXX20

#  include "utl/concepts/utl_lvalue_reference.h"
#  include "utl/iterator/utl_bidirectional_iterator.h"

UTL_NAMESPACE_BEGIN

template <typename It>
concept legacy_bidirectional_iterator =
    legacy_forward_iterator<It> && bidirectional_iterator<It> && requires(It it) {
        { *it } -> lvalue_reference;
    };

template <typename It>
struct __UTL_PUBLIC_TEMPLATE is_legacy_bidirectional_iterator :
    bool_constant<legacy_bidirectional_iterator<It>> {};

template <typename It>
inline constexpr bool is_legacy_bidirectional_iterator_v = legacy_bidirectional_iterator<It>;

UTL_NAMESPACE_END

#else // UTL_CXX20

#  include "utl/iterator/utl_indirectly_readable_traits.h"
#  include "utl/iterator/utl_iter_reference_t.h"
#  include "utl/type_traits/utl_is_convertible.h"
#  include "utl/type_traits/utl_is_default_constructible.h"
#  include "utl/type_traits/utl_is_reference.h"
#  include "utl/type_traits/utl_is_same.h"
#  include "utl/type_traits/utl_logical_traits.h"
#  include "utl/type_traits/utl_remove_cvref.h"

UTL_NAMESPACE_BEGIN

namespace details {
namespace legacy_bidirectional_iterator {

template <typename It>
__UTL_HIDE_FROM_ABI auto check(float) -> __UTL false_type;

template <typename It>
__UTL_HIDE_FROM_ABI auto check(int) -> __UTL conjunction<__UTL is_legacy_forward_iterator<It>,
    __UTL is_same<decltype(--static_cast<It (*)()>(0)()), It&>,
    __UTL is_convertible<decltype(static_cast<It (*)()>(0)()--), It const&>,
    __UTL is_same<decltype(*static_cast<It (*)()>(0)()--), __UTL iter_reference_t<It>>>;

template <typename It>
using implemented UTL_NODEBUG =
    decltype(__UTL details::legacy_bidirectional_iterator::check<It>(0));

} // namespace legacy_bidirectional_iterator
} // namespace details

template <typename It>
struct __UTL_PUBLIC_TEMPLATE is_legacy_bidirectional_iterator :
    details::legacy_bidirectional_iterator::implemented<It> {};

#  if UTL_CXX14
template <typename It>
UTL_INLINE_CXX17 constexpr bool is_legacy_bidirectional_iterator_v =
    __UTL is_legacy_bidirectional_iterator<It>::value;
#  endif

UTL_NAMESPACE_END

#endif // UTL_CXX20

#if UTL_CXX14
#  define UTL_TRAIT_is_legacy_bidirectional_iterator(...) \
      __UTL is_legacy_bidirectional_iterator_v<__VA_ARGS__>
#else
#  define UTL_TRAIT_is_legacy_bidirectional_iterator(...) \
      __UTL is_legacy_bidirectional_iterator<__VA_ARGS__>::value
#endif
