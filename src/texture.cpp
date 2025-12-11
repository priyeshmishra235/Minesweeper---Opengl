#include "../include/texture.h"
#include <algorithm>
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include "../ext/stb_image.h"

#include <lunasvg/lunasvg.h>

static bool hasExtension(const std::string &path, const std::string &ext) {
  if (path.size() < ext.size())
    return false;

  std::string a = path.substr(path.size() - ext.size());
  std::string b = ext;

  std::transform(a.begin(), a.end(), a.begin(), ::tolower);
  std::transform(b.begin(), b.end(), b.begin(), ::tolower);

  return a == b;
}

GLuint loadTexture(const std::string &path, int svgW, int svgH) {
  GLuint tex = 0;
  glGenTextures(1, &tex);
  glBindTexture(GL_TEXTURE_2D, tex);

  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                  GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  int width = 0, height = 0, channels = 0;

  if (hasExtension(path, ".svg")) {
    if (svgW <= 0 || svgH <= 0) {
      std::cerr << "SVG requires explicit raster target size: " << path
                << std::endl;
      return 0;
    }

    auto svg = lunasvg::Document::loadFromFile(path);
    if (!svg) {
      std::cerr << "Failed to load SVG: " << path << std::endl;
      return 0;
    }

    auto bmp = svg->renderToBitmap(svgW, svgH);
    if (!bmp.valid()) {
      std::cerr << "Failed to render SVG: " << path << std::endl;
      return 0;
    }

    unsigned char *pixels = (unsigned char *)bmp.data();
    width = bmp.width();
    height = bmp.height();
    channels = 4;

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, pixels);

    glGenerateMipmap(GL_TEXTURE_2D);
    return tex;
  }

  stbi_set_flip_vertically_on_load(false);

  unsigned char *pixels =
      stbi_load(path.c_str(), &width, &height, &channels, 0);
  if (!pixels) {
    std::cerr << "Failed to load raster image: " << path << std::endl;
    return 0;
  }

  GLenum format = (channels == 4) ? GL_RGBA : GL_RGB;

  glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format,
               GL_UNSIGNED_BYTE, pixels);

  stbi_image_free(pixels);
  glGenerateMipmap(GL_TEXTURE_2D);

  return tex;
}
