#include "../include/config.hpp"
#include "../include/gameState.hpp"
#include "../include/window.hpp"

Config cfg;

int main() {
  Window window(cfg.window.width, cfg.window.height, cfg.window.title,
                cfg.window.resizable);

  if (!window.init()) {
    return -1;
  }

  MinesweeperGame game;
  game.init();

  while (!window.shouldClose()) {
    if (window.isKeyPressed(GLFW_KEY_ESCAPE)) {
      window.setShouldClose(true);
    }

    glClearColor(cfg.colors.bg[0], cfg.colors.bg[1], cfg.colors.bg[2], 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    game.handleInput(window);
    game.render(window);

    window.update();
  }

  return 0;
}
