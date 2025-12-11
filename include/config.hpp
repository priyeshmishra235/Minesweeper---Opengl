#pragma once
#include <string>

struct Config {
  struct Window {
    int width = 325 * 2;
    int height = 350 * 2;
    const char *title = "GL Minesweeper";
    bool resizable = true;
  } window;

  struct Colors {
    float bg[3] = {0.5f, 0.5f, 0.5f};
    float text[3] = {0.0f, 0.0f, 0.0f};
    float textActive[3] = {1.0f, 0.0f, 0.0f};
  } colors;

  struct Paths {
    std::string font = "../assets/font.ttf";
    std::string closedTile = "../assets/closed.png";
    std::string hoverTile = "../assets/pressed.png";
    std::string mine = "../assets/mine.png";
    std::string mineRed = "../assets/mine_red.png";
    std::string flag = "../assets/flag.png";
    std::string wrongFlag = "../assets/mine_wrong.png";
    std::string faceHappy = "../assets/face_unpressed.png";
    std::string faceO = "../assets/face_unpressed.png";
    std::string faceDead = "../assets/face_lose.png";
    std::string borderH = "../assets/border_hor_2x.png";
    std::string borderV = "../assets/border_vert_2x.png";
    std::string cornerTL = "../assets/corner_up_left_2x.png";
    std::string cornerTR = "../assets/corner_up_right_2x.png";
    std::string cornerBL = "../assets/corner_bottom_left_2x.png";
    std::string cornerBR = "../assets/corner_bottom_right_2x.png";

    std::string getNumberPath(int i) {
      return "../assets/type" + std::to_string(i) + ".png";
    }
    std::string getYellowNumberPath(int i) {
      return "../assets/type" + std::to_string(i) + "_yellow.png";
    }
    std::string getDigitPath(int i) {
      return "../assets/d" + std::to_string(i) + ".png";
    }
  } paths;

  struct UI {
    float referenceHeight = 700.0f;
    float minScale = 0.5f;

    float baseHeaderHeight = 60.0f;
    float baseMenuHeight = 30.0f;
    float baseBorderThickness = 10.0f;
    float baseFaceSize = 42.0f;

    float digitWidth = 20.0f;
    float digitHeight = 40.0f;
    float digitPadding = 2.0f;

    float counterSideMargin = 20.0f;
    float counterTopMargin = 10.0f;
    float buttonPadding = 2.0f;

    float fontSize = 24.0f;
    float fontScaleCorrection = 0.8f;
  } ui;
};

extern Config cfg;
