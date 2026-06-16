#pragma once
#include "Enums.h"
#include <windows.h>
#define NOMINMAX

constexpr int WIDTH = 100;
constexpr int HEIGHT = 40;
constexpr int GRID_COLS = 13;

constexpr int SHOOTER_START_Y = HEIGHT - 5;
constexpr int GAMEOVER_Y = SHOOTER_START_Y + 1;

constexpr int GRID_ROWS = GAMEOVER_Y;

constexpr int DROP_SEC = 5;

struct Ball
{
    float x = 0.0f;
    float y = 0.0f;
    float prevX = 0.0f;   
    float prevY = 0.0f;

    float dirX = 0.0f;
    float dirY = -1.0f;
    float speed = 0.7f;

    OrbColor color = OrbColor::RED;
    OrbType type = OrbType::NORMAL;

    bool isMoving = false;
    bool isActive = false;  
};

struct Shooter
{
    int x = WIDTH / 2;
    int y = SHOOTER_START_Y;
    float angle = 0.0f;
    float prevAngle = 999.0f;   

    OrbColor currentColor = OrbColor::RED;
    OrbColor nextColor = OrbColor::RED;

    OrbType currentType = OrbType::NORMAL;
    OrbType nextType = OrbType::NORMAL;

    OrbColor holdColor = OrbColor::NONE;
    OrbType holdType = OrbType::NORMAL;

    bool hasHold = false;
};

struct Bubble
{
    OrbColor color = OrbColor::NONE;
    OrbType type = OrbType::NORMAL;
};

struct GameState
{
    Scene prevScene = Scene::NONE;
    Scene curScene = Scene::TITLE;
    Menu  curMenu = Menu::START;
    bool  isRunning = true;

    Ball    ball;
    Shooter shooter;

    Bubble grid[GRID_ROWS][GRID_COLS];
    ULONGLONG  lastDropTick = 0;
    int score = 0;
};