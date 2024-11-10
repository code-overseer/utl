// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

UTL_NAMESPACE_BEGIN

template <typename CharType, CharType... Vs>
struct literal_sequence {
    static constexpr CharType value[]{Vs..., (CharType)0};
    static constexpr size_t size = sizeof...(Vs);
};

template <typename CharType, CharType... Vs>
constexpr CharType literal_sequence<CharType, Vs...>::value[sizeof...(Vs) + 1];

UTL_NAMESPACE_END
