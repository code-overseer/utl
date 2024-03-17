// Copyright 2023-2024 Bryan Wong

#pragma once

#ifndef UTL_DISABLE_BUILTIN_reference_binds_to_temporary
#  define UTL_DISABLE_BUILTIN_reference_binds_to_temporary 0
#endif

#if UTL_SHOULD_USE_BUILTIN(reference_binds_to_temporary)
#  define UTL_BUILTIN_reference_binds_to_temporary(...) __reference_binds_to_temporary(__VA_ARGS__)
#endif // UTL_SHOULD_USE_BUILTIN(reference_constructs_from_temporary)
