#pragma once
#include "../glad/glad.h"
#include <GLFW/glfw3.h>
#include <string>

class Window {
public:
  Window(int width, int height, const std::string &title, bool resizable);
  ~Window();

  bool init();
  void update();
  void close();

  bool shouldClose() const;
  void setShouldClose(bool value);

  int getWidth() const;
  int getHeight() const;
  float getAspectRatio() const;

  void getCursorPos(double &x, double &y) const;
  bool isMouseButtonPressed(int button) const;
  bool isKeyPressed(int key) const;

  GLFWwindow *getNativeWindow() const;

private:
  GLFWwindow *handle;
  int width;
  int height;
  std::string title;
  bool resizable;
};
