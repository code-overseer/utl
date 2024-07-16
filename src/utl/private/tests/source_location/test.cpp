
// Copyright 2023-2024 Bryan Wong

#include "utl/source_location/utl_source_location.h"

#include <cstdio>
#if UTL_COMPILER_SUPPORTS_SOURCE_LOCATION
void func(utl::source_location l = utl::source_location::current()) noexcept {
    puts(l.function_name());
    puts(l.file_name());
}
#else
void inner_func(utl::source_location l) noexcept {
    puts(l.function_name());
    puts(l.file_name());
    puts(__builtin_FUNCTION());
    puts(__builtin_FILE());
    printf("%d\n", __builtin_LINE());
}

void func() noexcept {
    inner_func(UTL_SOURCE_LOCATION());
}

#endif
