// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/filesystem/utl_platform.h"

__UFS_NAMESPACE_BEGIN

enum perms {
    none = 0,
    owner_read = 0400,
    owner_write = 0200,
    owner_exec = 0100,
    owner_all = 0700,
    group_read = 040,
    group_write = 020,
    group_exec = 010,
    group_all = 070,
    others_read = 04,
    others_write = 02,
    others_exec = 01,
    others_all = 0777,
    set_uid = 04000,
    set_gid = 02000,
    sticky_bit = 01000,
    mask = 07777,
    invalid = 0xffff
};

__UFS_NAMESPACE_END
