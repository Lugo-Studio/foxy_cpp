//
// Created by galex on 9/3/2022.
//

#include "context.hpp"

// this is here to expose vulkan functions to glfw
#define FOXY_GLFW_INCLUDE_VULKAN
#include "glfw.hpp"

namespace fx::inferno {
  inline static void glfw_error_callback(int error, const char* message) {
    fx::Log::error("GFLW: {} | {}", error, message);
  }

  Context::Context() {
    fx::i32 glfw_init_success = glfwInit();
    if (!glfw_init_success) {
      fx::Log::fatal("Failed to initialize GLFW!");
    }
    fx::i32 major, minor, revision;
    glfwGetVersion(&major, &minor, &revision);
    glfwSetErrorCallback(glfw_error_callback);

    fx::Log::debug("GLFW Version {}.{}.{}", major, minor, revision);
  }

  Context::~Context() {
    glfwTerminate();
  }
}