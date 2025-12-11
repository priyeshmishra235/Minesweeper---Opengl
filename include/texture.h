#pragma once
#include "../glad/glad.h"
#include <string>

GLuint loadTexture(const std::string &path, int svgW = 0, int svgH = 0);

inline GLuint loadTexture(const char *path, int svgW = 0, int svgH = 0) {
  return loadTexture(std::string(path), svgW, svgH);
}
