#pragma once
#include "GameState.h"
#include "Console.h"
#include <vector>
#include <queue>
#include <set>
#include <cstdlib>
#include <cmath>

void InitInGame(GameState& state);
ULONGLONG GetCurrentDropInterval(GameState& state);

void UpdateInGame(GameState& state);
void UpdateShooter(GameState& state);
void ShootBall(GameState& state);
void UpdateBall(GameState& state);
void HoldBall(GameState& state);
void ProcessMatch(GameState& state, int r, int c);
void DropNewRow(GameState& state);

void RenderInGame(const GameState& state);
void RenderWall(const GameState& state);
void RenderShooter(const GameState& state);
void RenderAimLine(const GameState& state);
void RenderBall(const GameState& state);
void RenderAimLineByAngle(const GameState& state, float angle, bool erase);
wchar_t GetAimChar(float angle);
void ClearCell(int x, int y);
void RenderGrid(const GameState& state);

bool InGrid(int r, int c);
void GetNeighbors(int r, int c, int out[4][2]);
std::vector<std::pair<int, int> > FindConnected(GameState& state, int startR, int startC);
std::vector<std::pair<int, int> > FindFloating(GameState& state);

int  GetGridOffsetX();
int  GridToScreenX(int col);
int  GridToScreenY(int row);

bool CheckOrbCollision(const GameState& state, const Ball& ball, int& hitR, int& hitC);
void SnapBallToGrid(GameState& state, Ball& ball, int hitR, int hitC);

OrbColor GetRandomBubbleColor();
Color       ToConsoleColor(OrbColor color);

OrbType GetRandomOrbType();
const wchar_t* GetOrbShape(OrbType type);
void ClearOrbAt(GameState& state, int r, int c);
void ExplodeAround(GameState& state, int r, int c);

void CollectNearItems(GameState& state, int r, int c);
void CollectItemAt(GameState& state, int r, int c);