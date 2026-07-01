#pragma once
#include "Enums.h"
#include <windows.h>
#include <vector>
#define NOMINMAX

constexpr int WIDTH = 100; 
constexpr int HEIGHT = 40;

constexpr int SHOOTER_START_Y = HEIGHT - 5; //슈터 위치
constexpr int GAMEOVER_Y = SHOOTER_START_Y - 1; //게임오버 위치

constexpr int GRID_COLS = 13; //열
constexpr int GRID_ROWS = GAMEOVER_Y; // 행

const float ORB_HIT_DIST = 3.0f;
const float ORB_FRONT_DOT = 0.85f;

//오브 줄 나오는 시간 조절
constexpr ULONGLONG START_DROP_MS = 5000; //처음시간
constexpr ULONGLONG MIN_DROP_MS = 1200; //최소시간
constexpr ULONGLONG DROP_SPEED_UP_MS = 300; //줄어드는 시간
constexpr ULONGLONG DROP_SPEED_UP_INTERVAL_MS = 10000; //몇초마다 줄어드는가

//공 나올 확률
constexpr int BOMB_ORB_CHANCE = 7;
constexpr int JOKER_ORB_CHANCE = 10;

//아이템
constexpr int ITEM_DROP_CHANCE = 23;
//난사
constexpr ULONGLONG BARRAGE_SHOT_INTERVAL = 300;
constexpr float NORMAL_BALL_SPEED = 0.17;
constexpr float BARRAGE_BALL_SPEED = 0.25;
//얼음
constexpr ULONGLONG DROP_FREEZE_DURATION = 3000;

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

    ItemType item = ItemType::NONE;
};

struct GameState
{
    Scene prevScene = Scene::NONE;
    Scene curScene = Scene::TITLE;
    Menu  curMenu = Menu::START;
    Menu  prevMenu = Menu::QUIT;    
    bool  isRunning = true;

    ULONGLONG gameStartTick = 0;

    //오브 슈터
    Shooter shooter;
    std::vector<Ball> balls;
    ULONGLONG lastBallUpdateTick = 0;

    //오브 그리드
    Bubble grid[GRID_ROWS][GRID_COLS];
    ULONGLONG  lastDropTick = 0;
    int score = 0;

    //아이템
    ItemType currentItem = ItemType::NONE;
    bool hasItem = false;
    //난사
    bool isBarrage = false;
    ULONGLONG barrageEndTick = 0;
    ULONGLONG barrageLastShotTick = 0;
    //얼음
    bool isDropFreeze = false;
    ULONGLONG dropFreezeEndTick = 0;
};