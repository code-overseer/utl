#pragma once

#include "utl/preprocessor/utl_config.h"

#include "utl/tuple/utl_tuple_fwd.h"

#include "utl/type_traits/utl_copy_cvref.h"

UTL_NAMESPACE_BEGIN

namespace details {
namespace tuple {
template <size_t I, typename T>
auto decl_element() noexcept -> copy_cvref_t<T&&, tuple_element_t<I, T>>;

} // namespace tuple
} // namespace details

UTL_NAMESPACE_END
