// Copyright 2023-2024 Bryan Wong

#include "utl/type_traits/utl_common.h"
#include "utl/type_traits/utl_is_void.h"

UTL_NAMESPACE_BEGIN

template <typename T, typename... Ts>
struct UTL_PUBLIC_TEMPLATE undefined_trait {
private:
    static_assert(!is_void<T>::value && sizeof(T) == 0,
        "Undefined trait, use preprocessor constant"
        " `UTL_TRAIT_SUPPORTED_<trait_name>` to check for trait support. The"
        " constant will NOT be defined for unsupported traits, defined as `1` for supported "
        "traits");

public:
    static constexpr bool value = false;
};

UTL_NAMESPACE_END
