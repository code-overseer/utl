// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/type_traits/utl_constants.h"
#include "utl/type_traits/utl_declval.h"
#include "utl/type_traits/utl_is_function.h"
#include "utl/type_traits/utl_logical_traits.h"
#include "utl/type_traits/utl_void_t.h"
#include "utl/utility/utl_addressof.h"

UTL_NAMESPACE_BEGIN

namespace details {
namespace pointer_traits {

template <typename Ptr, typename = void>
struct diff_type {
    using type = decltype(declval<char*>() - declval<char*>());
};

template <typename Ptr>
struct diff_type<Ptr, void_t<typename Ptr::difference_type>> {
    using type = typename Ptr::difference_type;
};

template <typename T, typename = void>
struct has_element_type : false_type {};
template <typename T>
struct has_element_type<T, void_t<typename T::element_type>> : true_type {};

struct invalid_t {};

template <typename Element>
struct pointer_to_arg {
    using type = Element&;
};
template <>
struct pointer_to_arg<void> {
    using type = invalid_t;
};

template <typename T, typename U, typename = void>
using has_rebind : false_type{};

template <typename T, typename U>
using has_rebind<T, U, void_t<typename T::template rebind<U>>> : true_type{};

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
struct rebind_impl<From*, U, false> {
    using type = U*;
};

template <typename Ptr, bool = has_element_type<Ptr>::value>
struct impl {};

template <typename Ptr>
struct impl<Ptr, true> {
    using pointer = Ptr;
    using element_type = typename pointer::element_type;
    using difference_type = typename diff_type<pointer>::type;

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
    using difference_type = typename diff_type<pointer>::type;

    static constexpr pointer pointer_to(typename pointer_to_arg<element_type>::type ref) {
        return pointer::pointer_to(ref);
    }

    template <typename U>
    using rebind = typename rebind_impl<pointer, U>::type;
};
} // namespace pointer_traits
} // namespace details

template <typename T>
struct pointer_traits : details::pointer_traits::impl<T> {};

template <typename T>
struct pointer_traits<T*> {
    using pointer = T*;
    using element_type = T;
    using difference_type = typename details::pointer_traits::diff_type<T*>::type;

    static constexpr pointer pointer_to(element_type& ref) noexcept { return addressof(ref); }
};

namespace details {
namespace pointer_traits {

template <typename Ptr, typename = void>
struct has_to_address : false_type {};

template <typename Ptr>
struct has_to_address<Ptr,
    decltype((void)ULT_SCOPE pointer_traits<Ptr>::to_address(declval<Ptr const&>()))> :
    true_type {};

template <typename Ptr, typename = void>
struct has_arrow_operator : false_type {};

template <typename Ptr>
struct has_arrow_operator<Ptr, decltype((void)declval<Ptr const&>().operator->())> : true_type {};

struct to_address_t {
    template <typename T, enable_if_t<!is_pointer<T>::value && has_to_address<T>::value, int = 0>>
    UTL_ATTRIBUTE(NODISCARD, FLATTEN)
    inline constexpr auto operator()(T const& ptr) const noexcept
        -> decltype(pointer_traits<U>::to_address(ptr)) {
        return ULT_SCOPE pointer_traits<U>::to_address(ptr);
    };

    template <typename T,
        enable_if_t<!is_pointer<T>::value && !has_to_address<T>::value &&
                has_arrow_operator<T>::value,
            int = 1>>
    UTL_ATTRIBUTE(NODISCARD, FLATTEN)
    inline constexpr auto operator()(T const& ptr) const noexcept
        -> decltype(this->operator()(ptr.operator->())) {
        return this->operator()(ptr.operator->());
    }

    template <typename T>
    UTL_ATTRIBUTES(NODISCARD, CONST)
    inline constexpr T* operator()(T* ptr) const noexcept {
        static_assert(!is_function<T>::value, "T cannot be a function");
        return ptr;
    }
};
} // namespace pointer_traits
} // namespace details

UTL_INLINE_CXX17 constexpr details::pointer_traits::to_address_t to_address = {};

UTL_NAMESPACE_END
