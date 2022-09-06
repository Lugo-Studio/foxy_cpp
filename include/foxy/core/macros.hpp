//
// Created by galex on 8/21/2022.
//

#pragma once

#include <iostream>

#include "log_macros.hpp"

#if not defined(FOXY_DEBUG_MODE) and not defined(_WIN32) and defined(SIGTRAP)
#include <csignal>
#endif
#ifndef FOXY_DEBUG_MODE
  #define FOXY_DEBUG_BREAK
#else
  #ifdef _WIN32 // Windows
    #define FOXY_DEBUG_BREAK __debugbreak()
  #else
    #ifdef SIGTRAP // POSIX
      #define FOXY_DEBUG_BREAK raise(SIGTRAP)
    #else // Other
      #define FOXY_DEBUG_BREAK raise(SIGABRT)
    #endif
  #endif
#endif

#ifdef FOXY_ENABLE_ASSERTS
  #define FOXY_ASSERT(x) DCHECK(x) << "FAILED ASSERT: "
#else
  #define FOXY_ASSERT(x) DCHECK(x)
#endif

#define FN auto
#define MAKE_UNIQUE(x) std::make_unique<decltype(x)>(x)

#define BIT(x) (1 << x)
#define FOXY_BIT_COMPARE_TO(x, y) (x & y) == y
#define FOXY_STRINGIFY_VAL(x) FOXY_STRINGIFY(x)
#define FOXY_STRINGIFY(x) #x
#define FOXY_LAMBDA_INS(fn, instance) [objPtr = instance](auto&&... args) { return objPtr->fn(std::forward<decltype(args)>(args)...); }
#define FOXY_LAMBDA(fn) FOXY_LAMBDA_INS(fn, this)

#if defined(_WIN32) and not defined(FOXY_DEBUG_MODE)
#define REDIRECT_WINMAIN_TO_MAIN \
auto main(int, char**) -> int;\
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {\
  return main(__argc, __argv);\
}
#else
#define REDIRECT_WINMAIN_TO_MAIN auto __fubuki_is_cute() -> int { return 0; }
#endif

#if defined(_WIN32) and not defined(FOXY_DEBUG_MODE)
#define REDIRECT_WINMAIN_TO_FOXY_MAIN \
auto main(foxy::i32, foxy::i8**) -> foxy::i32;\
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {\
  return main(__argc, reinterpret_cast<std::int8_t**>(__argv));\
}
#else
#define REDIRECT_WINMAIN_TO_FOXY_MAIN auto __fubuki_is_cute() -> foxy::i32 { return 0; }
#endif