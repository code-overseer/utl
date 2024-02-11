# Untitled Library (UTL)

This essentially started out as a simple exercise with 3 simple goals:
 * Learn how to use concepts in C++20
 * Find out if there are noticable differences between SFINAE and Concepts
 * Implement a custom tuple that maintains triviality and "standard layout"-ness if all its elements meet those requirements

Eventually, it spiraled into learning more about how all the common compilers (Clang, GCC, MSVC, etc.) implement certain things and wondering what newer features could be brought into the older standards, e.g. constexpr things in C++11 that were only constexpr in C++17, etc.
