#pragma once
#include "../glad/glad.h"

class Renderer {
public:
  Renderer();
  ~Renderer();

  void init();

  void setProjection(GLuint prog, int width, int height);

  void drawRect(GLuint prog, float x, float y, float w, float h,
                unsigned int btnTex);

private:
  GLuint VAO, VBO, EBO;

  void updateQuad(float x, float y, float w, float h, float u0, float v0,
                  float u1, float v1);
};
