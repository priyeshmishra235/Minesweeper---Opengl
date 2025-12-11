#pragma once
#include "../glad/glad.h"
#include "assets.hpp"
#include "renderer.hpp"
#include "textRenderer.hpp"
#include "window.hpp"
#include <vector>

enum class GameState { PLAYING, WON, LOST };
enum class Difficulty { BEGINNER, INTERMEDIATE, EXPERT };
enum class RevealResult { CONTINUE, GAME_OVER, WIN };

struct Tile {
  float x, y, w, h;
  char symbol;
  bool revealed;
  bool flagged;
  GLuint texture;
};

struct GameContext {
  int rows = 9;
  int cols = 9;
  int totalMines = 10;
  Difficulty difficulty = Difficulty::BEGINNER;
  GameState state = GameState::PLAYING;
  double startTime = 0.0;
  double finalTime = 0.0;
  bool gameStarted = false;
  bool leftMouseHeld = false;
};

class MinesweeperGame {
public:
  MinesweeperGame();
  ~MinesweeperGame();

  void init();

  void update(float width, float height);

  void render(const Window &window);

  void handleInput(Window &window);

private:
  void setDifficulty(Difficulty d);
  void computeTileLayout(int windowWidth, int windowHeight, float headerHeight,
                         float menuHeight, float borderThickness);
  void resetBoard();
  RevealResult revealTileAt(int index);
  void processGameOver(int clickedIndex);
  void generateGameOnFirstClick(int safeIndex, int totalMines);
  int countNeighborMines(int r, int c);
  void setTileContent(Tile &t, char symbol);
  int getFlaggedCount() const;
  bool isPointInsideRect(float px, float py, float x, float y, float w,
                         float h);

  void drawCounter(float x, float y, int value, float scale);
  void drawBorderFrame(float x, float y, float w, float h, float th);

  GameContext ctx;
  std::vector<Tile> tiles;
  GameAssets assets;

  Renderer renderer;
  TextRenderer textRenderer;
  GLuint shaderProgram;

  bool lastLeftMouseState = false;
  bool lastRightMouseState = false;
  int lastWidth = 0;
  int lastHeight = 0;
};
