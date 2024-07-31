// Copyright 2023-2024 Bryan Wong

#include "utl/string/utl_basic_short_string.h"
#include "utl/string/utl_basic_string_view.h"
#include "utl/string/utl_basic_zstring_view.h"

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

// Ensure char_traits specialization is implemented
static_assert(!utl::char_traits<char>::lt(-1, 1), "Invalid");
