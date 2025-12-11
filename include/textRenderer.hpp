#pragma once
#include "../ext/stb_truetype.h"
#include "../glad/glad.h"
#include <string>

class TextRenderer {
public:
  void init(const char *filename, float pixelHeight);
  float getWidth(const std::string &text, float scale);
  void drawText(const std::string &text, float x, float y, float scale, float r,
                float g, float b, float sW, float sH);

private:
  GLuint texID;
  stbtt_bakedchar cdata[96];
  GLuint vao, vbo;
  GLuint shaderProgram;
};
