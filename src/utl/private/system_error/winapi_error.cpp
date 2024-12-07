// Copyright 2023-2024 Bryan Wong

#include "utl/system_error/utl_system_error.h"
#if UTL_TARGET_MICROSOFT

#  define NOMINMAX
#  define NODRAWTEXT
#  define NOGDI
#  define NOBITMAP
#  define NOMCX
#  define NOSERVICE
#  define NOHELP
#  ifndef WIN32_LEAN_AND_MEAN
#    define WIN32_LEAN_AND_MEAN
#  endif

#  include <Windows.h>
#  include <Winerror.h>

#  include <cstdio>
#  include <cstdlib>
#  include <cstring>

UTL_NAMESPACE_BEGIN

namespace hresult_error {
static size_t message(DWORD code, char* buffer, size_t size) noexcept {
    static constexpr DWORD local_size = 2048;
    char local_buffer[local_size];

    char* arg_ptr = local_buffer;
    auto result = FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        nullptr, code, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), &arg_ptr, local_size, nullptr);
    if (result == 0) UTL_ATTRIBUTE(UNLIKELY) {
        result = FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, nullptr,
            result, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), &arg_ptr, local_size, nullptr);
        if (result == 0) UTL_ATTRIBUTE(UNLIKELY) {
            abort();
        }
    }

    if (result > 3 && local_buffer[result - 2] == '\r' && local_buffer[result - 1] == '\n') {
        local_buffer[result - 2] = '\0';
    }

    return snprintf(buffer, size, "%s", local_buffer);
}
} // namespace hresult_error

class __UTL_ABI_PRIVATE hresult_error_category final : public error_category {
public:
    virtual char const* name() const noexcept final { return "HRESULT"; }
    virtual error_condition default_error_condition(int code) const noexcept final {
        return error_condition(code, hresult_category());
    }

    virtual size_t message(int code, char* buffer, size_t size) const noexcept final {
        return hresult_error::message((DWORD)code, buffer, size);
    }
};

error_category const& hresult_category() noexcept {
    union non_destructible {
        constexpr explicit non_destructible() noexcept : category{} {}
        ~non_destructible() {}
        hresult_error_category category;
    };
#  if UTL_CXX20
    static constinit non_destructible instance;
#  else
    static non_destructible const instance;
#  endif
    return instance.category;
}

UTL_NAMESPACE_END

#endif
