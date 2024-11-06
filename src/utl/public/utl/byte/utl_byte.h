
#include "utl/utl_config.h"

#if UTL_CXX17

#  include <stddef.h>

UTL_NAMESPACE_BEGIN
using byte = ::std::byte;
UTL_NAMESPACE_END

#else

#  include "utl/concepts/utl_integral.h"
#  include "utl/type_traits/utl_enable_if.h"
#  include "utl/type_traits/utl_is_integral.h"

UTL_NAMESPACE_BEGIN
/**
 * There is no way around this unfortunately, this may break between 14 to 17
 * But the standard explicitly calls out std::byte to be able to alias with
 * anything not an enum of unsigned char underlying type. Thus, defining a custom
 * byte would not result in equivalent behaviour with std::byte
 *
 * https://stackoverflow.com/questions/52239039/how-to-have-a-type-which-is-like-unsigned-char-but-not-allowed-to-alias
 */
using byte = unsigned char;
/**
 * ADL doesn't work because we don't have a type
 */
template <UTL_CONCEPT_CXX20(integral) Int>
constexpr UTL_ENABLE_IF_CXX11(Int, UTL_TRAIT_is_integral(Int)) to_integer(byte b) noexcept {
    return static_cast<Int>(b);
}
UTL_NAMESPACE_END

#endif
