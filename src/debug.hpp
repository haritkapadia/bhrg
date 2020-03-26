#ifndef DEBUG_HPP
#define DEBUG_HPP

#define NDEBUG
#include <assert.h>
#include <iostream>

#ifdef NDEBUG
template <typename T> void print(T t) {}
template <typename T, typename... Args> void print(T t, Args... args) {}
#else
template <typename T> void print(T t) { std::cout << t << '\n'; }
template <typename T, typename... Args> void print(T t, Args... args) {
    std::cout << t << ' ';
    print(args...);
}
#endif

#endif
