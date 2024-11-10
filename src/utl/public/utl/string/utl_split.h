// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

#include "utl/string/utl_string_fwd.h"

#include "utl/numeric/utl_add_sat.h"
#include "utl/utility/utl_forward.h"
#include "utl/utility/utl_move.h"

UTL_NAMESPACE_BEGIN

namespace string_utils {
namespace compile_time {

template <typename T>
__UTL_HIDE_FROM_ABI void remove_prefix(T) noexcept = delete;

template <typename CharType, typename Traits>
struct split_all_impl {
private:
    using view_type = basic_string_view<CharType, Traits>;

    template <typename F>
    __UTL_HIDE_FROM_ABI static inline constexpr size_t split2(
        view_type str, view_type delimiter, F&& process, size_t count, size_t idx) noexcept {
        return str.empty() ? count + 1
                           : split(str, delimiter, __UTL forward<F>(process), count + 1);
    }

    template <typename F>
    __UTL_HIDE_FROM_ABI static inline constexpr size_t split1(
        view_type str, view_type delimiter, F&& process, size_t count, size_t idx) noexcept {
        return process(str.substr(0, idx)),
               split2(remove_prefix(str, __UTL add_sat(idx, delimiter.size())), delimiter,
                   __UTL forward<F>(process), count, idx);
    }

    template <typename F>
    __UTL_HIDE_FROM_ABI static inline constexpr size_t split(
        view_type str, view_type delimiter, F&& process, size_t count = 0) noexcept {
        return split1(str, delimiter, __UTL forward<F>(process), count, str.find(delimiter));
    }

    template <typename F>
    __UTL_HIDE_FROM_ABI static inline constexpr size_t split2(
        view_type str, CharType delimiter, F&& process, size_t count, size_t idx) noexcept {
        return str.empty() ? count + 1
                           : split(str, delimiter, __UTL forward<F>(process), count + 1);
    }

    template <typename F>
    __UTL_HIDE_FROM_ABI static inline constexpr size_t split1(
        view_type str, CharType delimiter, F&& process, size_t count, size_t idx) noexcept {
        return process(str.substr(0, idx)),
               split2(remove_prefix(str, __UTL add_sat<size_t>(idx, 1)), delimiter,
                   __UTL forward<F>(process), count, idx);
    }

    template <typename F>
    __UTL_HIDE_FROM_ABI static inline constexpr size_t split(
        view_type str, CharType delimiter, F&& process, size_t count = 0) noexcept {
        return split1(str, delimiter, __UTL forward<F>(process), count, str.find(delimiter));
    }

public:
    template <typename F>
    __UTL_HIDE_FROM_ABI inline constexpr explicit split_all_impl(
        view_type str, view_type delimiter, F&& process) noexcept
        : count(split(str, delimiter, __UTL forward<F>(process))) {}

    template <typename F>
    __UTL_HIDE_FROM_ABI inline constexpr explicit split_all_impl(
        view_type str, CharType delimiter, F&& process) noexcept
        : count(split(str, delimiter, __UTL forward<F>(process))) {}

    size_t count;
};

template <typename CharType, typename Traits, typename F>
__UTL_HIDE_FROM_ABI inline constexpr size_t split_all(basic_string_view<CharType, Traits> str,
    basic_string_view<CharType, Traits> delimiter, F&& process) {
    return split_all_impl<CharType, Traits>(str, delimiter, __UTL forward<F>(process)).count;
}

template <typename CharType, typename Traits, typename F>
__UTL_HIDE_FROM_ABI inline constexpr size_t split_all(
    basic_string_view<CharType, Traits> str, CharType delimiter, F&& process) {
    return split_all_impl<CharType, Traits>(str, delimiter, __UTL forward<F>(process)).count;
}

template <typename CharType, typename Traits>
struct split_while_impl {
private:
    using view_type = basic_string_view<CharType, Traits>;

    template <typename F>
    __UTL_HIDE_FROM_ABI static inline constexpr size_t split2(
        view_type str, view_type delimiter, F&& predicate, size_t count, size_t idx) noexcept {
        return str.empty() ? count + 1
                           : split(str, delimiter, __UTL forward<F>(predicate), count + 1);
    }

    template <typename F>
    __UTL_HIDE_FROM_ABI static inline constexpr size_t split1(
        view_type str, view_type delimiter, F&& predicate, size_t count, size_t idx) noexcept {
        return !predicate(str.substr(0, idx))
            ? count + 1
            : split2(remove_prefix(str, __UTL add_sat(idx, delimiter.size())), delimiter,
                  __UTL forward<F>(predicate), count, idx);
    }

    template <typename F>
    __UTL_HIDE_FROM_ABI static inline constexpr size_t split(
        view_type str, view_type delimiter, F&& predicate, size_t count = 0) noexcept {
        return split1(str, delimiter, __UTL forward<F>(predicate), count, str.find(delimiter));
    }

    template <typename F>
    __UTL_HIDE_FROM_ABI static inline constexpr size_t split2(
        view_type str, CharType delimiter, F&& predicate, size_t count, size_t idx) noexcept {
        return str.empty() ? count + 1
                           : split(str, delimiter, __UTL forward<F>(predicate), count + 1);
    }

    template <typename F>
    __UTL_HIDE_FROM_ABI static inline constexpr size_t split1(
        view_type str, CharType delimiter, F&& predicate, size_t count, size_t idx) noexcept {
        return !predicate(str.substr(0, idx))
            ? count + 1
            : split2(remove_prefix(str, __UTL add_sat<size_t>(idx, 1)), delimiter,
                  __UTL forward<F>(predicate), count, idx);
    }

    template <typename F>
    __UTL_HIDE_FROM_ABI static inline constexpr size_t split(
        view_type str, CharType delimiter, F&& predicate, size_t count = 0) noexcept {
        return split1(str, delimiter, __UTL forward<F>(predicate), count, str.find(delimiter));
    }

public:
    template <typename F>
    __UTL_HIDE_FROM_ABI inline constexpr explicit split_while_impl(
        view_type str, view_type delimiter, F&& predicate) noexcept
        : count(split(str, delimiter, __UTL forward<F>(predicate))) {}

    template <typename F>
    __UTL_HIDE_FROM_ABI inline constexpr explicit split_while_impl(
        view_type str, CharType delimiter, F&& predicate) noexcept
        : count(split(str, delimiter, __UTL forward<F>(predicate))) {}

    size_t count;
};

template <typename CharType, typename Traits, typename F>
__UTL_HIDE_FROM_ABI inline constexpr size_t split_while(basic_string_view<CharType, Traits> str,
    basic_string_view<CharType, Traits> delimiter, F&& predicate) {
    return split_while_impl<CharType, Traits>(str, delimiter, __UTL forward<F>(predicate)).count;
}

template <typename CharType, typename Traits, typename F>
__UTL_HIDE_FROM_ABI inline constexpr size_t split_while(
    basic_string_view<CharType, Traits> str, CharType delimiter, F&& predicate) {
    return split_while_impl<CharType, Traits>(str, delimiter, __UTL forward<F>(predicate)).count;
}

} // namespace compile_time

namespace runtime {
template <typename CharType, typename Traits, typename F>
__UTL_HIDE_FROM_ABI inline size_t split_all(basic_string_view<CharType, Traits> str,
    basic_string_view<CharType, Traits> delimiter, F&& predicate) {
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
__UTL_HIDE_FROM_ABI inline size_t split_while(basic_string_view<CharType, Traits> str,
    basic_string_view<CharType, Traits> delimiter, F&& predicate) {
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
    basic_string_view<CharType, Traits> str, CharType delimiter, F&& predicate) {
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
    basic_string_view<CharType, Traits> str, CharType delimiter, F&& predicate) {
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

UTL_NAMESPACE_END
