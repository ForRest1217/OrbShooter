#pragma once
#include "Enums.h"
#include <windows.h>
#define NOMINMAX

constexpr int WIDTH = 100;
constexpr int HEIGHT = 60;

constexpr int GRID_COLS = 13;
constexpr int GRID_ROWS = 12;
constexpr int DROP_SEC = 5;

struct Bubble 
{ 
    BubbleColor color = BubbleColor::NONE; 
};

struct GameState
{
    Scene     prevScene = Scene::NONE;
    Scene     curScene = Scene::TITLE;
    Menu      curMenu = Menu::START;
    bool      isRunning = true;

    Bubble    grid[GRID_ROWS][GRID_COLS];
    ULONGLONG lastDropTick = 0;
    int       score = 0;
};
