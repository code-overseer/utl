// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/filesystem/utl_permissions.h"
#include "utl/filesystem/utl_platform.h"
#include "utl/tempus/utl_clock.h"

__UFS_NAMESPACE_BEGIN

struct __UTL_ABI_PUBLIC file_status {
    uintmax_t file_size;
    tempus::time_point<file_clock> created = 0;
    tempus::time_point<file_clock> last_accessed = 0;
    tempus::time_point<file_clock> last_modified = 0;
    uintmax_t hard_links = 0;
    perms perimissions = perms::invalid;
    file_type type = file_type::invalid;
};

__UFS_NAMESPACE_END