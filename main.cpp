#include <iostream>
#include <raylib.h>

using namespace std;

const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 800;

const int BALL_RADIUS = 20;

const int PLAYER_PADDLE_HEIGHT = 100;
const int PLAYER_PADDLE_WIDTH = 25;
const int PLAYER_PADDLE_SPEED = 8;

const int CPU_PADDLE_HEIGHT = 100;
const int CPU_PADDLE_WIDTH = 25;
const int CPU_PADDLE_SPEED = 8;

int player_score = 0;
int cpu_score = 0;

class Ball {
public:
  float x, y;
  float speed_x, speed_y;
  int radius;

  Ball(int x, int y, int speed_x, int speed_y, int radius) {
    this->x = x;
    this->y = y;
    this->speed_x = speed_x;
    this->speed_y = speed_y;
    this->radius = radius;
  }

  void Draw() { DrawCircle(x, y, radius, WHITE); }

  void setSpeed(float speed) {
    this->speed_x = speed;
    this->speed_y = speed;
  }

  void Update() {

    x += speed_x;
    y += speed_y;

    if (y + radius >= SCREEN_HEIGHT) {
      y = SCREEN_HEIGHT - radius;
      speed_y = -1 * speed_y;
    }

    if (y - radius <= 0) {
      y = 0 + radius;
      speed_y = -1 * speed_y;
    }

    if (x + radius >= SCREEN_WIDTH) {
      x = SCREEN_WIDTH - radius;
      speed_x = -1 * speed_x;
    }

    if (x - radius <= 0) {
      x = 0 + radius;
      speed_x = -1 * speed_x;
    }
  }

  void UpdateLegacy() {
    x += speed_x;
    y += speed_y;

    if (y + radius >= SCREEN_HEIGHT || y - radius <= 0) {
      speed_y = -1 * speed_y;
    }

    if (x + radius >= SCREEN_WIDTH || x - radius <= 0) {
      speed_x = -1 * speed_x;
    }
  }

  // not used currently
  void resetBall() {
    x = (float) SCREEN_HEIGHT / 2;
    y = (float) SCREEN_WIDTH / 2;

    int speedChoices[2] = {-1, 1};
    speed_x *= speedChoices[GetRandomValue(0, 1)];
    speed_y *= speedChoices[GetRandomValue(0, 1)];
  }
};

class Paddle {

protected:
  void LimitMovement() {
    if (y <= 0) {
      y = 0 + 5;
    }

    if (y + height >= SCREEN_HEIGHT) {
      y = SCREEN_HEIGHT - height - 5;
    }
  }

public:
  float x, y;
  float width, height;
  Color color;
  int speed;

  Paddle(int x, int y, int width, int height, Color color, int speed)
      : x(x), y(y), width(width), height(height), color(color), speed(speed) {}

  void Draw() { DrawRectangle(x, y, width, height, color); }

  void Update() {

    if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
      y = GetMouseY() - height / 2;
    } else {
      if (IsKeyDown(KEY_UP))
        y -= speed;
      if (IsKeyDown(KEY_DOWN))
        y += speed;
    }

    LimitMovement();
  }
};

class CPUPaddle : public Paddle {
public:
  CPUPaddle(int x, int y, int width, int height, Color color, int speed)
      : Paddle(x, y, width, height, color, speed) {}

  void Update(int ball_y) {
    if (y + height / 2 >= ball_y) {
      y -= speed;
    }

    if (y + height / 2 <= ball_y) {
      y += speed;
    }

    LimitMovement();
  }
};

int main() {
  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Insane pong!!");
  SetTargetFPS(60);

  Ball ball(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 7, 7, BALL_RADIUS);

  Paddle playerPaddle(5, SCREEN_HEIGHT / 2 - PLAYER_PADDLE_HEIGHT / 2,
                      PLAYER_PADDLE_WIDTH, PLAYER_PADDLE_HEIGHT, GREEN,
                      PLAYER_PADDLE_SPEED);
  CPUPaddle computerPaddle(SCREEN_WIDTH - CPU_PADDLE_WIDTH - 5,
                           SCREEN_HEIGHT / 2 - CPU_PADDLE_HEIGHT / 2,
                           CPU_PADDLE_WIDTH, CPU_PADDLE_HEIGHT, RED,
                           CPU_PADDLE_SPEED);

  // ==== DIFFICULTY MENU ====
  bool difficultyChosen = false;
  Rectangle easyRect = {(float)SCREEN_WIDTH / 2 - 50, 300, 100, 30};
  Rectangle hardRect = {(float)SCREEN_WIDTH / 2 - 50, 350, 100, 30};

  while (!difficultyChosen && !WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(BLACK);

    DrawText("Choose difficulty:", SCREEN_WIDTH / 2 - 100, 200, 20, WHITE);

    Vector2 mousePos = GetMousePosition();

    // HOVER ANIMATION: grow size if mouse over
    Rectangle easyHover = easyRect;
    Rectangle hardHover = hardRect;
    Color easyColor = GREEN;
    Color hardColor = RED;

    if (CheckCollisionPointRec(mousePos, easyRect)) {
      easyHover.width += 10;
      easyHover.height += 5;
      easyHover.x -= 5; // center the growth
      easyHover.y -= 2.5;
      easyColor = YELLOW;
    }

    if (CheckCollisionPointRec(mousePos, hardRect)) {
      hardHover.width += 10;
      hardHover.height += 5;
      hardHover.x -= 5;
      hardHover.y -= 2.5;
      hardColor = ORANGE;
    }

    // Draw rectangles
    DrawRectangleRec(easyHover, easyColor);
    DrawText("E - Easy", easyHover.x + 10, easyHover.y + 5, 20, BLACK);

    DrawRectangleRec(hardHover, hardColor);
    DrawText("H - Hard", hardHover.x + 10, hardHover.y + 5, 20, BLACK);

    // Check keyboard
    if (IsKeyPressed(KEY_E)) {
      ball.setSpeed(5);
      difficultyChosen = true;
    }
    if (IsKeyPressed(KEY_H)) {
      ball.setSpeed(12);
      difficultyChosen = true;
    }

    // Check mouse click
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
      if (CheckCollisionPointRec(mousePos, easyRect)) {
        ball.setSpeed(5);
        difficultyChosen = true;
      }
      if (CheckCollisionPointRec(mousePos, hardRect)) {
        ball.setSpeed(12);
        difficultyChosen = true;
      }
    }

    EndDrawing();
  }

  while (!WindowShouldClose()) {
    BeginDrawing();

    // Clear canvas each frame
    ClearBackground(BLACK);

    // Draw game elements
    DrawLine(SCREEN_WIDTH / 2, 0, SCREEN_WIDTH / 2, SCREEN_HEIGHT, WHITE);
    ball.Draw();
    playerPaddle.Draw();
    computerPaddle.Draw();
    DrawText(TextFormat("%i", player_score), SCREEN_WIDTH / 2 - SCREEN_WIDTH / 4, 20, 80, WHITE);
    DrawText(TextFormat("%i", cpu_score), SCREEN_WIDTH / 2 + SCREEN_WIDTH / 4, 20, 80, WHITE);

    // Check for collision //

    // Check ball collision with player
    if (CheckCollisionCircleRec(Vector2{ball.x, ball.y}, ball.radius,
                                Rectangle{playerPaddle.x, playerPaddle.y,
                                          playerPaddle.width,
                                          playerPaddle.height})) {

      ball.speed_x *= -1;

      // ball.x *= -1;
      // ball.y *= -1;

      cout << "colloide" << endl;
    }

    // Check ball collision with computer
    if (CheckCollisionCircleRec(Vector2{ball.x, ball.y}, ball.radius,
                                Rectangle{computerPaddle.x, computerPaddle.y,
                                          computerPaddle.width,
                                          computerPaddle.height})) {

      ball.speed_x *= -1;

      // ball.x *= -1;
      // ball.y *= -1;

      cout << "colloide" << endl;
    }

    if (ball.x + ball.radius >= SCREEN_WIDTH) {
      player_score++;
    }

    if (ball.x - ball.radius <= 0) {
      cpu_score++;
    }

    // Update game elements
    ball.Update();
    playerPaddle.Update();
    computerPaddle.Update(ball.y);

    EndDrawing();
  }

  CloseWindow();

  return 0;
}
