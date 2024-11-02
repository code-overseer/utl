// Copyright 2023-2024 Bryan Wong

#include "utl/string/utl_basic_short_string.h"
#include "utl/string/utl_basic_string_view.h"
#include "utl/string/utl_basic_zstring_view.h"
#include "utl/string/utl_string_utils.h"

static_assert(utl::string_view("  \tabc ").find_last_not_of(" ") == 5, "");
static_assert(utl::strip(utl::string_view("  \tabc ")) == utl::string_view("abc"), "");

int comparable(utl::string s) {
    if (s != "hello") {
        s = "hello";
    }

    return s.compare("hello") + s.rfind("jasl") + s.find("OIKAOSDJMI");
}

void iterable(utl::string s) {
    for (auto c : s) {
        if (c == '\0') {
            break;
        }
    }
}

utl::string concatenatable(utl::string s) {

    return s.c_str() + s + s.c_str();
}

utl::string erasable(utl::string s) {
    erase(s, 'u');
    erase_if(s, [](auto c) { return c == 'y'; });
    return s;
}

// Ensure char_traits specialization is implemented
static_assert(!utl::char_traits<char>::lt(-1, 1), "Invalid");

template <typename T>
class small_ptr {
public:
    using value_type = T;
    using difference_type = int16_t;
    using pointer = small_ptr;
    using reference = T&;
    using iterator_category = utl::random_access_iterator_tag;

private:
    uint16_t offset;
};

template <typename T>
class small_iter_allocator {
public:
    using value_type = T;
    using pointer = small_ptr<T>;
    using size_type = uint16_t;
    using difference_type = int16_t;

    small_iter_allocator() noexcept {}

    template <class U>
    small_iter_allocator(small_iter_allocator<U>) noexcept {}

    T* allocate(size_t n);
    void deallocate(T* p, size_t);

    friend bool operator==(small_iter_allocator, small_iter_allocator) { return true; }
    friend bool operator!=(small_iter_allocator, small_iter_allocator) { return false; }
};

template <typename CharT>
using small_string = utl::basic_string<CharT, utl::char_traits<CharT>, small_iter_allocator<CharT>>;

static_assert(sizeof(small_string<char>) == 6, "");
static_assert(sizeof(utl::string) == 32, "");
