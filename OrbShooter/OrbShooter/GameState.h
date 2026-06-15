#pragma once
#include "Enums.h"
#include <windows.h>
#define NOMINMAX

constexpr int WIDTH = 100;
constexpr int HEIGHT = 40;
constexpr int GRID_COLS = 13;
constexpr int GRID_ROWS = 12;
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

    BubbleColor color = BubbleColor::RED;
    bool isMoving = false;
    bool isActive = false;  
};

struct Shooter
{
    int x = WIDTH / 2;
    int y = HEIGHT - 5;
    float angle = 0.0f;
    float prevAngle = 999.0f;   

    BubbleColor currentColor = BubbleColor::RED;
    BubbleColor nextColor = BubbleColor::RED;

    BubbleColor holdColor = BubbleColor::NONE; 
    bool hasHold = false;
};

struct Bubble
{
    BubbleColor color = BubbleColor::NONE;
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