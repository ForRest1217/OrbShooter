#pragma once

#include<iostream>
#include<conio.h>
#define NOMINMAX //c++기본 민맥스 쓰고싶으니 이걸 디파인 해준다.
#include<Windows.h>
#include<io.h>
#include<fcntl.h>
#include<iomanip>
#include<string>
#pragma comment(lib, "winmm.lib") //PlaySound()함수 사용하기 위해서

using std::string;
using std::wstring;
using std::cout;
using std::endl;
using std::wcout;

//아스키아트
void SetUnicodeMode();
void SetDefaultMode();

//다이얼로그
void RenderDialogue(const string& text, int delay);
void RenderChar(char ch, int delayTime, bool isSkip);

//콘솔 세팅
void SetConsoleGameTiltle(const wstring& title);
void SetConsoleWindowSize(int width, int height);
void SetConsoleFullScreen();
void SetConsoleWindowStyle(bool showTitleBar);
void ShakeConsoleWindow(int intensity, int duration, int interval);
void SetConsoleMouseInputDisabled();

//커서 제어
COORD GetConsoleResolution();
void GotoXY(int x, int y);
void SetCursorVisible(bool visible, DWORD size = 1);
BOOL IsGotoXY(int x, int y);

//색 바꾸기
enum class Color
{
    BLACK, BLUE, GREEN, SKYBLUE, RED,
    VIOLET, YELLOW, LIGHT_GRAY, GRAY, LIGHT_BLUE,
    LIGHT_GREEN, CYAN, LIGHT_RED, LIGHT_VIOLET,
    LIGHT_YELLOW, WHITE, END
};

void SetColor(Color textColor = Color::WHITE, Color bgColor = Color::BLACK);

void DrawBar(int x, int y, const string& label, 
                       int value, int maxValue, int barWidth, 
                       const string& fillChar = "■", const string& emptyChar = "□");
void DrawLine(char ch, int width);
void SetConsoleFont(const wstring& fontName, COORD size, UINT weight = FW_NORMAL);

//키 입력
bool GetKey(int vKey);
bool GetKeyDown(int vKey);
void UpdateInput();
void FrameSync(int fps);
POINT GetMouseCellPos();
int GetCursorX();