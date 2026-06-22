#pragma once
#include "GameState.h"
#include "Console.h"

void InitGameover(GameState& state);
void UpdateGameover(GameState& state);
void RenderGameover(const GameState& state);

void InitInfo(GameState& state);
void UpdateInfo(GameState& state);
void RenderInfo(const GameState& state);