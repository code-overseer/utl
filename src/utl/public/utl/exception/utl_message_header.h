// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_config.h"

#include "utl/memory/utl_allocator_traits.h"
#include "utl/string/utl_libc.h"
#include "utl/utility/utl_move.h"

#include <cstring>
#include <new>

UTL_NAMESPACE_BEGIN

namespace exceptions {

class message_header {
public:
    template <typename Alloc>
    UTL_ATTRIBUTE(NODISCARD) static message_header* clone(message_header const& src, Alloc const& alloc) {
        constexpr auto header_size = sizeof(message_header);
        auto const buffer_size = src.size() + 1;
        auto const count = (header_size + buffer_size - 1) / buffer_size;
        auto ptr = allocator_traits<Alloc>::allocate(alloc, count);

        auto raw_bytes = ::new (ptr) unsigned char[count * header_size];
        auto header = ::new (raw_bytes) message_header(src.location(), src.size());
        auto str = ::new (raw_bytes + header_size) char[buffer_size];
        libc::unsafe::memcpy(str, src.message(), libc::element_count_t(buffer_size));

        return header;
    }
    constexpr message_header(source_location location, size_t size)
        : location_(UTL_SCOPE move(location))
        , size_(size) {}
    message_header(message_header const&) = delete;
    message_header& operator=(message_header const&) = delete;

    UTL_ATTRIBUTES(NODISCARD, CONST)
    constexpr UTL_SCOPE source_location const& location() const noexcept { return location_; }
    UTL_ATTRIBUTES(NODISCARD, PURE) char const* message() const noexcept {
#ifdef UTL_BUILTIN_launder
        return UTL_BUILTIN_launder(reinterpret_cast<char const*>(this) + sizeof(*this));
#else
        return reinterpret_cast<char const*>(this) + sizeof(*this);
#endif
    }
    UTL_ATTRIBUTES(NODISCARD, PURE) constexpr size_t size() const noexcept { return size_; }

private:
    UTL_SCOPE source_location location_;
    message_header* next_ = nullptr;
    size_t size_ = 0;

    friend void set_next(message_header& h, message_header* value) noexcept { h.next_ = value; }

    friend message_header* next(message_header const& h) noexcept { return h.next_; }
};

} // namespace exceptions

UTL_NAMESPACE_END
