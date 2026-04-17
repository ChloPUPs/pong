#include <iostream>
#include <cstdint>
#include <raylib.h>

struct WindowDetails {
    const int32_t width, height;
    const char* title;
};

int main() {
    WindowDetails window{
        .width = 640, .height = 480,
        .title = "Pong"
    };

    InitWindow(window.width, window.height, window.title);

    while (!WindowShouldClose()) {
        BeginDrawing();
            ClearBackground(BLACK);
        EndDrawing();
    }

    CloseWindow();

    return 0;
}
