

#include "utl/preprocessor/utl_namespace.h"

UTL_NAMESPACE_BEGIN

template <typename T>
struct remove_all_pointers {
    using type = T;
};
template <typename T>
struct remove_all_pointers<T*> : remove_all_pointers<T> {};

template <typename T>
using remove_all_pointers = typename remove_all_pointers<T>::type;

UTL_NAMESPACE_END
