// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/type_traits/utl_common.h"

#if defined(UTL_USE_STD_TYPE_TRAITS) && defined(UTL_CXX23)

#  include <type_traits>

UTL_NAMESPACE_BEGIN

using std::reference_constructs_from_temporary;

using std::reference_constructs_from_temporary_v;

UTL_NAMESPACE_END

#  define UTL_TRAIT_SUPPORTED_reference_constructs_from_temporary 1

#else // ifdef UTL_USE_STD_TYPE_TRAITS

#  include "utl/type_traits/utl_constants.h"
#  include "utl/type_traits/utl_reference_binds_to_temporary.h"

#  ifndef UTL_DISABLE_BUILTIN_reference_constructs_from_temporary
#    define UTL_DISABLE_BUILTIN_reference_constructs_from_temporary 0
#  endif

#  if UTL_SHOULD_USE_BUILTIN(reference_constructs_from_temporary)
#    define UTL_BUILTIN_reference_constructs_from_temporary(...) \
        __reference_constructs_from_temporary(__VA_ARGS__)
#  endif // UTL_SHOULD_USE_BUILTIN(reference_constructs_from_temporary)

#  ifdef UTL_BUILTIN_reference_constructs_from_temporary

UTL_NAMESPACE_BEGIN

template <typename T, typename U>
struct reference_constructs_from_temporary :
    bool_constant<UTL_BUILTIN_reference_constructs_from_temporary(T, U)> {};

#    ifdef UTL_CXX14
template <typename T, typename U>
UTL_INLINE_CXX17 constexpr bool reference_constructs_from_temporary_v =
    UTL_BUILTIN_reference_constructs_from_temporary(T, U);
#    endif // UTL_CXX14

UTL_NAMESPACE_END

#  elif defined(UTL_BUILTIN_reference_binds_to_temporary)

#    include "utl/type_traits/utl_is_convertible.h"
#    include "utl/type_traits/utl_is_same.h"
#    include "utl/type_traits/utl_remove_cv.h"
#    include "utl/type_traits/utl_remove_cvref.h"

#    ifdef UTL_CXX14
#      define UTL_TRAIT_VALUE(TRAIT, ...) TRAIT##_v<__VA_ARGS__>
#    else
#      define UTL_TRAIT_VALUE(TRAIT, ...) TRAIT<__VA_ARGS__>::value
#    endif

UTL_NAMESPACE_BEGIN

template <typename T, typename U>
struct reference_constructs_from_temporary : false_type {};

namespace details {
namespace dangling {

/**
 * Checks valid conversion in unevaluated context
 * @tparam T - reference only, first argument of reference_constructs_from_temporary
 */
template <typename T>
void init_ref(T ref) noexcept;

/**
 * Creates a prvalue in unevaluated context
 * @tparam U - any type
 */
template <typename U>
remove_cv_t<U> make_type() noexcept;

template <typename T, typename U>
using constructs_from_impl = bool_constant<UTL_BUILTIN_reference_binds_to_temporary(T, U) ||
    UTL_TRAIT_VALUE(is_convertible, remove_cvref_t<U>*, remove_cvref_t<T>*)>;

template <typename T, typename U, typename = void>
struct constructs_from : false_type {};

template <typename T, typename U>
struct constructs_from<T, U&, decltype(init_ref<T>(make_type<U&>()))> :
    bool_constant<!UTL_TRAIT_VALUE(is_same, remove_cvref_t<T>, remove_cv_t<U>) &&
        constructs_from_impl<T, U&>::value> {};

template <typename T, typename U>
struct constructs_from<T, U&&, decltype(init_ref<T>(make_type<U&&>()))> :
    bool_constant<!UTL_TRAIT_VALUE(is_same, remove_cvref_t<T>, remove_cv_t<U>) &&
        constructs_from_impl<T, U&&>::value> {};

template <typename T, typename U>
struct constructs_from<T, U, decltype(init_ref<T>(make_type<U>()))> : constructs_from_impl<T, U> {};

} // namespace dangling
} // namespace details

template <typename T, typename U>
struct reference_constructs_from_temporary<T&, U> : details::dangling::constructs_from<T&, U> {};

template <typename T, typename U>
struct reference_constructs_from_temporary<T&&, U> : details::dangling::constructs_from<T&&, U> {};

#    ifdef UTL_CXX14
template <typename T, typename U>
UTL_INLINE_CXX17 constexpr bool reference_constructs_from_temporary_v =
    reference_constructs_from_temporary<T, U>::value;
#    endif // UTL_CXX14

UTL_NAMESPACE_END

#    undef UTL_TRAIT_VALUE

#    define UTL_TRAIT_SUPPORTED_reference_constructs_from_temporary 1

#  else // ifdef UTL_BUILTIN_reference_constructs_from_temporary

UTL_NAMESPACE_BEGIN

template <typename T, typename U>
struct reference_constructs_from_temporary : false_type {};

UTL_NAMESPACE_END

#  endif

#endif // ifdef UTL_USE_STD_TYPE_TRAITS
