// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_config.h"

#include "utl/memory/utl_addressof.h"
#include "utl/type_traits/utl_is_function.h"
#include "utl/type_traits/utl_is_pointer.h"
#include "utl/type_traits/utl_logical_traits.h"
#include "utl/utility/utl_customization_point.h"

#if UTL_CXX20

#  include "utl/concepts/utl_referenceable.h"

UTL_NAMESPACE_BEGIN

namespace details {
namespace pointer_traits {
template <typename P>
struct diff_type;

template <typename P>
using diff_type_t = typename diff_type<P>::type;

template <typename P>
requires requires { typename P::difference_type; }
struct diff_type<P> {
    using type = typename P::difference_type;
};

template <typename P>
struct diff_type {
    using type = decltype((char*)(0) - (char*)(0));
};

template <typename Ptr, typename T>
struct rebind_impl;
template <typename Ptr, typename T>
requires requires { typename Ptr::template rebind<T>; }
struct rebind_impl<Ptr, T> {
    using type = typename Ptr::template rebind<T>;
};

template <template <typename...> class Template, typename T, typename... Args, typename U>
requires requires { typename Template<T, Args...>::template rebind<U>; }
struct rebind_impl<Template<T, Args...>, U> {
    using type = typename Template<T, Args...>::template rebind<U>;
};

template <template <typename...> class Template, typename T, typename... Args, typename U>
struct rebind_impl<Template<T, Args...>, U> {
    using type = Template<U, Args...>;
};

template <typename From, typename To>
struct rebind_impl<From*, To> {
    using type = To*;
};

template <typename Ptr>
struct impl {};

template <typename Ptr>
requires requires { typename Ptr::element_type; }
struct impl<Ptr> {
    using pointer = Ptr;
    using element_type = typename pointer::element_type;
    using difference_type = diff_type_t<pointer>;

    static constexpr pointer pointer_to(element_type& ref)
    requires UTL_SCOPE
    referenceable<element_type> {
        return pointer::pointer_to(ref);
    }

    template <typename U>
    using rebind = typename rebind_impl<pointer, U>::type;
};

template <template <typename...> class Template, typename T, typename... Args>
struct impl<Template<T, Args...>> {
    using pointer = Template<T, Args...>;
    using element_type = T;
    using difference_type = diff_type_t<pointer>;

    static constexpr pointer pointer_to(element_type& ref)
    requires UTL_SCOPE
    referenceable<element_type> {
        return pointer::pointer_to(ref);
    }

    template <typename U>
    using rebind = typename rebind_impl<pointer, U>::type;
};

} // namespace pointer_traits
} // namespace details

UTL_NAMESPACE_END

#else

#  include "utl/type_traits/utl_constants.h"
#  include "utl/type_traits/utl_declval.h"
#  include "utl/type_traits/utl_has_member_difference_type.h"
#  include "utl/type_traits/utl_has_member_element_type.h"

UTL_NAMESPACE_BEGIN

namespace details {
namespace pointer_traits {

struct invalid_t {};

template <typename Element>
struct pointer_to_arg {
    using type = Element&;
};
template <>
struct pointer_to_arg<void> {
    using type = invalid_t;
};

template <typename P>
using diff_type_t = UTL_SCOPE conditional_t<UTL_TRAIT_has_member_difference_type(P),
    typename P::difference_type, decltype((char*)(0) - (char*)(0))>;

template <typename T, typename U>
auto has_rebind_impl(float) noexcept -> UTL_SCOPE false_type;

template <typename T, typename U>
auto has_rebind_impl(int) noexcept -> UTL_SCOPE always_true<typename T::template rebind<U>>;

template <typename T, typename U>
using has_rebind = decltype(has_rebind_impl<T, U>(0));

template <typename Ptr, typename T, bool = has_rebind<Ptr, T>::value>
struct rebind_impl;

template <typename Ptr, typename T>
struct rebind_impl<Ptr, T, true> {
    using type = typename Ptr::template rebind<T>;
};

template <template <typename...> class Template, typename T, typename... Args, typename U>
struct rebind_impl<Template<T, Args...>, U, true> {
    using type = typename Template<T, Args...>::template rebind<U>;
};

template <template <typename...> class Template, typename T, typename... Args, typename U>
struct rebind_impl<Template<T, Args...>, U, false> {
    using type = Template<U, Args...>;
};

template <typename From, typename To>
struct rebind_impl<From*, To, false> {
    using type = To*;
};

template <typename Ptr, bool = UTL_TRAIT_has_member_element_type(Ptr)>
struct impl {};

template <typename Ptr>
struct impl<Ptr, true> {
    using pointer = Ptr;
    using element_type = typename pointer::element_type;
    using difference_type = diff_type_t<pointer>;

    static constexpr pointer pointer_to(typename pointer_to_arg<element_type>::type ref) {
        return pointer::pointer_to(ref);
    }

    template <typename U>
    using rebind = typename rebind_impl<pointer, U>::type;
};

template <template <typename...> class Template, typename T, typename... Args>
struct impl<Template<T, Args...>, false> {
    using pointer = Template<T, Args...>;
    using element_type = T;
    using difference_type = diff_type_t<pointer>;

    static constexpr pointer pointer_to(typename pointer_to_arg<element_type>::type ref) {
        return pointer::pointer_to(ref);
    }

    template <typename U>
    using rebind = typename rebind_impl<pointer, U>::type;
};

} // namespace pointer_traits
} // namespace details

UTL_NAMESPACE_END

#endif

UTL_NAMESPACE_BEGIN

template <typename T>
struct pointer_traits : details::pointer_traits::impl<T> {};

template <typename T>
struct pointer_traits<T*> {
    using pointer = T*;
    using element_type = T;
    using difference_type = details::pointer_traits::diff_type_t<T*>;
    template <typename U>
    using rebind = U*;

    static constexpr pointer pointer_to(element_type& ref) noexcept {
        return UTL_SCOPE addressof(ref);
    }
};

UTL_NAMESPACE_END
