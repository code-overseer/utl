// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

#if UTL_TARGET_APPLE
#  include "darwin/utl_futex.h"
#endif

UTL_NAMESPACE_BEGIN

namespace platform {
template <typename T>
class futex {

public:
    constexpr explicit futex(T& t) : address(__UTL addressof(t)) {}
    futex(futex const&) = delete;
    futex(futex&&) = delete;
    futex& operator=(futex const&) = delete;
    futex& operator=(futex&&) = delete;
    UTL_CONSTEXPR_CXX20 ~futex() noexcept = default;

    template <typename R, typename P>
    void wait(T value, chrono::duration<R, P>) noexcept {}

    void wait(T value) noexcept;

private:
    T* address;
};
} // namespace platform

UTL_NAMESPACE_END
