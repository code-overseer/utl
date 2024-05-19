// Copyright 2023-2024 Bryan Wong

#include "utl/memory/utl_allocator.h"

#include <vector>
void func() {
    utl::allocator<int> a;
    a.deallocate(a.allocate(1), 1);
}

void func(std::vector<int, utl::allocator<int>> v) {
    v.reserve(100);
    std::vector<int, utl::allocator<int>> u(v);
}
