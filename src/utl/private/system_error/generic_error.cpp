// Copyright 2023-2024 Bryan Wong

#include "utl/system_error/utl_errc.h"
#include "utl/system_error/utl_system_error.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>

UTL_NAMESPACE_BEGIN

namespace generic_error {
constexpr char const* message(errc code) noexcept {
    if (code == static_cast<errc>(0)) {
        return "success";
    }
    switch (code) {
    case errc::address_family_not_supported:
        return "address family not supported";
    case errc::address_in_use:
        return "address in use";
    case errc::address_not_available:
        return "address not available";
    case errc::already_connected:
        return "already connected";
    case errc::argument_list_too_long:
        return "argument list too long";
    case errc::argument_out_of_domain:
        return "argument out of domain";
    case errc::bad_address:
        return "bad address";
    case errc::bad_file_descriptor:
        return "bad file descriptor";
    case errc::bad_message:
        return "bad message";
    case errc::broken_pipe:
        return "broken pipe";
    case errc::connection_aborted:
        return "connection aborted";
    case errc::connection_already_in_progress:
        return "connection already in progress";
    case errc::connection_refused:
        return "connection refused";
    case errc::connection_reset:
        return "connection reset";
    case errc::cross_device_link:
        return "cross device link";
    case errc::destination_address_required:
        return "destination address required";
    case errc::device_or_resource_busy:
        return "device or resource busy";
    case errc::directory_not_empty:
        return "directory not empty";
    case errc::executable_format_error:
        return "executable format error";
    case errc::file_exists:
        return "file exists";
    case errc::file_too_large:
        return "file too large";
    case errc::filename_too_long:
        return "filename too long";
    case errc::function_not_supported:
        return "function not supported";
    case errc::host_unreachable:
        return "host unreachable";
    case errc::identifier_removed:
        return "identifier removed";
    case errc::illegal_byte_sequence:
        return "illegal byte sequence";
    case errc::inappropriate_io_control_operation:
        return "inappropriate io control operation";
    case errc::interrupted:
        return "interrupted";
    case errc::invalid_argument:
        return "invalid argument";
    case errc::invalid_seek:
        return "invalid seek";
    case errc::io_error:
        return "io error";
    case errc::is_a_directory:
        return "is a directory";
    case errc::message_size:
        return "message size";
    case errc::network_down:
        return "network down";
    case errc::network_reset:
        return "network reset";
    case errc::network_unreachable:
        return "network unreachable";
    case errc::no_buffer_space:
        return "no buffer space";
    case errc::no_child_process:
        return "no child process";
    case errc::no_link:
        return "no link";
    case errc::no_lock_available:
        return "no lock available";
    case errc::no_message:
        return "no message";
    case errc::no_protocol_option:
        return "no protocol option";
    case errc::no_space_on_device:
        return "no space on device";
    case errc::no_such_device_or_address:
        return "no such device or address";
    case errc::no_such_device:
        return "no such device";
    case errc::no_such_file_or_directory:
        return "no such file or directory";
    case errc::no_such_process:
        return "no such process";
    case errc::not_a_directory:
        return "not a directory";
    case errc::not_a_socket:
        return "not a socket";
    case errc::not_connected:
        return "not connected";
    case errc::not_enough_memory:
        return "not enough memory";
    case errc::not_supported:
        return "not supported";
    case errc::operation_canceled:
        return "operation canceled";
    case errc::operation_in_progress:
        return "operation in progress";
    case errc::operation_not_permitted:
        return "operation not permitted";
    case errc::operation_not_supported:
        return "operation not supported";
    case errc::operation_would_block:
        return "operation would block";
    case errc::owner_dead:
        return "owner dead";
    case errc::permission_denied:
        return "permission denied";
    case errc::protocol_error:
        return "protocol error";
    case errc::protocol_not_supported:
        return "protocol not supported";
    case errc::read_only_file_system:
        return "read only file system";
    case errc::resource_deadlock_would_occur:
        return "resource deadlock would occur";
    case errc::result_out_of_range:
        return "result out of range";
    case errc::state_not_recoverable:
        return "state not recoverable";
    case errc::text_file_busy:
        return "text file busy";
    case errc::timed_out:
        return "timed out";
    case errc::too_many_files_open_in_system:
        return "too many files open in system";
    case errc::too_many_files_open:
        return "too many files open";
    case errc::too_many_links:
        return "too many links";
    case errc::too_many_symbolic_link_levels:
        return "too many symbolic link levels";
    case errc::value_too_large:
        return "value too large";
    case errc::wrong_protocol_type:
        return "wrong protocol type";
    default:
        return "unknown error";
    }
}
} // namespace generic_error

class __UTL_ABI_PRIVATE generic_error_category final : public error_category {
public:
    virtual char const* name() const noexcept final { return "generic"; }
    virtual size_t message(int code, char* buffer, size_t size) const noexcept final {
        return snprintf(buffer, size, "%s", generic_error::message(static_cast<errc>(code)));
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

UTL_NAMESPACE_END
