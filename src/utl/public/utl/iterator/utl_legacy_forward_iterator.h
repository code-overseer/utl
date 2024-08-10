// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_config.h"

#include "utl/iterator/utl_legacy_input_iterator.h"
#include "utl/type_traits/utl_constants.h"

#if UTL_CXX20

#  include "utl/concepts/utl_reference.h"
#  include "utl/iterator/utl_forward_iterator.h"

UTL_NAMESPACE_BEGIN

template <typename It>
concept legacy_forward_iterator =
    legacy_input_iterator<It> && forward_iterator<It> && requires(It i) {
        { *i } -> reference;
    };

template <typename It>
struct UTL_PUBLIC_TEMPLATE is_legacy_forward_iterator :
    bool_constant<legacy_forward_iterator<It>> {};

template <typename It>
inline constexpr bool is_legacy_forward_iterator_v = legacy_forward_iterator<It>;

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
namespace legacy_forward_iterator {

template <typename It>
UTL_HIDE_FROM_ABI auto check(float) -> UTL_SCOPE false_type;

template <typename It>
UTL_HIDE_FROM_ABI auto check(int) -> UTL_SCOPE conjunction<UTL_SCOPE is_legacy_input_iterator<It>,
    UTL_SCOPE is_default_constructible<It>, UTL_SCOPE is_reference<UTL_SCOPE iter_reference_t<It>>,
    UTL_SCOPE is_same<UTL_SCOPE remove_cvref_t<UTL_SCOPE iter_reference_t<It>>,
        typename UTL_SCOPE indirectly_readable_traits<It>::value_type>,
    UTL_SCOPE is_convertible<decltype(++static_cast<It& (*)()>(0)()), It const&>,
    UTL_SCOPE is_same<decltype(*static_cast<It& (*)()>(0)()++), UTL_SCOPE iter_reference_t<It>>>;

template <typename It>
using implemented UTL_NODEBUG = decltype(UTL_SCOPE details::legacy_forward_iterator::check<It>(0));

} // namespace legacy_forward_iterator
} // namespace details

template <typename It>
struct UTL_PUBLIC_TEMPLATE is_legacy_forward_iterator :
    details::legacy_forward_iterator::implemented<It> {};

#  if UTL_CXX14
template <typename It>
UTL_INLINE_CXX17 constexpr bool is_legacy_forward_iterator_v = is_legacy_forward_iterator<It>::value;
#  endif

UTL_NAMESPACE_END

#endif // UTL_CXX20

#if UTL_CXX14
#  define UTL_TRAIT_is_legacy_forward_iterator(...) \
      UTL_SCOPE is_legacy_forward_iterator_v<__VA_ARGS__>
#else
#  define UTL_TRAIT_is_legacy_forward_iterator(...) \
      UTL_SCOPE is_legacy_forward_iterator<__VA_ARGS__>::value
#endif
