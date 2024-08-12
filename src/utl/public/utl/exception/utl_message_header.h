// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

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

/**
 * @brief Represents a message header in a doubly linked list node stack implementation.
 *
 * This final class implements a reference-counted message header used in a doubly
 * linked list. The tail is the owner of the linked list, essentially the first element
 * on the stack. New elements are pushed onto the head. The class also provides methods
 * to manage memory allocation, access message details, and handle linked list operations.
 */
class __UTL_ABI_PUBLIC message_header final : reference_count<message_header> {
public:
    // Deleted new and delete operators to prevent direct allocation
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

    // Deleted copy constructor and copy assignment operator
    message_header(message_header const&) = delete;
    message_header& operator=(message_header const&) = delete;

    /**
     * @brief Retrieves the source location the message was created
     *
     * This function returns a constant reference to the source location of the message header.
     *
     * @return The constant reference to the source location.
     */
    UTL_ATTRIBUTES(NODISCARD, CONST) __UTL_HIDE_FROM_ABI constexpr UTL_SCOPE source_location const&
    location() const noexcept UTL_LIFETIMEBOUND {
        return location_;
    }

    /**
     * @brief Retrieves the message associated with the message header.
     *
     * This function returns a pointer to the message string associated with the message header.
     *
     * @return The constant pointer to the message string.
     */
    UTL_ATTRIBUTES(NODISCARD, PURE) __UTL_HIDE_FROM_ABI char const* message() const noexcept UTL_ATTRIBUTE(
        LIFETIMEBOUND) {
        return reinterpret_cast<char const*>(this) + sizeof(*this);
    }

    /**
     * @brief Retrieves the size of the message string.
     *
     * This function returns the size of the message string.
     *
     * @return The size of the message string.
     */
    UTL_ATTRIBUTES(NODISCARD, PURE) __UTL_HIDE_FROM_ABI constexpr size_t size() const noexcept {
        return size_;
    }

    /**
     * @brief Creates a new message.
     *
     * This static function creates a new message with the provided message format and
     * arguments.
     *
     * @param fmt The message format object containing the format string.
     * @param ... Variadic arguments for the message format.
     * @return A pointer to the header of the newly created message.
     * @throws std::bad_alloc on memory allocation failure.
     */
    UTL_NODISCARD static message_header* create(message_format fmt, ...) UTL_THROWS {
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

    /**
     * @brief Constructs a message header with the given source location and size.
     *
     * This constructor initializes the message header with the provided source location and size.
     *
     * @param location The source location of the message header.
     * @param size The size of the message header.
     */
    __UTL_HIDE_FROM_ABI constexpr message_header(source_location&& location, size_t size) noexcept
        : location_(UTL_SCOPE move(location))
        , size_(size) {}

    /**
     * @brief Sets the next message header in the linked list.
     *
     * This friend function sets the next message header in the linked list and updates
     * the previous pointer of the next message header.
     *
     * @param h The current message header.
     * @param value The next message header to be set.
     */
    __UTL_HIDE_FROM_ABI friend void set_next(message_header& h, message_header* value) noexcept {
        if (value != nullptr) {
            value->prev_ = UTL_SCOPE addressof(h);
        }

        h.next_ = value;
    }

    /**
     * @brief Retrieves the next message header in the linked list.
     *
     * This friend function returns a pointer to the next message header in the linked list.
     *
     * @param h The current message header.
     * @return A pointer to the next message header.
     */
    UTL_ATTRIBUTES(NODISCARD, PURE) __UTL_HIDE_FROM_ABI friend message_header* next(
        message_header const& h) noexcept {
        return h.next_;
    }

    /**
     * @brief Destroys the message given it's header and all messages above it.
     *
     * @param ptr The initial pointer to the message to be destroyed.
     */
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
