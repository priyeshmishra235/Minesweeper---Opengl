#pragma once
#include "../glad/glad.h"
#include <vector>

struct GameAssets {
  GLuint closed, hover, mine, mineRed, flag, wrongFlag;
  std::vector<GLuint> numbers;
  std::vector<GLuint> yellowNumbers;
  std::vector<GLuint> digits;
  GLuint faceHappy, faceO, faceDead, faceWin;
  GLuint borderH, borderV, cornerTL, cornerTR, cornerBL, cornerBR;
};
