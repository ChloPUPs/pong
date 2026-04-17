/* PONG (electricaloutlet edition) */

#include <iostream>
#include <cstdint>
#include <raylib.h>

struct WindowDetails {
    const int32_t width, height;
    const char* title;
};

class GameObject {
public:
    const float speed;
    Vector2 vel;
    Vector2 dir;
    Rectangle dest;

    GameObject(Rectangle initDest, float speed)
    : speed(speed), dest(initDest) {
        vel.x = 0.0f;
        vel.y = 0.0f;
    }

    void applyVelocity() {
        dest.x += vel.x * GetFrameTime();
        dest.y += vel.y * GetFrameTime();
    }
};

class PlayerPaddle : public GameObject {
public:
    PlayerPaddle(Rectangle initDest, float speed)
    : GameObject(initDest, speed) {}

    void input() {
        this->dir.y = -IsKeyDown(KEY_UP) + IsKeyDown(KEY_DOWN);
        this->vel.y = dir.y * speed;
    }
};

class EnemyPaddle : public GameObject {
public:
};

class Ball : public GameObject {
public:
};

int main() {
    const WindowDetails window{
        .width = 640, .height = 480,
        .title = "Pong"
    };

    InitWindow(window.width, window.height, window.title);

    PlayerPaddle player((Rectangle) {
        .x = 40.0f,
        .y = (float)(window.height / 2.0f),
        .width = 20.0f,
        .height = 60.0f,
    }, 230.0f);

    while (!WindowShouldClose()) {
        player.input();
        player.applyVelocity();

        BeginDrawing();
            ClearBackground(BLACK);
            DrawRectangle(
                player.dest.x,
                player.dest.y,
                player.dest.width,
                player.dest.height,
                WHITE
            );
        EndDrawing();
    }

    CloseWindow();

    return 0;
}
