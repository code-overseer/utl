// Copyright 2023-2024 Bryan Wong

#include "utl/scope/utl_scope_exit.h"
#include "utl/scope/utl_scope_fail.h"
#include "utl/scope/utl_scope_success.h"

struct foo {
    void operator()() const;
    foo(foo&&) = delete;
};

template class utl::scope_exit<foo>;
