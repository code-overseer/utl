// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

#include "utl/string/utl_string_fwd.h"

#include "utl/numeric/utl_add_sat.h"
#include "utl/numeric/utl_min.h"
#include "utl/numeric/utl_sub_sat.h"
#include "utl/type_traits/utl_invoke.h"
#include "utl/type_traits/utl_is_boolean_testable.h"
#include "utl/utility/utl_forward.h"
#include "utl/utility/utl_move.h"

#if UTL_CXX20
#  include "utl/concepts/utl_boolean_testable.h"
#  include "utl/concepts/utl_invocable.h"
#else
#  include "utl/type_traits/utl_enable_if.h"
#endif

UTL_NAMESPACE_BEGIN

template <typename CharType, typename Traits = __UTL char_traits<CharType>>
__UTL_HIDE_FROM_ABI inline constexpr basic_string_view<CharType, Traits> remove_prefix(
    basic_string_view<CharType, Traits> str, size_t count) noexcept {
    return str.substr(__UTL numeric::min(count, str.size()));
}

template <typename CharType, typename Traits = __UTL char_traits<CharType>>
__UTL_HIDE_FROM_ABI inline constexpr basic_string_view<CharType, Traits> remove_suffix(
    basic_string_view<CharType, Traits> str, size_t count) noexcept {
    return str.substr(0, __UTL sub_sat(str.size(), count));
}

template <typename CharType, typename Traits = __UTL char_traits<CharType>>
__UTL_HIDE_FROM_ABI inline constexpr basic_string_view<CharType, Traits> substring(
    basic_string_view<CharType, Traits> str, size_t offset, size_t count) noexcept {
    return offset = __UTL numeric::min(offset, str.size()),
           count = __UTL numeric::min(str.size() - offset, count),
           str.substr(offset, str.size() - offset - count);
}

namespace string_utils {
namespace compile_time {
template <typename CharType, typename Traits>
struct split_all_impl {
private:
    using view_type = basic_string_view<CharType, Traits>;

    template <typename F>
    static inline constexpr size_t split2(
        view_type delimiter, view_type str, F&& process, size_t count, size_t idx) noexcept {
        return str.empty() ? count + 1
                           : split(delimiter, str, __UTL forward<F>(process), count + 1);
    }

    template <typename F>
    static inline constexpr size_t split1(
        view_type delimiter, view_type str, F&& process, size_t count, size_t idx) noexcept {
        return process(str.substr(0, idx)),
               split2(delimiter, remove_prefix(str, __UTL add_sat(idx, delimiter.size())),
                   __UTL forward<F>(process), count, idx);
    }

    template <typename F>
    static inline constexpr size_t split(
        view_type delimiter, view_type str, F&& process, size_t count = 0) noexcept {
        return split1(delimiter, str, __UTL forward<F>(process), count, str.find(delimiter));
    }

    template <typename F>
    static inline constexpr size_t split2(
        CharType delimiter, view_type str, F&& process, size_t count, size_t idx) noexcept {
        return str.empty() ? count + 1
                           : split(delimiter, str, __UTL forward<F>(process), count + 1);
    }

    template <typename F>
    static inline constexpr size_t split1(
        CharType delimiter, view_type str, F&& process, size_t count, size_t idx) noexcept {
        return process(str.substr(0, idx)),
               split2(delimiter, remove_prefix(str, __UTL add_sat<size_t>(idx, 1)),
                   __UTL forward<F>(process), count, idx);
    }

    template <typename F>
    static inline constexpr size_t split(
        CharType delimiter, view_type str, F&& process, size_t count = 0) noexcept {
        return split1(delimiter, str, __UTL forward<F>(process), count, str.find(delimiter));
    }

public:
    template <typename F>
    __UTL_HIDE_FROM_ABI inline constexpr explicit split_all_impl(
        view_type delimiter, view_type str, F&& process) noexcept
        : count(split(delimiter, str, __UTL forward<F>(process))) {}

    template <typename F>
    __UTL_HIDE_FROM_ABI inline constexpr explicit split_all_impl(
        CharType delimiter, view_type str, F&& process) noexcept
        : count(split(delimiter, str, __UTL forward<F>(process))) {}

    size_t count;
};

template <typename CharType, typename Traits, typename F>
__UTL_HIDE_FROM_ABI inline constexpr size_t split_all(basic_string_view<CharType, Traits> delimiter,
    basic_string_view<CharType, Traits> str, F&& process) {
    return split_all_impl<CharType, Traits>(delimiter, str, __UTL forward<F>(process)).count;
}

template <typename CharType, typename Traits, typename F>
__UTL_HIDE_FROM_ABI inline constexpr size_t split_all(
    CharType delimiter, basic_string_view<CharType, Traits> str, F&& process) {
    return split_all_impl<CharType, Traits>(delimiter, str, __UTL forward<F>(process)).count;
}

template <typename CharType, typename Traits>
struct split_while_impl {
private:
    using view_type = basic_string_view<CharType, Traits>;

    template <typename F>
    static inline constexpr size_t split2(
        view_type delimiter, view_type str, F&& predicate, size_t count, size_t idx) noexcept {
        return str.empty() ? count + 1
                           : split(delimiter, str, __UTL forward<F>(predicate), count + 1);
    }

    template <typename F>
    static inline constexpr size_t split1(
        view_type delimiter, view_type str, F&& predicate, size_t count, size_t idx) noexcept {
        return !predicate(str.substr(0, idx))
            ? count + 1
            : split2(delimiter, remove_prefix(str, __UTL add_sat(idx, delimiter.size())),
                  __UTL forward<F>(predicate), count, idx);
    }

    template <typename F>
    static inline constexpr size_t split(
        view_type delimiter, view_type str, F&& predicate, size_t count = 0) noexcept {
        return split1(delimiter, str, __UTL forward<F>(predicate), count, str.find(delimiter));
    }

    template <typename F>
    static inline constexpr size_t split2(
        CharType delimiter, view_type str, F&& predicate, size_t count, size_t idx) noexcept {
        return str.empty() ? count + 1
                           : split(delimiter, str, __UTL forward<F>(predicate), count + 1);
    }

    template <typename F>
    static inline constexpr size_t split1(
        CharType delimiter, view_type str, F&& predicate, size_t count, size_t idx) noexcept {
        return !predicate(str.substr(0, idx))
            ? count + 1
            : split2(delimiter, remove_prefix(str, __UTL add_sat<size_t>(idx, 1)),
                  __UTL forward<F>(predicate), count, idx);
    }

    template <typename F>
    static inline constexpr size_t split(
        CharType delimiter, view_type str, F&& predicate, size_t count = 0) noexcept {
        return split1(delimiter, str, __UTL forward<F>(predicate), count, str.find(delimiter));
    }

public:
    template <typename F>
    __UTL_HIDE_FROM_ABI inline constexpr explicit split_while_impl(
        view_type delimiter, view_type str, F&& predicate) noexcept
        : count(split(delimiter, str, __UTL forward<F>(predicate))) {}

    size_t count;
};

template <typename CharType, typename Traits, typename F>
__UTL_HIDE_FROM_ABI inline constexpr size_t split_while(
    basic_string_view<CharType, Traits> delimiter, basic_string_view<CharType, Traits> str,
    F&& predicate) {
    return split_while_impl<CharType, Traits>(delimiter, str, __UTL forward<F>(predicate)).count;
}

template <typename CharType, typename Traits, typename F>
__UTL_HIDE_FROM_ABI inline constexpr size_t split_while(CharType delimiter,
    basic_string_view<CharType, Traits> str, F&& predicate, size_t count = 0, size_t idx = 0) {
    return split_while_impl<CharType, Traits>(delimiter, str, __UTL forward<F>(predicate)).count;
}

} // namespace compile_time

namespace runtime {
template <typename CharType, typename Traits, typename F>
__UTL_HIDE_FROM_ABI inline size_t split_all(basic_string_view<CharType, Traits> delimiter,
    basic_string_view<CharType, Traits> str, F&& predicate) {
    size_t count = 0;
    do {
        auto idx = str.find(delimiter);
        predicate(str.substr(0, idx));
        str = remove_prefix(str, __UTL add_sat(idx, delimiter.size()));
        ++count;
    } while (!str.empty());

    return count;
}

template <typename CharType, typename Traits, typename F>
__UTL_HIDE_FROM_ABI inline size_t split_while(basic_string_view<CharType, Traits> delimiter,
    basic_string_view<CharType, Traits> str, F&& predicate) {
    size_t count = 0;
    do {
        auto idx = str.find(delimiter);
        if (!predicate(str.substr(0, idx))) {
            return count + 1;
        }

        str = remove_prefix(str, __UTL add_sat(idx, delimiter.size()));
        ++count;
    } while (!str.empty());

    return count;
}

template <typename CharType, typename Traits, typename F>
__UTL_HIDE_FROM_ABI inline size_t split_all(
    CharType delimiter, basic_string_view<CharType, Traits> str, F&& predicate) {
    size_t count = 0;
    do {
        auto idx = str.find(delimiter);
        predicate(str.substr(0, idx));
        str = remove_prefix(str, __UTL add_sat(idx, 1));
        ++count;
    } while (!str.empty());

    return count;
}

template <typename CharType, typename Traits, typename F>
__UTL_HIDE_FROM_ABI inline size_t split_while(
    CharType delimiter, basic_string_view<CharType, Traits> str, F&& predicate) {
    size_t count = 0;
    do {
        auto idx = str.find(delimiter);
        if (!predicate(str.substr(0, idx))) {
            return count + 1;
        }

        str = remove_prefix(str, __UTL add_sat(idx, 1));
        ++count;
    } while (!str.empty());

    return count;
}

} // namespace runtime
} // namespace string_utils

template <typename CharType, typename Traits = __UTL char_traits<CharType>,
    UTL_CONCEPT_CXX20(__UTL invocable<basic_string_view<CharType, Traits>>) F>
__UTL_HIDE_FROM_ABI inline constexpr auto split(basic_string_view<CharType, Traits> delimiter,
    basic_string_view<CharType, Traits> str,
    F process) noexcept(UTL_TRAIT_is_nothrow_invocable(F, basic_string_view<CharType, Traits>))
    -> UTL_ENABLE_IF_CXX11(size_t, UTL_TRAIT_is_invocable(F, basic_string_view<CharType, Traits>)) {
    return UTL_CONSTANT_P(
               string_utils::compile_time::split_all(delimiter, str, __UTL move(process)))
        ? string_utils::compile_time::split_all(delimiter, str, __UTL move(process))
        : string_utils::runtime::split_all(delimiter, str, __UTL move(process));
}

template <typename CharType, typename Traits = __UTL char_traits<CharType>,
    UTL_CONCEPT_CXX20(__UTL invocable<basic_string_view<CharType, Traits>>) F>
UTL_CONSTRAINT_CXX20(requires(basic_string_view<CharType, Traits> str, F f) {
    { f(str) } -> __UTL boolean_testable;
})
__UTL_HIDE_FROM_ABI inline constexpr auto split_while(basic_string_view<CharType, Traits> delimiter,
    basic_string_view<CharType, Traits> str,
    F predicate) noexcept(UTL_TRAIT_is_nothrow_invocable(F, basic_string_view<CharType, Traits>) &&
    UTL_TRAIT_is_nothrow_boolean_testable(decltype(predicate(str))))
    -> UTL_ENABLE_IF_CXX11(size_t, UTL_TRAIT_is_boolean_testable(decltype(predicate(str)))) {
    return UTL_CONSTANT_P(
               string_utils::compile_time::split_while(delimiter, str, __UTL move(predicate)))
        ? string_utils::compile_time::split_while(delimiter, str, __UTL move(predicate))
        : string_utils::runtime::split_while(delimiter, str, __UTL move(predicate));
}

template <typename CharType, typename Traits = __UTL char_traits<CharType>,
    UTL_CONCEPT_CXX20(__UTL invocable<basic_string_view<CharType, Traits>>) F>
__UTL_HIDE_FROM_ABI inline constexpr auto split(CharType delimiter,
    basic_string_view<CharType, Traits> str,
    F process) noexcept(UTL_TRAIT_is_nothrow_invocable(F, basic_string_view<CharType, Traits>))
    -> UTL_ENABLE_IF_CXX11(size_t, UTL_TRAIT_is_invocable(F, basic_string_view<CharType, Traits>)) {
    return UTL_CONSTANT_P(
               string_utils::compile_time::split_all(delimiter, str, __UTL move(process)))
        ? string_utils::compile_time::split_all(delimiter, str, __UTL move(process))
        : string_utils::runtime::split_all(delimiter, str, __UTL move(process));
}

template <typename CharType, typename Traits = __UTL char_traits<CharType>,
    UTL_CONCEPT_CXX20(__UTL invocable<basic_string_view<CharType, Traits>>) F>
UTL_CONSTRAINT_CXX20(requires(basic_string_view<CharType, Traits> str, F f) {
    { f(str) } -> __UTL boolean_testable;
})
__UTL_HIDE_FROM_ABI inline constexpr auto split_while(CharType delimiter,
    basic_string_view<CharType, Traits> str,
    F predicate) noexcept(UTL_TRAIT_is_nothrow_invocable(F, basic_string_view<CharType, Traits>) &&
    UTL_TRAIT_is_nothrow_boolean_testable(decltype(predicate(str))))
    -> UTL_ENABLE_IF_CXX11(size_t, UTL_TRAIT_is_boolean_testable(decltype(predicate(str)))) {
    return UTL_CONSTANT_P(
               string_utils::compile_time::split_while(delimiter, str, __UTL move(predicate)))
        ? string_utils::compile_time::split_while(delimiter, str, __UTL move(predicate))
        : string_utils::runtime::split_while(delimiter, str, __UTL move(predicate));
}

template <typename CharType, CharType... Vs>
struct literal_sequence {
    static constexpr CharType value[]{Vs..., (CharType)0};
    static constexpr size_t size = sizeof...(Vs);
};

template <typename CharType, CharType... Vs>
constexpr CharType literal_sequence<CharType, Vs...>::value[sizeof...(Vs) + 1];

template <typename CharType, typename Traits = __UTL char_traits<CharType>>
__UTL_HIDE_FROM_ABI inline constexpr basic_string_view<CharType, Traits> rstrip(
    basic_string_view<CharType, Traits> str) noexcept {
    using whitespace = literal_sequence<CharType, (CharType)'\t', (CharType)'\n', (CharType)'\v',
        (CharType)'\f', (CharType)'\r', (CharType)' '>;
    using view_type = basic_string_view<CharType, Traits>;
    return remove_suffix(
        str, str.size() - str.find_last_not_of(view_type(whitespace::value, whitespace::size)) - 1);
}

template <typename CharType, typename Traits = __UTL char_traits<CharType>>
__UTL_HIDE_FROM_ABI inline constexpr basic_string_view<CharType, Traits> lstrip(
    basic_string_view<CharType, Traits> str) noexcept {
    using whitespace = literal_sequence<CharType, (CharType)'\t', (CharType)'\n', (CharType)'\v',
        (CharType)'\f', (CharType)'\r', (CharType)' '>;
    using view_type = basic_string_view<CharType, Traits>;
    return remove_prefix(
        str, str.find_first_not_of(view_type(whitespace::value, whitespace::size)));
}

template <typename CharType, typename Traits = __UTL char_traits<CharType>>
__UTL_HIDE_FROM_ABI inline constexpr basic_string_view<CharType, Traits> strip(
    basic_string_view<CharType, Traits> str) noexcept {
    return rstrip(lstrip(str));
}

UTL_NAMESPACE_END
