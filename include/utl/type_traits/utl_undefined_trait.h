// Copyright 2023-2024 Bryan Wong

#include "utl/type_traits/utl_common.h"

UTL_NAMESPACE_BEGIN

using size_t = decltype(sizeof(0));

template<typename... Ts>
struct undefined_trait {
private:
    template<typename U>
    static UTL_CONSTEVAL_CXX20 size_t sum_of_sizes() noexcept {
        return sizeof(U);
    }

    template<typename U0, typename U1, typename... Us>
    static UTL_CONSTEVAL_CXX20 size_t sum_of_sizes() noexcept {
        return sizeof(U0) + sum_of_sizes<U1, Us...>();
    }

    static constexpr bool always_false = sizeof...(Ts) + sum_of_sizes<int, Ts...>() == 0;

    static_assert(always_false, "Undefined trait, use preprocessor constant"
        " `UTL_TRAIT_SUPPORTED_<trait_name>` to check for trait support. The"
        " constant will NOT be defined for unsupported traits, defined as `1` for supported traits");
};

UTL_NAMESPACE_END
