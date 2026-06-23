#include "ConsoleUI.h"
#include <cwchar>

void ClearLine(int y)
{
    GotoXY(0, y);

    for (int i = 0; i < WIDTH; ++i)
        cout << " ";
}

void ClearArea(int x, int y, int width)
{
    GotoXY(x, y);

    for (int i = 0; i < width; ++i)
        cout << " ";
}

void ClearScreenByLine()
{
    for (int y = 0; y < HEIGHT; ++y)
        ClearLine(y);
}

void DrawTextAt(int x, int y, const string& text, Color color)
{
    GotoXY(x, y);

    SetColor(color);
    cout << text;
    SetColor();
}

void DrawWTextAt(int x, int y, const wchar_t* text, Color color)
{
    GotoXY(x, y);

    SetColor(color);

    DWORD written = 0;
    WriteConsoleW(GetStdHandle(STD_OUTPUT_HANDLE), text, (DWORD)wcslen(text), &written, nullptr);

    SetColor();
}

void DrawCenterText(int y, const string& text, Color color)
{
    ClearLine(y);

    int x = WIDTH / 2 - (int)text.length() / 2;

    if (x < 0)
        x = 0;

    DrawTextAt(x, y, text, color);
}

void DrawCenterWText(int y, const wchar_t* text, Color color)
{
    ClearLine(y);

    int len = (int)wcslen(text);
    int x = WIDTH / 2 - len / 2;

    if (x < 0)
        x = 0;

    DrawWTextAt(x, y, text, color);
}

void DrawMenuText(int y, const string& text, bool selected)
{
    string line;

    if (selected)
        line = "> " + text + " <";
    else
        line = "  " + text + "  ";

    DrawCenterText(y, line, selected ? Color::LIGHT_YELLOW : Color::WHITE);
}

bool GetMouseDown()
{
    static bool prevClick = false;

    bool curClick = GetAsyncKeyState(VK_LBUTTON) & 0x8000;
    bool result = curClick && !prevClick;

    prevClick = curClick;

    return result;
}

bool IsMouseInArea(int startX, int startY, int endX, int endY)
{
    POINT pos = GetMouseCellPos();

    return
        pos.x >= startX &&
        pos.x <= endX &&
        pos.y >= startY &&
        pos.y <= endY;
}

bool IsMouseInCenterArea(int y, int width)
{
    int startX = WIDTH / 2 - width / 2;
    int endX = startX + width;

    return IsMouseInArea(startX, y, endX, y);
}