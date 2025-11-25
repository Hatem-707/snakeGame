#include <cstdlib>
#include <ctime>
#include <optional>
#include <raylib.h>
#include <set>
#include <utility>
#include <vector>

enum class GameState {
  starting,
  running,
  paused,
  gameOver,
};

enum class Directions { up, right, down, left };

std::vector<Directions> compass = {
    Directions::right,
    Directions::down,
    Directions::left,
    Directions::up,
};

std::vector<float> rotations = {
    0.f,
    90.f,
    180.f,
    270.f,
};

bool inRange(int max, int min, int value) {
  return ((value >= min) && (value <= max));
}

class Node {
  Node *next;
  Node *prev;
  std::pair<int, int> position;

public:
  Node(Node *next, Node *prev, std::pair<int, int> position)
      : next(next), prev(prev), position(position) {

        };
  friend class Snake;
};

class Snake {
  Node *head;
  Node *tail;
  Directions direction = Directions::right;
  int directionIndex = 0;
  bool eaten = false;
  Rectangle snakeHead;
  Rectangle snakeTail;
  Rectangle snakeBody;
  Rectangle snakeCornerUpRight;
  Rectangle snakeCornerDownRight;
  Rectangle snakeCornerUpLeft;
  Rectangle snakeCornerDownLeft;
  Texture2D assets;
  Vector2 cell;

  float tailRotation() {
    if (tail->prev->position.first == tail->position.first + 1) {
      return 0.f;
    } else if (tail->prev->position.first == tail->position.first - 1) {
      return 180.f;
    } else if (tail->prev->position.second == tail->position.second - 1) {
      return 270.f;
    } else {
      return 90.f;
    }
  }

  Rectangle getBodyRotation(Node *runner) {
    std::vector<bool> neighbors(4, false);
    neighbors[0] =
        (runner->next->position.first == runner->position.first + 1) ||
        (runner->prev->position.first == runner->position.first + 1);
    neighbors[2] =
        (runner->next->position.first == runner->position.first - 1) ||
        (runner->prev->position.first == runner->position.first - 1);
    neighbors[1] =
        (runner->next->position.second == runner->position.second - 1) ||
        (runner->prev->position.second == runner->position.second - 1);
    neighbors[3] =
        (runner->next->position.second == runner->position.second + 1) ||
        (runner->prev->position.second == runner->position.second + 1);
    if (neighbors[0] && neighbors[2] || neighbors[1] && neighbors[3]) {
      return snakeBody;
    } else if (neighbors[0] && neighbors[3]) {
      return snakeCornerUpLeft;
    } else if (neighbors[2] && neighbors[3]) {
      return snakeCornerUpRight;
    } else if (neighbors[0] && neighbors[1]) {
      return snakeCornerDownLeft;
    } else {
      return snakeCornerDownRight;
    }
  }

public:
  Snake(std::pair<int, int> startingPos, Vector2 assetGrid, Vector2 cell,
        Texture2D assets)
      : head(new Node(nullptr, nullptr, startingPos)),
        tail(new Node(nullptr, head,
                      {startingPos.first - 1, startingPos.second})),
        cell(cell), assets(assets) {
    snakeHead = {2 * assetGrid.x, 0, assetGrid.x, assetGrid.y};
    snakeTail = {0, 0, assetGrid.x, assetGrid.y};
    snakeBody = {assetGrid.x, 0, assetGrid.x, assetGrid.y};
    snakeCornerUpRight = {assetGrid.x, assetGrid.y, assetGrid.x, assetGrid.y};
    snakeCornerDownRight = {3 * assetGrid.x, assetGrid.y, assetGrid.x,
                            assetGrid.y};
    snakeCornerUpLeft = {0 * assetGrid.x, assetGrid.y, assetGrid.x,
                         assetGrid.y};
    snakeCornerDownLeft = {2 * assetGrid.x, assetGrid.y, assetGrid.x,
                           assetGrid.y};
    head->next = tail;
    tail->prev = head;
  }
  std::pair<int, int> getHeadPos() { return head->position; }
  std::pair<int, int> getTailPos() { return tail->position; }
  void setEaten() { this->eaten = true; }
  void addNode() {
    Node *newNode = new Node(nullptr, tail, tail->position);
    tail->next = newNode;
    newNode->prev = tail;
    tail = newNode;
  }
  void nextDirection() {
    this->direction = compass[(++directionIndex) % 4];
    directionIndex = directionIndex % 4;
  }
  void prevDirection() {
    this->direction = compass[(--directionIndex + 4) % 4];
    directionIndex = (directionIndex + 4) % 4;
  }

  void update() {
    Node *runner = (this->eaten) ? tail->prev : tail;
    // update everything other than the head
    while (runner != nullptr && runner->prev != nullptr) {
      runner->position = runner->prev->position;
      runner = runner->prev;
    }
    // update head
    switch (this->direction) {
    case Directions::up:
      runner->position = {runner->position.first, runner->position.second - 1};
      break;
    case Directions::down:
      runner->position = {runner->position.first, runner->position.second + 1};
      break;
    case Directions::left:
      runner->position = {runner->position.first - 1, runner->position.second};
      break;
    case Directions::right:
      runner->position = {runner->position.first + 1, runner->position.second};
      break;
    }
    if (this->eaten) {
      this->eaten = false;
    }
  }
  void draw() {
    Node *runner = head;
    Rectangle dest = {runner->position.first * cell.x + cell.x / 2,
                      runner->position.second * cell.y + cell.y / 2, cell.x,
                      cell.y};
    DrawTexturePro(assets, snakeHead, dest, {cell.x / 2, cell.y / 2},
                   rotations[directionIndex], WHITE);
    runner = runner->next;
    while (runner->next != nullptr) {
      dest = {runner->position.first * cell.x, runner->position.second * cell.y,
              cell.x, cell.y};
      DrawTexturePro(assets, getBodyRotation(runner), dest, {0, 0}, 0, WHITE);
      runner = runner->next;
    }
    dest = {runner->position.first * cell.x + cell.x / 2,
            runner->position.second * cell.y + cell.y / 2, cell.x, cell.y};
    DrawTexturePro(assets, snakeTail, dest, {cell.x / 2, cell.y / 2},
                   this->tailRotation(), WHITE);
  }
  ~Snake() {
    Node *runner = head;
    while (runner != nullptr) {
      Node *temp = runner;
      runner = runner->next;
      delete temp;
    }
  }
};

void drawWorld(int width, int height, Vector2 cell, Vector2 assetGrid,
               Texture2D assets, std::vector<std::vector<bool>> worldGrid) {
  Rectangle upGrass = {5 * assetGrid.x, 0, assetGrid.x, assetGrid.y};
  Rectangle downGrass = {6 * assetGrid.x, assetGrid.y, assetGrid.x,
                         assetGrid.y};
  Rectangle rightGrass = {6 * assetGrid.x, 0, assetGrid.x, assetGrid.y};
  Rectangle leftGrass = {5 * assetGrid.x, assetGrid.y, assetGrid.x,
                         assetGrid.y};

  for (int i = 0; i < width / cell.x; i++) {
    for (int j = 0; j < height / cell.y; j++) {
      Rectangle cellDest = {i * cell.x, j * cell.y, cell.x, cell.y};
      bool parity = worldGrid[i][j];
      Rectangle cellSource = {4 * assetGrid.x, parity ? assetGrid.y : 0,
                              assetGrid.x, assetGrid.y};
      DrawTexturePro(assets, cellSource, cellDest, {0.f, 0.f}, 0, WHITE);

      if (i > 0 && worldGrid[i - 1][j]) {
        DrawTexturePro(assets, leftGrass, cellDest, {0.f, 0.f}, 0, WHITE);
      }
      if (i + 1 < width / cell.x && worldGrid[i + 1][j]) {
        DrawTexturePro(assets, rightGrass, cellDest, {0.f, 0.f}, 0, WHITE);
      }
      if (j + 1 < height / cell.y && worldGrid[i][j + 1]) {
        DrawTexturePro(assets, downGrass, cellDest, {0.f, 0.f}, 0, WHITE);
      }
      if (j > 0 && worldGrid[i][j - 1]) {
        DrawTexturePro(assets, upGrass, cellDest, {0.f, 0.f}, 0, WHITE);
      }
    }
  }
}

std::pair<int, int> spawnApple(int width, int height,
                               std::set<std::pair<int, int>> occupied) {
  std::pair<int, int> proposedPos = {(rand() % width), (rand() % height)};
  while (occupied.contains(proposedPos)) {
    proposedPos = {(float)(rand() % width), (float)(rand() % height)};
  }
  return proposedPos;
}

int main() {
  int height = 900;
  int width = 1800;
  int moveEvery = 20;
  std::set<std::pair<int, int>> occupied;
  Vector2 cell = {45, 45};
  Vector2 numCells = {width / cell.x, height / cell.y};
  InitWindow(width, height, "Snake Game");
  SetTargetFPS(60);
  srand(time(0));

  Texture2D assets =
      LoadTexture("/home/hatem/Development/cpp/snakeGame/assets/Textures.png");
  Vector2 assetGrid = {(float)assets.width / 7, (float)assets.height / 2};
  Texture2D startScreen = LoadTexture(
      "/home/hatem/Development/cpp/snakeGame/assets/StartScreen.png");
  Texture2D loseScreen = LoadTexture(
      "/home/hatem/Development/cpp/snakeGame/assets/LoseScreen.png");

  GameState state = GameState::starting;
  Rectangle fullScreenSource = {0.f, 0.f, (float)startScreen.width,
                                (float)startScreen.height};
  Rectangle fullScreenDest = {0.f, 0.f, (float)width, (float)height};

  // 1. Setup Grid Logic
  std::vector<std::vector<bool>> worldGrid(
      width / cell.x,
      std::vector<bool>(height / cell.y)); // Removed {false} init for clarity
  for (auto &row : worldGrid) {
    for (int i = 0; i < row.size(); i++) {
      row[i] = rand() % 2;
    }
  }

  RenderTexture2D backgroundTexture = LoadRenderTexture(width, height);

  BeginTextureMode(backgroundTexture);

  ClearBackground(BLACK);
  drawWorld(width, height, cell, assetGrid, assets, worldGrid);

  EndTextureMode();

  Rectangle bgRecSource = {0.0f, 0.0f, (float)backgroundTexture.texture.width,
                           -(float)backgroundTexture.texture.height};
  Rectangle bgRecDest = {0.0f, 0.0f, (float)width, (float)height};

  Snake snake({numCells.x / 2, numCells.y / 2}, assetGrid, cell, assets);
  occupied.insert({width / 2, height / 2.f});
  occupied.insert({(width / 2) - 1, height / 2});

  std::pair<int, int> apple = spawnApple(numCells.x, numCells.y, occupied);
  Rectangle appleSrc = {3 * assetGrid.x, 0, assetGrid.x, assetGrid.y};
  int count = 1;

  std::optional<int> cachedDirection = std::nullopt;

  while (!WindowShouldClose()) {
    BeginDrawing();

    switch (state) {
    case GameState::starting:
      DrawTexturePro(startScreen, fullScreenSource, fullScreenDest, {0.f, 0.f},
                     0, WHITE);
      DrawText("Press Enter to start", width / 2 - 220, height / 2 - 20, 40,
               WHITE);
      if (IsKeyPressed(KEY_ENTER)) {
        state = GameState::running;
      }
      break;

    case GameState::running: {

      DrawTexturePro(backgroundTexture.texture, bgRecSource, bgRecDest, {0, 0},
                     0, WHITE);
      Rectangle appleDest = {(float)apple.first * cell.x,
                             (float)apple.second * cell.y, cell.x, cell.y};
      DrawTexturePro(assets, appleSrc, appleDest, {0, 0}, 0, WHITE);
      snake.draw();

      if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)) {
        cachedDirection = 1;
      } else if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A)) {
        cachedDirection = -1;
      }
      if (!(count % moveEvery)) {
        bool eaten = false;
        if (cachedDirection.has_value()) {
          if (cachedDirection.value() == 1) {
            snake.nextDirection();
          } else {
            snake.prevDirection();
          }
          cachedDirection.reset();
        }

        if (snake.getHeadPos() == apple) {
          eaten = true;
          apple = spawnApple(numCells.x, numCells.y, occupied);
          snake.setEaten();
          snake.addNode();
        }
        snake.update();
        if (occupied.contains(snake.getHeadPos()) ||
            !inRange(numCells.x - 1, 0, snake.getHeadPos().first) ||
            !inRange(numCells.y - 1, 0, snake.getHeadPos().second)) {
          state = GameState::gameOver;
        }
        occupied.insert(snake.getHeadPos());
        if (!eaten) {
          occupied.erase(snake.getTailPos());
        }
        count = 1;
      } else {
        count++;
      }

      if (IsKeyPressed(KEY_P)) {
        state = GameState::paused;
      }
      break;
    }

    case GameState::paused:
      DrawTexturePro(backgroundTexture.texture, bgRecSource, bgRecDest, {0, 0},
                     0, WHITE);

      DrawText("Game paused. Press p again to continue!", width / 2 - 400,
               height / 2 - 20, 40, BROWN);

      if (IsKeyPressed(KEY_P)) {
        state = GameState::running;
      }
      break;

    case GameState::gameOver:
      ClearBackground(BLACK);
      DrawTexturePro(loseScreen, fullScreenSource, fullScreenDest, {0.f, 0.f},
                     0, WHITE);
      break;
    }
    EndDrawing();
  }

  // Cleanup
  UnloadRenderTexture(backgroundTexture);
  UnloadTexture(assets);
  UnloadTexture(startScreen);
  UnloadTexture(loseScreen);
  CloseWindow();
}
