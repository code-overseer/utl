// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

#include "utl/exception/utl_program_exception.h"

#ifndef UTL_ALLOCATOR_PRIVATE_HEADER_GUARD
#  error "Private header accessed"
#endif

UTL_NAMESPACE_BEGIN

template <typename T>
inline UTL_CONSTEXPR_CXX20 auto allocator<T>::allocate(size_type count) UTL_THROWS -> pointer {
    UTL_THROW_IF(count > memory::max_size<T>::value,
        bad_array_new_length(
            UTL_MESSAGE_FORMAT("[UTL] allocation operation failed, Reason=[element count "
                               "limit exceeded], count=[%zu], limit=[%zu]"),
            count, memory::max_size<T>::value));

    return memory::allocate<value_type>(count);
}

template <typename T>
inline UTL_CONSTEXPR_CXX20 void allocator<T>::deallocate(pointer pointer, size_type count) noexcept {
    memory::deallocate<value_type>(pointer, count);
}
UTL_NAMESPACE_END
