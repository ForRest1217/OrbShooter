#pragma once
#include "GameState.h"
#include "Console.h"
#include <vector>
#include <queue>
#include <set>
#include <cstdlib>
#include <cmath>

void InitInGame(GameState& state);

void UpdateInGame(GameState& state);
void UpdateShooter(GameState& state);
void ShootBall(GameState& state);
void UpdateBall(GameState& state);
void ProcessMatch(GameState& state, int r, int c);
void DropNewRow(GameState& state);

void RenderInGame(const GameState& state);
void RenderWall();
void RenderShooter(const GameState& state);
void RenderAimLine(const GameState& state);
void RenderBall(const GameState& state);
void DrawAimLineByAngle(const GameState& state, float angle, bool erase);
void ClearCell(int x, int y);
void RenderGrid(const GameState& state);

bool InGrid(int r, int c);
void GetNeighbors(int r, int c, int out[6][2]);
std::vector<std::pair<int, int> > FindConnected(GameState& state, int startR, int startC);
std::vector<std::pair<int, int> > FindFloating(GameState& state);

int  GetGridOffsetX();
int  GridToScreenX(int col);
int  GridToScreenY(int row);

bool CheckBubbleCollision(const GameState& state);
void SnapBallToGrid(GameState& state);

BubbleColor GetRandomBubbleColor();
Color       ToConsoleColor(BubbleColor color);