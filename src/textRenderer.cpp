#include "../include/textRenderer.hpp"
#include <cstdio>
#include <iostream>
#include <vector>

#define STB_TRUETYPE_IMPLEMENTATION
#include "../ext/stb_truetype.h"

void TextRenderer::init(const char *filename, float pixelHeight) {
  unsigned char *ttf_buffer = (unsigned char *)malloc(1 << 20);
  unsigned char *temp_bitmap = (unsigned char *)malloc(512 * 512);
  FILE *f = fopen(filename, "rb");
  if (!f) {
    std::cerr << "Failed to load font: " << filename << "\n";
    return;
  }
  fread(ttf_buffer, 1, 1 << 20, f);
  fclose(f);
  stbtt_BakeFontBitmap(ttf_buffer, 0, pixelHeight, temp_bitmap, 512, 512, 32,
                       96, cdata);
  free(ttf_buffer);

  glGenTextures(1, &texID);
  glBindTexture(GL_TEXTURE_2D, texID);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, 512, 512, 0, GL_RED, GL_UNSIGNED_BYTE,
               temp_bitmap);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  free(temp_bitmap);

  glGenVertexArrays(1, &vao);
  glGenBuffers(1, &vbo);
  glBindVertexArray(vao);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4 * 128, NULL,
               GL_DYNAMIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
                        (void *)(2 * sizeof(float)));

  const char *vS = "#version 330 core\nlayout (location=0) in vec2 p; layout "
                   "(location=1) in vec2 t; out vec2 T; uniform mat4 P; void "
                   "main(){gl_Position=P*vec4(p,0,1); T=t;}";
  const char *fS =
      "#version 330 core\nin vec2 T; out vec4 C; uniform sampler2D x; "
      "uniform vec3 c; void main(){C=vec4(c,texture(x,T).r);}";
  GLuint vs = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vs, 1, &vS, 0);
  glCompileShader(vs);
  GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fs, 1, &fS, 0);
  glCompileShader(fs);
  shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vs);
  glAttachShader(shaderProgram, fs);
  glLinkProgram(shaderProgram);
  glDeleteShader(vs);
  glDeleteShader(fs);
}

float TextRenderer::getWidth(const std::string &text, float scale) {
  float x = 0;
  float y = 0;
  for (char c : text) {
    if (c < 32 || c >= 127)
      continue;
    stbtt_aligned_quad q;
    stbtt_GetBakedQuad(cdata, 512, 512, c - 32, &x, &y, &q, 1);
  }
  return x * scale;
}

void TextRenderer::drawText(const std::string &text, float x, float y,
                            float scale, float r, float g, float b, float sW,
                            float sH) {
  glUseProgram(shaderProgram);
  float p[16] = {2.0f / sW, 0, 0,  0, 0,  -2.0f / sH, 0, 0,
                 0,         0, -1, 0, -1, 1,          0, 1};
  glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "P"), 1, GL_FALSE, p);
  glUniform3f(glGetUniformLocation(shaderProgram, "c"), r, g, b);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, texID);
  glBindVertexArray(vao);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);

  std::vector<float> v;

  for (char c : text) {
    if (c < 32 || c >= 127)
      continue;
    stbtt_aligned_quad q;

    float tempX = 0, tempY = 0;
    stbtt_GetBakedQuad(cdata, 512, 512, c - 32, &tempX, &tempY, &q, 1);

    float x0 = x + q.x0 * scale;
    float y0 = y + q.y0 * scale;
    float x1 = x + q.x1 * scale;
    float y1 = y + q.y1 * scale;

    x += tempX * scale;

    v.insert(v.end(),
             {x0, y1, q.s0, q.t1, x0, y0, q.s0, q.t0, x1, y0, q.s1, q.t0,
              x0, y1, q.s0, q.t1, x1, y0, q.s1, q.t0, x1, y1, q.s1, q.t1});
  }
  glBufferSubData(GL_ARRAY_BUFFER, 0, v.size() * sizeof(float), v.data());
  glDrawArrays(GL_TRIANGLES, 0, v.size() / 4);
}
