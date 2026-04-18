/* PONG (electricaloutlet edition) */

#include <iostream>
#include <cstdint>
#include <array>
#include <cmath>
#include <utility>
#include <raylib.h>

struct WindowDetails {
    const int32_t width, height;
    const char* title;
};

enum Direction {
    DIR_LEFT,
    DIR_RIGHT,
};

class GameObject {
public:
    const float speed;
    Vector2 vel;
    Vector2 dir;
    Rectangle dest;

    GameObject(Rectangle initDest, float speed)
    : speed(speed), dest(initDest) {
        this->vel.x = 0.0f;
        this->vel.y = 0.0f;
        this->dir.x = 0.0f;
        this->dir.y = 0.0f;
    }

    GameObject() : speed(0.0f) {
        this->dest = (Rectangle) {
            .x = 0.0f,
            .y = 0.0f,
            .width = 0.0f,
            .height = 0.0f,
        };
        this->dir = (Vector2) {0.0f, 0.0f};
        this->vel = (Vector2) {0.0f, 0.0f};
    }

    void applyVelocity() {
        dest.x += vel.x * GetFrameTime();
        dest.y += vel.y * GetFrameTime();
    }

    float getCenterX() {
        return this->dest.x + (this->dest.width / 2);
    }

    float getCenterY() {
        return this->dest.y + (this->dest.height / 2);
    }
};

class Paddle : public GameObject {
public:
    Direction side;
    int32_t points;

    Paddle(Rectangle initDest, float speed, Direction side)
    : GameObject(initDest, speed), side(side), points(0) {}

    bool isOutOfBounds() {
        return this->dest.y < 0.0f || this->dest.y;
    }

    // Moves the paddle back in bounds if it out is out of bounds.
    void ifOutMoveIn(WindowDetails window) {
        while (this->dest.y < 0.0f) {
            this->dest.y += 1.0f;
        }
        while (this->dest.y > (window.height - this->dest.height)) {
            this->dest.y -= 1.0f;
        }
    }

    Paddle() : GameObject(), side(DIR_LEFT), points(0) {}
};

class PlayerPaddle : public Paddle {
public:
    PlayerPaddle(Rectangle initDest, float speed, Direction side)
    : Paddle(initDest, speed, side) {}

    PlayerPaddle() : Paddle() {}

    void input() {
        if (this->side == DIR_RIGHT)
            this->dir.y = -IsKeyDown(KEY_UP) + IsKeyDown(KEY_DOWN);
        else
            this->dir.y = -IsKeyDown(KEY_W) + IsKeyDown(KEY_S);
        this->vel.y = dir.y * speed;
    }
};

class Ball : public GameObject {
public:
    Ball(Rectangle initDest, float speed)
    : GameObject(initDest, speed) {
        this->vel.x = -speed;
        this->vel.y = speed;
    }

    void handleWallCollision(WindowDetails window) {
        if (
            this->dest.x < 0.0f ||
            this->dest.x > (float)(window.width - this->dest.width)
        ) {
            this->vel.x = -this->vel.x;
        }
        if (
            this->dest.y < 0.0f ||
            this->dest.y > (float)(window.height - this->dest.height)
        ) {
            this->vel.y = -this->vel.y;
        }
    }

    // This code is so bad
    template <size_t N>
    std::pair<bool, Paddle*> isHitPaddles(std::array<Paddle*, N> paddles) {
        for (Paddle *paddle : paddles) {
            switch (paddle->side) {
                case DIR_LEFT:
                    if ((paddle->dest.x + paddle->dest.width) - 12.0f > this->dest.x) {
                        continue;
                    }
                    break;
                case DIR_RIGHT:
                    if (paddle->dest.x + 12.0f < this->dest.x) {
                        continue;
                    }
                    break;
            }

            bool collided = CheckCollisionRecs(this->dest, paddle->dest);
            if (collided) {
                return std::pair{true, paddle};
            } else {
                continue;
            }
        }
        Paddle p;
        return std::pair{false, nullptr};
    }

    void applyHitPaddles(Paddle hitPaddle) {
        std::cout << "Hit!\n";
        this->vel.x = -this->vel.x;
        switch (hitPaddle.side) {
            case DIR_LEFT:
                this->dest.x = hitPaddle.dest.x + hitPaddle.dest.width;
                break;
            case DIR_RIGHT:
                this->dest.x = hitPaddle.dest.x - hitPaddle.dest.width;
                break;
        }
    }
};

int main() {
    const WindowDetails window{
        .width = 640, .height = 480,
        .title = "Pong"
    };

    InitWindow(window.width, window.height, window.title);
    SetTargetFPS(240);

    InitAudioDevice();

    const Sound collisionSound = LoadSound("../res/collision.wav");

    PlayerPaddle player1((Rectangle) {
        .x = 40.0f,
        .y = (float)((window.height / 2.0f) - 50.0f),
        .width = 20.0f,
        .height = 100.0f,
    }, 230.0f, DIR_LEFT);
    PlayerPaddle player2((Rectangle) {
        .x = (window.width - 40.f) - 20.0f,
        .y = (float)((window.height / 2.0f) - 50.0f),
        .width = 20.0f,
        .height = 100.0f,
    }, 230.0f, DIR_RIGHT);

    Ball ball((Rectangle) {
        .x = (float)((window.width * (2.0f / 3.0f)) - 10.0f),
        .y = (float)((window.height / 2.0f) - 10.0f),
        .width = 20.0f,
        .height = 20.0f,
    }, 140.0f);

    while (!WindowShouldClose()) {
        player1.input();
        player2.input();
        player1.applyVelocity();
        player2.applyVelocity();
        player1.ifOutMoveIn(window);
        player2.ifOutMoveIn(window);

        ball.handleWallCollision(window);
        // I could probably just return the pointer by itself and check if it is null or not
        // that might be marginally less safe though so idk
        std::pair<bool, Paddle*> res = ball.isHitPaddles<2>({&player1, &player2});
        if (res.first && res.second) {
            ball.applyHitPaddles(*res.second);
            res.second->points += 1;
            PlaySound(collisionSound);
        }
        ball.applyVelocity();

        BeginDrawing();
            ClearBackground(BLACK);
            DrawRectangle(
                ball.dest.x,
                ball.dest.y,
                ball.dest.width,
                ball.dest.height,
                WHITE
            );
            DrawRectangle(
                player1.dest.x,
                player1.dest.y,
                player1.dest.width,
                player1.dest.height,
                WHITE
            );
            DrawRectangle(
                player2.dest.x,
                player2.dest.y,
                player2.dest.width,
                player2.dest.height,
                WHITE
            );
            std::string t1 = std::to_string(player1.points);
            DrawText(t1.c_str(), 120, window.height / 2 - 32, 64, WHITE);
            std::string t2 = std::to_string(player2.points);
            DrawText(t2.c_str(), window.width - 120 - MeasureText(t2.c_str(), 64), window.height / 2 - 32, 64, WHITE);
        EndDrawing();
    }

    UnloadSound(collisionSound);

    CloseAudioDevice();
    CloseWindow();

    return 0;
}
