// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_config.h"

#include "utl/iterator/utl_iter_move.h"
#include "utl/iterator/utl_iter_reference_t.h"
#include "utl/iterator/utl_iter_rvalue_reference_t.h"
#include "utl/iterator/utl_iter_value_t.h"
#include "utl/type_traits/utl_constants.h"
#include "utl/type_traits/utl_enable_if.h"
#include "utl/type_traits/utl_remove_cvref.h"

#if UTL_CXX20
#  include "utl/concepts/utl_common_reference_with.h"
#  include "utl/concepts/utl_same_as.h"

UTL_NAMESPACE_BEGIN

namespace details {
namespace indirectly_readable {
template <typename T>
concept concept_impl =
    requires(T const t) {
        typename UTL_SCOPE iter_value_t<T>;
        typename UTL_SCOPE iter_reference_t<T>;
        typename UTL_SCOPE iter_rvalue_reference_t<T>;
        { *t } -> UTL_SCOPE same_as<UTL_SCOPE iter_reference_t<T>>;
        {
            UTL_SCOPE ranges::iter_move(t)
        } -> UTL_SCOPE same_as<UTL_SCOPE iter_rvalue_reference_t<T>>;
    } &&
    UTL_SCOPE common_reference_with<UTL_SCOPE iter_reference_t<T>&&, UTL_SCOPE iter_value_t<T>&> &&
    UTL_SCOPE common_reference_with<UTL_SCOPE iter_reference_t<T>&&,
        UTL_SCOPE iter_rvalue_reference_t<T>&&> &&
    UTL_SCOPE
        common_reference_with<UTL_SCOPE iter_rvalue_reference_t<T>&&, UTL_SCOPE iter_value_t<T>&>;

} // namespace indirectly_readable
} // namespace details

template <typename T>
concept indirectly_readable = details::indirectly_readable::concept_impl<UTL_SCOPE remove_cvref<T>>;

template <typename T>
struct is_indirectly_readable : UTL_SCOPE bool_constant<indirectly_readable<T>> {};

template <typename T>
inline constexpr bool is_indirectly_readable_v = indirectly_readable<T>;

UTL_NAMESPACE_END

#else

UTL_NAMESPACE_BEGIN
namespace details {
namespace indirectly_readable {
template <typename T, typename = void>
struct trait : UTL_SCOPE false_type {};

template <typename T>
struct trait<T,
    UTL_SCOPE void_t<UTL_SCOPE iter_value_t<T>, UTL_SCOPE iter_reference_t<T>,
        UTL_SCOPE iter_rvalue_reference_t<T>>> : UTL_SCOPE true_type {};

} // namespace indirectly_readable
} // namespace details

template <typename T>
struct is_indirectly_readable : details::indirectly_readable::trait<UTL_SCOPE remove_cvref_t<T>> {};

#  if UTL_CXX14
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_indirectly_readable_v = details::indirectly_readable::trait<T>::value;
#  endif

UTL_NAMESPACE_END
#endif // UTL_CXX20
