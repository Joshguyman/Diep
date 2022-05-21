#include "../include/raylib.h"
#include <ctime>
#include <iostream>
#include "../include/raymath.h"
#include <vector>
#include <deque>
#include <string>
using namespace std;

int main(void)
{
    const int screenWidth = 1600;
    const int screenHeight = 900;

    InitWindow(screenWidth, screenHeight, "Gravity");

    SetTargetFPS(60);
    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        EndDrawing();
    }
    CloseWindow();

    return 0;
}