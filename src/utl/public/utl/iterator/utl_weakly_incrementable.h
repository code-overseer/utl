// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/iterator/utl_iter_difference_t.h"
#include "utl/preprocessor/utl_config.h"
#include "utl/type_traits/utl_constants.h"
#include "utl/type_traits/utl_enable_if.h"

#if UTL_CXX20
#  include "utl/concepts/utl_movable.h"
#  include "utl/concepts/utl_same_as.h"
#  include "utl/concepts/utl_signed_integral.h"

UTL_NAMESPACE_BEGIN

template <typename T>
concept weakly_incrementable = UTL_SCOPE movable<T> && requires(T t) {
    typename UTL_SCOPE iter_difference_t<T>;
    requires UTL_SCOPE signed_integral<UTL_SCOPE iter_difference_t<T>>;
    { ++t } -> UTL_SCOPE same_as<T&>;
    i++;
};

template <typename T>
struct is_weakly_incrementable : UTL_SCOPE bool_constant<weakly_incrementable<T>> {};

template <typename T>
inline constexpr bool is_weakly_incrementable_v = weakly_incrementable<T>;

UTL_NAMESPACE_END

#else

#  include "utl/type_traits/utl_is_same.h"
#  include "utl/type_traits/utl_is_signed_integral.h"

UTL_NAMESPACE_BEGIN
namespace details {
namespace weakly_incrementable {
template <typename T, typename = void>
struct trait : UTL_SCOPE false_type {};

template <typename T>
struct trait<T,
    UTL_SCOPE void_t<UTL_SCOPE iter_difference_t<T>,
        UTL_SCOPE enable_if_t<
            UTL_SCOPE conjunction<UTL_SCOPE is_signed_integral<UTL_SCOPE iter_difference_t<T>>,
                UTL_SCOPE is_same<decltype(++static_cast<T (*)()>(0)()), T&>>::value>,
        decltype(i++)>> : UTL_SCOPE true_type {};

} // namespace weakly_incrementable
} // namespace details

template <typename T>
struct is_weakly_incrementable : details::weakly_incrementable::trait<T> {};

#  if UTL_CXX14
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_weakly_incrementable_v = details::weakly_incrementable::trait<T>::value;
#  endif

UTL_NAMESPACE_END
#endif // UTL_CXX20
