#include "../include/window.hpp"
#include <iostream>

Window::Window(int width, int height, const std::string &title, bool resizable)
    : handle(nullptr), width(width), height(height), title(title),
      resizable(resizable) {}

Window::~Window() {
  if (handle) {
    glfwDestroyWindow(handle);
  }
  glfwTerminate();
}

bool Window::init() {
  if (!glfwInit()) {
    std::cerr << "Failed to initialize GLFW" << std::endl;
    return false;
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_RESIZABLE, resizable ? GLFW_TRUE : GLFW_FALSE);

  handle = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
  if (!handle) {
    std::cerr << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return false;
  }

  glfwMakeContextCurrent(handle);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cerr << "Failed to initialize GLAD" << std::endl;
    return false;
  }

  int fbW, fbH;
  glfwGetFramebufferSize(handle, &fbW, &fbH);
  glViewport(0, 0, fbW, fbH);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  return true;
}

void Window::update() {
  glfwSwapBuffers(handle);
  glfwPollEvents();

  glfwGetWindowSize(handle, &width, &height);

  int fbW, fbH;
  glfwGetFramebufferSize(handle, &fbW, &fbH);
  glViewport(0, 0, fbW, fbH);
}

void Window::close() { glfwSetWindowShouldClose(handle, true); }

bool Window::shouldClose() const { return glfwWindowShouldClose(handle); }

void Window::setShouldClose(bool value) {
  glfwSetWindowShouldClose(handle, value);
}

int Window::getWidth() const { return width; }
int Window::getHeight() const { return height; }

float Window::getAspectRatio() const { return (float)width / (float)height; }

void Window::getCursorPos(double &x, double &y) const {
  glfwGetCursorPos(handle, &x, &y);
}

bool Window::isMouseButtonPressed(int button) const {
  return glfwGetMouseButton(handle, button) == GLFW_PRESS;
}

bool Window::isKeyPressed(int key) const {
  return glfwGetKey(handle, key) == GLFW_PRESS;
}

GLFWwindow *Window::getNativeWindow() const { return handle; }
