#pragma once
#include "GameState.h"
#include "Console.h"

void InitTitle(GameState& state);
void UpdateTitle(GameState& state);
void RenderTitle(const GameState& state);

void InitInfo(GameState& state);
void UpdateInfo(GameState& state);
void RenderInfo(const GameState& state);