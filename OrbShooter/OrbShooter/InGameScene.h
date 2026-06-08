#pragma once
#include "GameState.h"

void InitInGame(GameState& state);
void UpdateInGame(GameState& state);
void RenderInGame(const GameState& state);
void ProcessMatch(GameState& state, int r, int c);