#pragma once
#include "Enums.h"
#define NOMINMAX

constexpr int WIDTH = 100;
constexpr int HEIGHT = 49;

enum class OrbColor
{
	RED,
	BLUE,
	GREEN,
	YELLOW
};

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

	bool isActive = false;
	bool isMoving = false;
};

struct Shooter
{
	int x = WIDTH / 2;
	int y = HEIGHT - 5;

	float angle = 0.0f;
	float prevAngle = 999.0f;

	OrbColor nextColor = OrbColor::RED;
};

struct GameState
{
	Scene  prevScene = Scene::NONE;
	Scene  curScene = Scene::INGAME;
	Menu   curMenu = Menu::START;
	bool   isRunning = true;

	Shooter shooter;
	Ball ball;
};