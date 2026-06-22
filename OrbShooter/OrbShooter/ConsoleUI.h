#pragma once

#include "Console.h"
#include "GameState.h"

void ClearLine(int y);
void ClearArea(int x, int y, int width);
void ClearScreenByLine();

void DrawTextAt(int x, int y, const string& text, Color color = Color::WHITE);
void DrawWTextAt(int x, int y, const wchar_t* text, Color color = Color::WHITE);

void DrawCenterText(int y, const string& text, Color color = Color::WHITE);
void DrawCenterWText(int y, const wchar_t* text, Color color = Color::WHITE);

void DrawMenuText(int y, const string& text, bool selected);

bool GetMouseDown();
bool IsMouseInArea(int startX, int startY, int endX, int endY);
bool IsMouseInCenterArea(int y, int width);