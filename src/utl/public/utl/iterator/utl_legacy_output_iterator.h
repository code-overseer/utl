// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

#include "utl/iterator/utl_iterator_concept_t.h"
#include "utl/type_traits/utl_constants.h"
#if UTL_CXX20

#  include "utl/iterator/utl_output_iterator.h"

UTL_NAMESPACE_BEGIN

template <typename It, typename ValueType>
concept legacy_output_iterator = __UTL legacy_iterator<It> &&
    __UTL output_iterator<It, ValueType> && requires { typename __UTL iterator_concept_t<It>; };

template <typename It, typename ValueType>
struct __UTL_PUBLIC_TEMPLATE is_legacy_output_iterator :
    bool_constant<legacy_output_iterator<It, ValueType>> {};

template <typename It, typename ValueType>
inline constexpr bool is_legacy_output_iterator_v = __UTL legacy_output_iterator<It, ValueType>;

UTL_NAMESPACE_END

#else // UTL_CXX20

#  include "utl/iterator/utl_iterator_tags.h"
#  include "utl/type_traits/utl_declval.h"
#  include "utl/type_traits/utl_is_convertible.h"
#  include "utl/type_traits/utl_is_same.h"
#  include "utl/type_traits/utl_logical_traits.h"

UTL_NAMESPACE_BEGIN

namespace details {
namespace legacy_output_iterator {

template <typename It, typename ValueType>
__UTL_HIDE_FROM_ABI auto check(float) noexcept -> __UTL false_type;

template <typename It, typename ValueType>
__UTL_HIDE_FROM_ABI auto check(int) noexcept -> __UTL conjunction<__UTL is_legacy_iterator<It>,
    decltype((*__UTL declval<It&>() = __UTL declval<ValueType>(), __UTL true_type{})),
    __UTL is_same<decltype(++__UTL declval<It&>()), It&>,
    __UTL is_convertible<decltype(__UTL declval<It&>()++), It const&>,
    decltype((*__UTL declval<It&>()++ = __UTL declval<ValueType>(), __UTL true_type{})),
    __UTL is_iterator_tag<__UTL iterator_concept_t<It>>>;

template <typename It, typename ValueType>
using implemented UTL_NODEBUG =
    decltype(__UTL details::legacy_output_iterator::check<It, ValueType>(0));

} // namespace legacy_output_iterator
} // namespace details

template <typename It, typename ValueType>
struct __UTL_PUBLIC_TEMPLATE is_legacy_output_iterator :
    __UTL details::legacy_output_iterator::implemented<It, ValueType> {};

#  if UTL_CXX14
template <typename It, typename ValueType>
UTL_INLINE_CXX17 constexpr bool is_legacy_output_iterator_v =
    __UTL is_legacy_output_iterator<It, ValueType>::value;
#  endif

UTL_NAMESPACE_END

#endif // UTL_CXX20

#if UTL_CXX14
#  define UTL_TRAIT_is_legacy_output_iterator(...) __UTL is_legacy_output_iterator_v<__VA_ARGS__>
#else
#  define UTL_TRAIT_is_legacy_output_iterator(...) \
      __UTL is_legacy_output_iterator<__VA_ARGS__>::value
#endif
