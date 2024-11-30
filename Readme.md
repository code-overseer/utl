# Untitled Library (UTL)

Incredibly pointless library. Guranteed to compile at least 3 times slower than STL.

# Status
 * Untested at runtime
 * Untested on Windows/MSVC
 * Untested on Linux/BSD on GCC/CLang
 * Untested on x86
 * C++11 broken

# Disclaimer
This project contains undefined behaviours.
 * Forward declared std types if not using modules; if using modules, TBD.
 * Double underscore macros

# Incompatibilities

## Legacy ADL syntax of utl::swap
Pre-c++20 it is common to use the following syntax pattern for when algorithm/utility functions, e.g. `swap`
```c++
template<typename T>
class container {
public:
    void swap(container& other) {
        using std::swap; // bring std::swap into scope so that they can be use as a default implementation if ADL fails
        swap(data, other.data);
    }
private:
    T data;
};
```

This pattern may not always work with classes in the `std` namespace. In particular, in a case where an `std` class has no more specialized overload of `swap`  defined and the definition of the default `std::swap` (usually found in `<utility>`) is visible at instantiation of the `swap` call in a scope where `utl::swap` has been imported through `using utl::swap`, the overload cannot be resolved as ADL will also add `std::swap` as an overload candidate.

Consider the following example,
```c++
template<typename T>
class container {
public:
    void swap(container& other) {
        using utl::swap; // bring utl::swap into scope so that they can be use as a default implementation if ADL fails
        swap(data, other.data);
    }
private:
    T data;
};
```
If `T` is of type `std::string`, the program is valid, since `std::basic_string` has defined more specialized overloads as defined in `27.4.4.3, string.specials`. However, if T is of type, `std::string[N]`, where `N` is any valid array extent, the program becomes ill-formed as the `swap` lookup will be ambigous between the array overload (`22.2.2, utility.swap`) of `std::swap` (brought in via ADL) and the exact same overload defined in `utl`.

