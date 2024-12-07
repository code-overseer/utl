// Copyright 2023-2024 Bryan Wong

#include "utl/system_error/utl_system_error.h"
#if !UTL_TARGET_MICROSOFT

#  include <errno.h>

#  include <cstdio>
#  include <cstdlib>
#  include <cstring>

#  if defined(ELAST)
#    define __UTL_ELAST ELAST
#  elif defined(__LLVM_LIBC__)
// No __UTL_ELAST needed for LLVM libc
#  elif defined(_NEWLIB_VERSION)
#    define __UTL_ELAST __ELASTERROR
#  elif defined(__NuttX__)
// No __UTL_ELAST needed on NuttX
#  elif defined(__Fuchsia__)
// No __UTL_ELAST needed on Fuchsia
#  elif defined(__wasi__)
// No __UTL_ELAST needed on WASI
#  elif defined(__EMSCRIPTEN__)
// No __UTL_ELAST needed on Emscripten
#  elif defined(__linux__)
#    define __UTL_ELAST 4095
#  elif defined(__APPLE__)
// No __UTL_ELAST needed on Apple
#  elif defined(__MVS__)
#    define __UTL_ELAST 1160
#  elif defined(_AIX)
#    define __UTL_ELAST 127
#  else
#    warning ELAST for this platform not yet implemented
#  endif

UTL_NAMESPACE_BEGIN

namespace posix_error {
namespace {
// GNU variant
UTL_ATTRIBUTE(MAYBE_UNUSED) char const* handle_strerror(char* result, char* buffer) noexcept {
    return result;
}

// XSI variant
UTL_ATTRIBUTE(MAYBE_UNUSED) char const* handle_strerror(int result, char* buffer) noexcept {
    if (result == 0) {
        return buffer;
    }

    // Only handle EINVAL. Other errors abort.
    int const new_errno = result == -1 ? errno : result;
    if (new_errno == EINVAL) {
        return "";
    }

    std::abort();
}

size_t message(int code, char* buffer, size_t size) noexcept {
#  ifdef __UTL_ELAST
    if (code > __UTL_ELAST) {
        return snprintf(buffer, size, "Unspecified generic_category error");
    }
#  endif // __UTL_ELAST

    static constexpr size_t local_size = 1024;
    char local_buffer[local_size];
    int const old_errno = errno;
    char const* msg = handle_strerror(::strerror_r(code, local_buffer, local_size), local_buffer);
    if (!msg[0]) {
        return snprintf(buffer, size, "Unknown error %d", code);
    }

    errno = old_errno;
    return snprintf(buffer, size, "%s", msg);
}
} // namespace
} // namespace posix_error

class __UTL_ABI_PRIVATE posix_error_category final : public error_category {
public:
    virtual char const* name() const noexcept final { return "system"; }
    virtual error_condition default_error_condition(int code) const noexcept final {
#  ifdef __UTL_ELAST
        if (code > __UTL_ELAST) {
            return error_condition(code, system_category());
        }
#  endif // __UTL_ELAST
        return error_condition(code, generic_category());
    }

    virtual size_t message(int code, char* buffer, size_t size) const noexcept final {
#  ifdef __UTL_ELAST
        if (code > __UTL_ELAST) {
            return snprintf(buffer, size, "Unspecified system_category error");
        }
#  endif // __UTL_ELAST
        return posix_error::message(code, buffer, size);
    }
};

error_category const& system_category() noexcept {
    union non_destructible {
        constexpr explicit non_destructible() noexcept : category{} {}
        ~non_destructible() {}
        posix_error_category category;
    };
#  if UTL_CXX20
    static constinit non_destructible instance;
#  else
    static non_destructible const instance;
#  endif
    return instance.category;
}

UTL_NAMESPACE_END

#  ifdef __UTL_ELAST
#    undef __UTL_ELAST
#  endif

#endif
