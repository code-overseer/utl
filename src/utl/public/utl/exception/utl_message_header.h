// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/preprocessor/utl_config.h"

#include "utl/exception/utl_message_format.h"
#include "utl/memory/utl_allocator_decl.h"
#include "utl/memory/utl_reference_count.h"
#include "utl/string/utl_libc.h"
#include "utl/utility/utl_move.h"

#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <new>

UTL_NAMESPACE_BEGIN

namespace exceptions {

class message_header final : reference_count<message_header> {
public:
    static void* operator new(size_t) = delete;
    static void* operator new[](size_t) = delete;
    static void operator delete(void*) = delete;
    static void operator delete[](void*) = delete;
    static void operator delete(void*, size_t) = delete;
    static void operator delete[](void*, size_t) = delete;
#if UTL_SUPPORTS_ALIGNED_ALLOCATION
    static void* operator new(size_t, align_val_t) = delete;
    static void* operator new[](size_t, align_val_t) = delete;
    static void operator delete(void*, size_t, align_val_t) = delete;
    static void operator delete[](void*, size_t, align_val_t) = delete;
#endif

    message_header(message_header const&) = delete;
    message_header& operator=(message_header const&) = delete;

    UTL_ATTRIBUTES(NODISCARD, CONST)
    constexpr UTL_SCOPE source_location const& location() const noexcept UTL_ATTRIBUTE(
        LIFETIMEBOUND) {
        return location_;
    }

    UTL_ATTRIBUTES(NODISCARD, PURE) char const* message() const noexcept UTL_ATTRIBUTE(LIFETIMEBOUND) {
#ifdef UTL_BUILTIN_launder
        return UTL_BUILTIN_launder(reinterpret_cast<char const*>(this) + sizeof(*this));
#else
        return reinterpret_cast<char const*>(this) + sizeof(*this);
#endif
    }

    UTL_ATTRIBUTES(NODISCARD, PURE) constexpr size_t size() const noexcept { return size_; }

    static message_header* create(message_format fmt, ...) UTL_THROWS {
        static constexpr auto header_size = sizeof(message_header);
        va_list args1;
        va_list args2;

        va_start(args1, fmt);
        va_copy(args2, args1);
        auto const str_size = vsnprintf(nullptr, 0, fmt.format, args1);
        va_end(args1);
        auto const buffer_size = str_size + 1;
        auto const count = (buffer_size + header_size - 1) / header_size + 1;

        UTL_TRY {
            auto ptr = memory::allocate<message_header>(count); // throwable

            // restart lifetime (null operation)
            auto raw_bytes = ::new (ptr) unsigned char[count * header_size];
            // start lifetime of header
            auto header = ::new (raw_bytes) message_header(UTL_SCOPE move(fmt.location), str_size);
            // start lifetime of string (null operation)
            auto str = ::new (raw_bytes + header_size) char[buffer_size];
            vsnprintf(str, buffer_size, fmt.format, args2);
            va_end(args2);

            UTL_ASSERT(str == header->message());

            return header;
        } UTL_CATCH(...) {
            va_end(args2);
            UTL_RETHROW();
        }
    }

private:
    UTL_SCOPE source_location location_;
    message_header* next_ = nullptr;
    message_header* prev_ = nullptr;
    size_t size_ = 0;

    constexpr message_header(source_location&& location, size_t size)
        : location_(UTL_SCOPE move(location))
        , size_(size) {}

    friend void set_next(message_header& h, message_header* value) noexcept {
        if (value != nullptr) {
            value->prev_ = UTL_SCOPE addressof(h);
        }

        h.next_ = value;
    }

    UTL_ATTRIBUTES(NODISCARD, PURE) friend message_header* next(message_header const& h) noexcept {
        return h.next_;
    }

    friend void destroy(message_header* ptr) noexcept {
        UTL_ASSERT(ptr != nullptr);
        static constexpr size_t header_size = sizeof(message_header);
        do {
            auto to_delete = ptr;
            ptr = ptr->prev_;

            auto const buffer_size = to_delete->size() + 1;
            auto const count = (buffer_size + header_size - 1) / header_size + 1;
            memory::deallocate<message_header>(to_delete, count);
        } while (ptr != nullptr);
    }
};

} // namespace exceptions

UTL_NAMESPACE_END
