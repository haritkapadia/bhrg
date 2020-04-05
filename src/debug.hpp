#ifndef DEBUG_HPP
#define DEBUG_HPP

#include <assert.h>
#include <iostream>

namespace debug {
#ifdef BHRG_NO_DEBUG
template <typename T> void print(T t) {}
template <typename T, typename... Args> void print(T t, Args... args) {}
#else
template <typename T> void print(T t) { std::cout << t << '\n'; }
template <typename T, typename... Args> void print(T t, Args... args) {
    std::cout << t << ' ';
    print(args...);
}
#endif
} // namespace debug

#endif
