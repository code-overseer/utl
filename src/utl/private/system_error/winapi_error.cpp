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

namespace winapi_error {
namespace {
size_t message(DWORD code, char* buffer, size_t size) noexcept {
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

class map_result {
public:
    inline constexpr map_result(errc val) noexcept : value_{val}, has_value{true} {}
    inline constexpr map_result() noexcept : value_{}, has_value{false} {}
    inline constexpr explicit operator bool() const noexcept { return has_value; }
    inline constexpr errc value() const noexcept { return value_; }

private:
    errc value_;
    bool has_value;
};

constexpr map_result map_to_generic(int code) noexcept {
    switch (code) {
    // table of Windows/Posix pairs
    case ERROR_INVALID_FUNCTION:
        return errc::function_not_supported;
    case ERROR_FILE_NOT_FOUND:
        return errc::no_such_file_or_directory;
    case ERROR_PATH_NOT_FOUND:
        return errc::no_such_file_or_directory;
    case ERROR_TOO_MANY_OPEN_FILES:
        return errc::too_many_files_open;
    case ERROR_ACCESS_DENIED:
        return errc::permission_denied;
    case ERROR_INVALID_HANDLE:
        return errc::invalid_argument;
    case ERROR_NOT_ENOUGH_MEMORY:
        return errc::not_enough_memory;
    case ERROR_INVALID_ACCESS:
        return errc::permission_denied;
    case ERROR_OUTOFMEMORY:
        return errc::not_enough_memory;
    case ERROR_INVALID_DRIVE:
        return errc::no_such_device;
    case ERROR_CURRENT_DIRECTORY:
        return errc::permission_denied;
    case ERROR_NOT_SAME_DEVICE:
        return errc::cross_device_link;
    case ERROR_WRITE_PROTECT:
        return errc::permission_denied;
    case ERROR_BAD_UNIT:
        return errc::no_such_device;
    case ERROR_NOT_READY:
        return errc::resource_unavailable_try_again;
    case ERROR_SEEK:
        return errc::io_error;
    case ERROR_WRITE_FAULT:
        return errc::io_error;
    case ERROR_READ_FAULT:
        return errc::io_error;
    case ERROR_SHARING_VIOLATION:
        return errc::permission_denied;
    case ERROR_LOCK_VIOLATION:
        return errc::no_lock_available;
    case ERROR_HANDLE_DISK_FULL:
        return errc::no_space_on_device;
    case ERROR_NOT_SUPPORTED:
        return errc::not_supported;
    case ERROR_BAD_NETPATH:
        return errc::no_such_file_or_directory;
    case ERROR_DEV_NOT_EXIST:
        return errc::no_such_device;
    case ERROR_BAD_NET_NAME:
        return errc::no_such_file_or_directory;
    case ERROR_FILE_EXISTS:
        return errc::file_exists;
    case ERROR_CANNOT_MAKE:
        return errc::permission_denied;
    case ERROR_INVALID_PARAMETER:
        return errc::invalid_argument;
    case ERROR_BROKEN_PIPE:
        return errc::broken_pipe;
    case ERROR_OPEN_FAILED:
        return errc::io_error;
    case ERROR_BUFFER_OVERFLOW:
        return errc::filename_too_long;
    case ERROR_DISK_FULL:
        return errc::no_space_on_device;
    case ERROR_SEM_TIMEOUT:
        return errc::timed_out;
    case ERROR_INVALID_NAME:
        return errc::no_such_file_or_directory;
    case ERROR_NEGATIVE_SEEK:
        return errc::invalid_argument;
    case ERROR_BUSY_DRIVE:
        return errc::device_or_resource_busy;
    case ERROR_DIR_NOT_EMPTY:
        return errc::directory_not_empty;
    case ERROR_BUSY:
        return errc::device_or_resource_busy;
    case ERROR_ALREADY_EXISTS:
        return errc::file_exists;
    case ERROR_FILENAME_EXCED_RANGE:
        return errc::filename_too_long;
    case ERROR_LOCKED:
        return errc::no_lock_available;
    case WAIT_TIMEOUT:
        return errc::timed_out;
    case ERROR_DIRECTORY:
        return errc::not_a_directory;
    case ERROR_OPERATION_ABORTED:
        return errc::operation_canceled;
    case ERROR_NOACCESS:
        return errc::permission_denied;
    case ERROR_CANTOPEN:
        return errc::io_error;
    case ERROR_CANTREAD:
        return errc::io_error;
    case ERROR_CANTWRITE:
        return errc::io_error;
    case ERROR_RETRY:
        return errc::resource_unavailable_try_again;
    case ERROR_TIMEOUT:
        return errc::timed_out;
    case ERROR_OPEN_FILES:
        return errc::device_or_resource_busy;
    case ERROR_DEVICE_IN_USE:
        return errc::device_or_resource_busy;
    case ERROR_REPARSE_TAG_INVALID:
        return errc::invalid_argument;
    case WSAEINTR:
        return errc::interrupted;
    case WSAEBADF:
        return errc::bad_file_descriptor;
    case WSAEACCES:
        return errc::permission_denied;
    case WSAEFAULT:
        return errc::bad_address;
    case WSAEINVAL:
        return errc::invalid_argument;
    case WSAEMFILE:
        return errc::too_many_files_open;
    case WSAEWOULDBLOCK:
        return errc::operation_would_block;
    case WSAEINPROGRESS:
        return errc::operation_in_progress;
    case WSAEALREADY:
        return errc::connection_already_in_progress;
    case WSAENOTSOCK:
        return errc::not_a_socket;
    case WSAEDESTADDRREQ:
        return errc::destination_address_required;
    case WSAEMSGSIZE:
        return errc::message_size;
    case WSAEPROTOTYPE:
        return errc::wrong_protocol_type;
    case WSAENOPROTOOPT:
        return errc::no_protocol_option;
    case WSAEPROTONOSUPPORT:
        return errc::protocol_not_supported;
    case WSAEOPNOTSUPP:
        return errc::operation_not_supported;
    case WSAEAFNOSUPPORT:
        return errc::address_family_not_supported;
    case WSAEADDRINUSE:
        return errc::address_in_use;
    case WSAEADDRNOTAVAIL:
        return errc::address_not_available;
    case WSAENETDOWN:
        return errc::network_down;
    case WSAENETUNREACH:
        return errc::network_unreachable;
    case WSAENETRESET:
        return errc::network_reset;
    case WSAECONNABORTED:
        return errc::connection_aborted;
    case WSAECONNRESET:
        return errc::connection_reset;
    case WSAENOBUFS:
        return errc::no_buffer_space;
    case WSAEISCONN:
        return errc::already_connected;
    case WSAENOTCONN:
        return errc::not_connected;
    case WSAETIMEDOUT:
        return errc::timed_out;
    case WSAECONNREFUSED:
        return errc::connection_refused;
    case WSAENAMETOOLONG:
        return errc::filename_too_long;
    case WSAEHOSTUNREACH:
        return errc::host_unreachable;
    default:
        return map_result{};
    }
}

} // namespace
} // namespace winapi_error

class __UTL_ABI_PRIVATE winapi_error_category final : public error_category {
public:
    virtual char const* name() const noexcept final { return "system"; }
    virtual error_condition default_error_condition(int code) const noexcept final {
        auto const result = map_to_generic(code);
        if (result) {
            return error_condition{result.value(), generic_category()};
        }

        return error_condition{code, system_category()};
    }

    virtual size_t message(int code, char* buffer, size_t size) const noexcept final {
        auto const result = map_to_generic(code);
        if (result) UTL_ATTRIBUTE(LIKELY) {
            return generic_category().message(code, buffer, size);
        }

        return winapi_error::message((DWORD)code, buffer, size);
    }
};

error_category const& system_category() noexcept {
    union non_destructible {
        constexpr explicit non_destructible() noexcept : category{} {}
        ~non_destructible() {}
        winapi_error_category category;
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
