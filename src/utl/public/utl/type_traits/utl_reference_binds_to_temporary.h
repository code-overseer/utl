// Copyright 2023-2024 Bryan Wong

#pragma once

#if __UTL_SHOULD_USE_BUILTIN(reference_binds_to_temporary)
#  define UTL_BUILTIN_reference_binds_to_temporary(...) __reference_binds_to_temporary(__VA_ARGS__)
#endif // __UTL_SHOULD_USE_BUILTIN(reference_constructs_from_temporary)
