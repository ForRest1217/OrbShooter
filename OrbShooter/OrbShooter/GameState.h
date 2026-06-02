#pragma once
#include "Enums.h"
#define NOMINMAX

constexpr int WIDTH = 100;
constexpr int HEIGHT = 60;

struct GameState
{
	Scene  prevScene = Scene::NONE;
	Scene  curScene = Scene::TITLE;
	Menu   curMenu = Menu::START;
	bool   isRunning = true;
};
