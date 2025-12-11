#include "../include/gameState.hpp"
#include "../include/config.hpp"
#include "../include/shader.hpp"
#include "../include/texture.h"
#include <cstdlib>
#include <ctime>
#include <queue>

static inline int rcToIndex(int r, int c, int cols) { return r * cols + c; }

MinesweeperGame::MinesweeperGame() {}

MinesweeperGame::~MinesweeperGame() { glDeleteProgram(shaderProgram); }

void MinesweeperGame::init() {
  renderer.init();
  shaderProgram = Shader::createProgram();
  textRenderer.init(cfg.paths.font.c_str(), cfg.ui.fontSize);

  assets.closed = loadTexture(cfg.paths.closedTile.c_str(), 40, 40);
  assets.hover = loadTexture(cfg.paths.hoverTile.c_str(), 40, 40);
  assets.mine = loadTexture(cfg.paths.mine.c_str(), 40, 40);
  assets.mineRed = loadTexture(cfg.paths.mineRed.c_str(), 40, 40);
  assets.flag = loadTexture(cfg.paths.flag.c_str(), 40, 40);
  assets.wrongFlag = loadTexture(cfg.paths.wrongFlag.c_str(), 40, 40);

  for (int i = 0; i <= 8; ++i)
    assets.numbers.push_back(
        loadTexture(cfg.paths.getNumberPath(i).c_str(), 40, 40));

  assets.yellowNumbers.push_back(assets.numbers[0]);
  for (int i = 1; i <= 8; ++i)
    assets.yellowNumbers.push_back(
        loadTexture(cfg.paths.getYellowNumberPath(i).c_str(), 40, 40));

  for (int i = 0; i <= 9; ++i)
    assets.digits.push_back(
        loadTexture(cfg.paths.getDigitPath(i).c_str(), 26, 46));

  assets.faceHappy = loadTexture(cfg.paths.faceHappy.c_str(), 50, 50);
  assets.faceO = loadTexture(cfg.paths.faceO.c_str(), 50, 50);
  assets.faceDead = loadTexture(cfg.paths.faceDead.c_str(), 50, 50);
  assets.faceWin = assets.faceHappy;

  assets.borderH = loadTexture(cfg.paths.borderH.c_str(), 32, 32);
  assets.borderV = loadTexture(cfg.paths.borderV.c_str(), 32, 32);
  assets.cornerTL = loadTexture(cfg.paths.cornerTL.c_str(), 32, 32);
  assets.cornerTR = loadTexture(cfg.paths.cornerTR.c_str(), 32, 32);
  assets.cornerBL = loadTexture(cfg.paths.cornerBL.c_str(), 32, 32);
  assets.cornerBR = loadTexture(cfg.paths.cornerBR.c_str(), 32, 32);

  setDifficulty(Difficulty::BEGINNER);
}

void MinesweeperGame::setDifficulty(Difficulty d) {
  ctx.difficulty = d;
  switch (d) {
  case Difficulty::BEGINNER:
    ctx.rows = 9;
    ctx.cols = 9;
    ctx.totalMines = 20;
    break;
  case Difficulty::INTERMEDIATE:
    ctx.rows = 16;
    ctx.cols = 16;
    ctx.totalMines = 49;
    break;
  case Difficulty::EXPERT:
    ctx.rows = 30;
    ctx.cols = 30;
    ctx.totalMines = 199;
    break;
  }
  ctx.state = GameState::PLAYING;
  ctx.gameStarted = false;
  ctx.startTime = 0;
  ctx.finalTime = 0;
  resetBoard();
}

void MinesweeperGame::resetBoard() {
  tiles.resize(ctx.rows * ctx.cols);
  for (auto &t : tiles) {
    t.symbol = ' ';
    t.revealed = false;
    t.flagged = false;
    t.texture = assets.closed;
    t.x = 0;
    t.y = 0;
    t.w = 0;
    t.h = 0;
  }
}

void MinesweeperGame::computeTileLayout(int windowWidth, int windowHeight,
                                        float headerHeight, float menuHeight,
                                        float borderThickness) {
  float usableWidth = (float)windowWidth - (borderThickness * 2);
  float usableHeight =
      (float)windowHeight - headerHeight - menuHeight - (borderThickness * 2);

  if (usableWidth < 1.0f)
    usableWidth = 1.0f;
  if (usableHeight < 1.0f)
    usableHeight = 1.0f;

  float tileW = usableWidth / ctx.cols;
  float tileH = usableHeight / ctx.rows;
  float tileSize = (tileW < tileH) ? tileW : tileH;

  float gridWidth = tileSize * ctx.cols;
  float gridHeight = tileSize * ctx.rows;

  float startX = borderThickness + (usableWidth - gridWidth) * 0.5f;
  float startY = menuHeight + headerHeight + borderThickness +
                 (usableHeight - gridHeight) * 0.5f;

  for (int cy = 0; cy < ctx.rows; ++cy) {
    for (int cx = 0; cx < ctx.cols; ++cx) {
      int i = cy * ctx.cols + cx;
      if (i >= (int)tiles.size())
        break;
      tiles[i].x = startX + cx * tileSize;
      tiles[i].y = startY + cy * tileSize;
      tiles[i].w = tileSize;
      tiles[i].h = tileSize;
    }
  }
}

void MinesweeperGame::handleInput(Window &window) {
  double mx, my;
  window.getCursorPos(mx, my);

  bool leftPressed = window.isMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT);
  bool leftClicked = leftPressed && !lastLeftMouseState;
  ctx.leftMouseHeld = leftPressed;

  bool rightPressed = window.isMouseButtonPressed(GLFW_MOUSE_BUTTON_RIGHT);
  bool rightClicked = rightPressed && !lastRightMouseState;

  lastLeftMouseState = leftPressed;
  lastRightMouseState = rightPressed;

  int windowWidth = window.getWidth();
  int windowHeight = window.getHeight();

  float uiScale = (float)windowHeight / cfg.ui.referenceHeight;
  if (uiScale < cfg.ui.minScale)
    uiScale = cfg.ui.minScale;

  float currentHeaderH = cfg.ui.baseHeaderHeight * uiScale;
  float currentMenuH = cfg.ui.baseMenuHeight * uiScale;
  float currentFaceSize = cfg.ui.baseFaceSize * uiScale;
  float gridTop = currentMenuH + currentHeaderH;
  float menuBtnW = windowWidth / 3.0f;

  if (leftClicked && my < currentMenuH) {
    if (mx < menuBtnW)
      setDifficulty(Difficulty::BEGINNER);
    else if (mx < menuBtnW * 2)
      setDifficulty(Difficulty::INTERMEDIATE);
    else
      setDifficulty(Difficulty::EXPERT);

    float currentBorderTh = cfg.ui.baseBorderThickness * uiScale;
    computeTileLayout(windowWidth, windowHeight, currentHeaderH, currentMenuH,
                      currentBorderTh);
  }

  float faceX = (windowWidth - currentFaceSize) * 0.5f;
  float faceY = currentMenuH + (currentHeaderH - currentFaceSize) * 0.5f;
  bool hoverFace = isPointInsideRect((float)mx, (float)my, faceX, faceY,
                                     currentFaceSize, currentFaceSize);

  if (!leftPressed && lastLeftMouseState && hoverFace) {

    setDifficulty(ctx.difficulty);
  }

  auto findTileIndexAt = [&](double x, double y) -> int {
    for (int i = 0; i < (int)tiles.size(); ++i) {
      if (isPointInsideRect((float)x, (float)y, tiles[i].x, tiles[i].y,
                            tiles[i].w, tiles[i].h))
        return i;
    }
    return -1;
  };

  if (ctx.state == GameState::PLAYING && my > gridTop) {
    if (rightClicked) {
      int idx = findTileIndexAt(mx, my);
      if (idx >= 0 && !tiles[idx].revealed) {
        tiles[idx].flagged = !tiles[idx].flagged;
        tiles[idx].texture = tiles[idx].flagged ? assets.flag : assets.closed;
      }
    }
    if (leftClicked) {
      int idx = findTileIndexAt(mx, my);
      if (idx >= 0 && !tiles[idx].flagged && !tiles[idx].revealed) {
        if (!ctx.gameStarted) {
          ctx.gameStarted = true;
          ctx.startTime = glfwGetTime();
        }
        RevealResult res = revealTileAt(idx);
        if (res == RevealResult::GAME_OVER) {
          ctx.state = GameState::LOST;
          ctx.finalTime = glfwGetTime() - ctx.startTime;
          processGameOver(idx);
        } else if (res == RevealResult::WIN) {
          ctx.state = GameState::WON;
          ctx.finalTime = glfwGetTime() - ctx.startTime;
        }
      }
    }
  }
}

void MinesweeperGame::render(const Window &window) {
  int windowWidth = window.getWidth();
  int windowHeight = window.getHeight();

  if (windowWidth < 1 || windowHeight < 1)
    return;

  renderer.setProjection(shaderProgram, (float)windowWidth,
                         (float)windowHeight);

  float uiScale = (float)windowHeight / cfg.ui.referenceHeight;
  if (uiScale < cfg.ui.minScale)
    uiScale = cfg.ui.minScale;

  float currentHeaderH = cfg.ui.baseHeaderHeight * uiScale;
  float currentMenuH = cfg.ui.baseMenuHeight * uiScale;
  float currentBorderTh = cfg.ui.baseBorderThickness * uiScale;
  float currentFaceSize = cfg.ui.baseFaceSize * uiScale;

  if (windowWidth != lastWidth || windowHeight != lastHeight) {
    computeTileLayout(windowWidth, windowHeight, currentHeaderH, currentMenuH,
                      currentBorderTh);

    const_cast<MinesweeperGame *>(this)->lastWidth = windowWidth;
    const_cast<MinesweeperGame *>(this)->lastHeight = windowHeight;
  }

  float menuBtnW = windowWidth / 3.0f;
  float btnPad = cfg.ui.buttonPadding * uiScale;

  drawBorderFrame(0 + btnPad, btnPad, menuBtnW - (btnPad * 2),
                  currentMenuH - (btnPad * 2), currentBorderTh);
  drawBorderFrame(menuBtnW + btnPad, btnPad, menuBtnW - (btnPad * 2),
                  currentMenuH - (btnPad * 2), currentBorderTh);
  drawBorderFrame(menuBtnW * 2 + btnPad, btnPad, menuBtnW - (btnPad * 2),
                  currentMenuH - (btnPad * 2), currentBorderTh);

  float textScale = uiScale * cfg.ui.fontScaleCorrection;
  float textY = (currentMenuH + (cfg.ui.fontSize * textScale)) * 0.5f - 2.0f;
  auto drawCenteredText = [&](std::string txt, float btnX, float btnW) {
    float w = textRenderer.getWidth(txt, textScale);
    float tx = btnX + (btnW - w) * 0.5f;
    textRenderer.drawText(txt, tx, textY, textScale, cfg.colors.text[0],
                          cfg.colors.text[1], cfg.colors.text[2],
                          (float)windowWidth, (float)windowHeight);
  };

  drawCenteredText("BEGINNER", 0.0f, menuBtnW);
  drawCenteredText("INTERMEDIATE", menuBtnW, menuBtnW);
  drawCenteredText("EXPERT", menuBtnW * 2.0f, menuBtnW);

  float headerY = currentMenuH;
  drawBorderFrame(0, headerY, (float)windowWidth, currentHeaderH,
                  currentBorderTh);

  float faceX = (windowWidth - currentFaceSize) * 0.5f;
  float faceY = headerY + (currentHeaderH - currentFaceSize) * 0.5f;

  GLuint faceTex = assets.faceHappy;
  if (ctx.state == GameState::LOST)
    faceTex = assets.faceDead;
  else if (ctx.state == GameState::WON)
    faceTex = assets.faceWin;
  else if (ctx.leftMouseHeld) {

    double mx, my;
    window.getCursorPos(mx, my);

    bool hoverFace = isPointInsideRect((float)mx, (float)my, faceX, faceY,
                                       currentFaceSize, currentFaceSize);
    float gridTop = currentMenuH + currentHeaderH;

    if (hoverFace)
      faceTex = assets.faceO;
    else if (my > gridTop)
      faceTex = assets.faceO;
  }
  renderer.drawRect(shaderProgram, faceX, faceY, currentFaceSize,
                    currentFaceSize, faceTex);

  int flagsUsed = getFlaggedCount();
  drawCounter(cfg.ui.counterSideMargin * uiScale,
              headerY + (cfg.ui.counterTopMargin * uiScale),
              ctx.totalMines - flagsUsed, uiScale);

  int seconds = (ctx.gameStarted && ctx.state == GameState::PLAYING)
                    ? (int)(glfwGetTime() - ctx.startTime)
                    : (int)ctx.finalTime;

  float timerTotalW =
      ((cfg.ui.digitWidth * 3) + (cfg.ui.digitPadding * 2)) * uiScale;
  float rightMargin = cfg.ui.counterSideMargin * uiScale;
  drawCounter(windowWidth - timerTotalW - rightMargin,
              headerY + (cfg.ui.counterTopMargin * uiScale), seconds, uiScale);

  float gridTop = currentMenuH + currentHeaderH;
  drawBorderFrame(0, gridTop, (float)windowWidth, (float)windowHeight - gridTop,
                  currentBorderTh);

  double mx, my;
  window.getCursorPos(mx, my);

  for (auto &tile : tiles) {
    if (tile.x + tile.w < 0 || tile.x > windowWidth || tile.y + tile.h < 0 ||
        tile.y > windowHeight)
      continue;

    bool hover =
        (my > gridTop) &&
        isPointInsideRect((float)mx, (float)my, tile.x, tile.y, tile.w, tile.h);
    GLuint textureToDraw = tile.texture;

    if (ctx.state == GameState::PLAYING && !tile.revealed) {
      if (tile.flagged)
        textureToDraw = assets.flag;
      else if (hover && ctx.leftMouseHeld)
        textureToDraw = assets.hover;
      else
        textureToDraw = assets.closed;
    }
    renderer.drawRect(shaderProgram, tile.x, tile.y, tile.w, tile.h,
                      textureToDraw);
  }
}

void MinesweeperGame::drawCounter(float x, float y, int value, float scale) {
  if (value > 999)
    value = 999;
  if (value < 0)
    value = 0;
  int d1 = value / 100, d2 = (value % 100) / 10, d3 = value % 10;
  float w = cfg.ui.digitWidth * scale;
  float h = cfg.ui.digitHeight * scale;
  float pad = cfg.ui.digitPadding * scale;
  renderer.drawRect(shaderProgram, x, y, w, h, assets.digits[d1]);
  renderer.drawRect(shaderProgram, x + w + pad, y, w, h, assets.digits[d2]);
  renderer.drawRect(shaderProgram, x + (w + pad) * 2, y, w, h,
                    assets.digits[d3]);
}

void MinesweeperGame::drawBorderFrame(float x, float y, float w, float h,
                                      float th) {
  renderer.drawRect(shaderProgram, x, y, th, th, assets.cornerTL);
  renderer.drawRect(shaderProgram, x + w - th, y, th, th, assets.cornerTR);
  renderer.drawRect(shaderProgram, x, y + h - th, th, th, assets.cornerBL);
  renderer.drawRect(shaderProgram, x + w - th, y + h - th, th, th,
                    assets.cornerBR);
  renderer.drawRect(shaderProgram, x + th, y, w - 2 * th, th, assets.borderH);
  renderer.drawRect(shaderProgram, x + th, y + h - th, w - 2 * th, th,
                    assets.borderH);
  renderer.drawRect(shaderProgram, x, y + th, th, h - 2 * th, assets.borderV);
  renderer.drawRect(shaderProgram, x + w - th, y + th, th, h - 2 * th,
                    assets.borderV);
}

bool MinesweeperGame::isPointInsideRect(float px, float py, float x, float y,
                                        float w, float h) {
  return (px >= x && px <= x + w && py >= y && py <= y + h);
}

int MinesweeperGame::getFlaggedCount() const {
  int count = 0;
  for (const auto &t : tiles) {
    if (t.flagged)
      count++;
  }
  return count;
}

void MinesweeperGame::setTileContent(Tile &t, char symbol) {
  t.symbol = symbol;
  if (symbol == 'X') {
    t.texture = assets.mine;
  } else if (symbol >= '1' && symbol <= '8') {
    t.texture = assets.numbers[symbol - '0'];
  } else {
    t.texture = assets.numbers[0];
  }
}

int MinesweeperGame::countNeighborMines(int r, int c) {
  const int dir[8][2] = {{-1, -1}, {-1, 0}, {-1, 1}, {0, -1},
                         {0, 1},   {1, -1}, {1, 0},  {1, 1}};
  int count = 0;
  for (auto &d : dir) {
    int nr = r + d[0];
    int nc = c + d[1];
    if (nr >= 0 && nr < ctx.rows && nc >= 0 && nc < ctx.cols) {
      if (tiles[rcToIndex(nr, nc, ctx.cols)].symbol == 'X')
        count++;
    }
  }
  return count;
}

void MinesweeperGame::generateGameOnFirstClick(int safeIndex, int totalMines) {
  std::srand((unsigned int)std::time(nullptr));
  int minesPlaced = 0;
  int maxIndex = ctx.rows * ctx.cols;
  if (totalMines > maxIndex - 9)
    totalMines = maxIndex - 9;

  std::vector<int> safeZone;
  int safeR = safeIndex / ctx.cols;
  int safeC = safeIndex % ctx.cols;
  safeZone.push_back(safeIndex);

  const int dir[8][2] = {{-1, -1}, {-1, 0}, {-1, 1}, {0, -1},
                         {0, 1},   {1, -1}, {1, 0},  {1, 1}};
  for (auto &d : dir) {
    int nr = safeR + d[0];
    int nc = safeC + d[1];
    if (nr >= 0 && nr < ctx.rows && nc >= 0 && nc < ctx.cols) {
      safeZone.push_back(rcToIndex(nr, nc, ctx.cols));
    }
  }

  while (minesPlaced < totalMines) {
    int idx = std::rand() % maxIndex;
    if (tiles[idx].symbol == 'X')
      continue;
    bool isSafe = false;
    for (int s : safeZone) {
      if (idx == s) {
        isSafe = true;
        break;
      }
    }
    if (isSafe)
      continue;
    setTileContent(tiles[idx], 'X');
    minesPlaced++;
  }

  for (int r = 0; r < ctx.rows; ++r) {
    for (int c = 0; c < ctx.cols; ++c) {
      int idx = rcToIndex(r, c, ctx.cols);
      if (tiles[idx].symbol != 'X') {
        int mines = countNeighborMines(r, c);
        char sym = (mines == 0) ? ' ' : (char)('0' + mines);
        setTileContent(tiles[idx], sym);
      }
    }
  }
}

RevealResult MinesweeperGame::revealTileAt(int index) {
  Tile &first = tiles[index];
  if (first.flagged || first.revealed)
    return RevealResult::CONTINUE;

  bool boardIsBlank = true;
  for (const auto &t : tiles) {
    if (t.symbol == 'X') {
      boardIsBlank = false;
      break;
    }
  }

  if (boardIsBlank) {
    int totalCells = ctx.rows * ctx.cols;
    int mineCount = (int)(totalCells * 0.15);
    if (totalCells == 81)
      mineCount = 10;
    else if (totalCells == 256)
      mineCount = 40;
    else if (totalCells == 480)
      mineCount = 99;
    generateGameOnFirstClick(index, mineCount);
  }

  if (first.symbol == 'X') {
    first.revealed = true;
    first.texture = assets.mineRed;
    return RevealResult::GAME_OVER;
  }

  std::queue<int> q;
  q.push(index);
  std::vector<bool> visited(tiles.size(), false);
  visited[index] = true;

  const int dir[8][2] = {{-1, -1}, {-1, 0}, {-1, 1}, {0, -1},
                         {0, 1},   {1, -1}, {1, 0},  {1, 1}};

  while (!q.empty()) {
    int cur = q.front();
    q.pop();
    Tile &t = tiles[cur];
    if (t.flagged)
      continue;

    t.revealed = true;
    if (t.symbol == 'X')
      t.texture = assets.mine;
    else if (t.symbol >= '1' && t.symbol <= '8')
      t.texture = assets.numbers[t.symbol - '0'];
    else
      t.texture = assets.numbers[0];

    if (t.symbol == ' ') {
      int r = cur / ctx.cols;
      int c = cur % ctx.cols;
      for (auto &d : dir) {
        int nr = r + d[0];
        int nc = c + d[1];
        if (nr < 0 || nr >= ctx.rows || nc < 0 || nc >= ctx.cols)
          continue;
        int ni = rcToIndex(nr, nc, ctx.cols);
        if (!visited[ni] && !tiles[ni].revealed && !tiles[ni].flagged) {
          visited[ni] = true;
          q.push(ni);
        }
      }
    }
  }

  for (auto &t : tiles) {
    if (t.symbol != 'X' && !t.revealed)
      return RevealResult::CONTINUE;
  }
  return RevealResult::WIN;
}

void MinesweeperGame::processGameOver(int clickedIndex) {
  for (int i = 0; i < (int)tiles.size(); ++i) {
    Tile &t = tiles[i];
    if (i == clickedIndex) {
      t.texture = assets.mineRed;
      t.revealed = true;
      continue;
    }
    if (t.flagged && t.symbol != 'X') {
      t.texture = assets.wrongFlag;
    } else if (t.symbol == 'X' && !t.flagged) {
      t.texture = assets.mine;
      t.revealed = true;
    } else if (t.symbol != 'X' && !t.revealed && !t.flagged) {
      int val = 0;
      if (t.symbol >= '1' && t.symbol <= '8')
        val = t.symbol - '0';
      if (val > 0) {
        t.texture = assets.yellowNumbers[val];
        t.revealed = true;
      }
    }
  }
}
