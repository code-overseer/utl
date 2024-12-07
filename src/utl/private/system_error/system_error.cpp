// Copyright 2023-2024 Bryan Wong

#include "utl/system_error/utl_system_error.h"
#include "utl_elast.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>

UTL_NAMESPACE_BEGIN

static size_t error_message(int code, char* buffer, size_t size) noexcept;

#if UTL_TARGET_MICROSOFT

static size_t error_message(int code, char* buffer, size_t size) noexcept {
#  ifdef __UTL_ELAST
    if (code > __UTL_ELAST) {
        return snprintf(buffer, size, "Unspecified generic_category error");
    }
#  endif // __UTL_ELAST

    static constexpr size_t local_size = 1024;
    char local_buffer[local_size];
    if (strerror_s(local_buffer, sizeof(local_buffer), code) == 0) {
        return snprintf(buffer, size, "%s", local_buffer);
    }

    return snprintf(buffer, size, "Unknown error %d", code);
}

#else

// GNU variant
UTL_ATTRIBUTE(MAYBE_UNUSED) static char const* handle_strerror(char* result, char* buffer) noexcept {
    return result;
}

// XSI variant
UTL_ATTRIBUTE(MAYBE_UNUSED) static char const* handle_strerror(int result, char* buffer) noexcept {
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

static size_t error_message(int code, char* buffer, size_t size) noexcept {
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

#endif

class __UTL_ABI_PRIVATE generic_error_category final : public error_category {
public:
    virtual char const* name() const noexcept final { return "generic"; }
    virtual size_t message(int code, char* buffer, size_t size) const noexcept final {
        return error_message(code, buffer, size);
    }
};

class __UTL_ABI_PRIVATE system_error_category final : public error_category {
public:
    virtual char const* name() const noexcept final { return "system"; }
    virtual error_condition default_error_condition(int code) const noexcept final {
#ifdef __UTL_ELAST
        if (code > __UTL_ELAST) {
            return error_condition(code, system_category());
        }
#endif // __UTL_ELAST
        return error_condition(code, generic_category());
    }

    virtual size_t message(int code, char* buffer, size_t size) const noexcept final {
#ifdef __UTL_ELAST
        if (code > __UTL_ELAST) {
            return snprintf(buffer, size, "Unspecified generic_category error");
        }
#endif // __UTL_ELAST
        return error_message(code, buffer, size);
    }
};

error_category const& generic_category() noexcept {
    union non_destructible {
        constexpr explicit non_destructible() noexcept : category{} {}
        ~non_destructible() {}
        generic_error_category category;
    };
#if UTL_CXX20
    static constinit non_destructible instance;
#else
    static non_destructible const instance;
#endif
    return instance.category;
}

error_category const& system_category() noexcept {
    union non_destructible {
        constexpr explicit non_destructible() noexcept : category{} {}
        ~non_destructible() {}
        system_error_category category;
    };
#if UTL_CXX20
    static constinit non_destructible instance;
#else
    static non_destructible const instance;
#endif
    return instance.category;
}

UTL_NAMESPACE_END
