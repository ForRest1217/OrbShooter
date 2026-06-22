#include "GameOverScene.h"
#include <cstdlib>
#include <string>
#include <cwchar>
#include <utility>

static int showScore = 0;
static int prevShowScore = -1;
static int prevMenu = -1;
static int commentIndex = -1;

static ULONGLONG lastScoreTick = 0;

static bool isDrawStatic = false;
static bool isDrawRank = false;
static bool isDrawComment = false;

static void ClearLine(int y)
{
    GotoXY(0, y);

    for (int i = 0; i < WIDTH; ++i)
        cout << " ";
}

static void ClearArea(int x, int y, int width)
{
    GotoXY(x, y);

    for (int i = 0; i < width; ++i)
        cout << " ";
}

static void ClearScreenByLine()
{
    for (int y = 0; y < HEIGHT; ++y)
        ClearLine(y);
}

static void DrawCenterText(int y, const string& text, Color color = Color::WHITE)
{
    ClearLine(y);

    int x = WIDTH / 2 - (int)text.length() / 2;

    if (x < 0)
        x = 0;

    GotoXY(x, y);

    SetColor(color);
    cout << text;
    SetColor();
}

static void DrawWTextAt(int x, int y, const wchar_t* text, Color color = Color::WHITE)
{
    GotoXY(x, y);

    SetColor(color);

    DWORD written = 0;
    WriteConsoleW(GetStdHandle(STD_OUTPUT_HANDLE), text, (DWORD)wcslen(text), &written, nullptr);

    SetColor();
}

static void DrawCenterWText(int y, const wchar_t* text, Color color = Color::WHITE)
{
    ClearLine(y);

    int len = (int)wcslen(text);
    int x = WIDTH / 2 - len / 2;

    if (x < 0)
        x = 0;

    DrawWTextAt(x, y, text, color);
}

static void DrawGameOverLogo()
{
    DrawCenterWText(3, L"  █████████                                        ███████                                  ", Color::LIGHT_RED);
    DrawCenterWText(4, L" ███░░░░░███                                     ███░░░░░░███                                ", Color::LIGHT_RED);
    DrawCenterWText(5, L" ███     ░░░   ██████   █████████████    ██████  ███     ░░███ █████ █████  ██████  ████████ ", Color::LIGHT_RED);
    DrawCenterWText(6, L"░███          ░░░░░███ ░░███░░███░░███  ███░░███░███      ░███░░███ ░░███  ███░░███░░███░░███", Color::LIGHT_RED);
    DrawCenterWText(7, L"░███    █████  ███████  ░███ ░███ ░███ ░███████ ░███      ░███ ░███  ░███ ░███████  ░███ ░░░ ", Color::LIGHT_RED);
    DrawCenterWText(8, L"░░███  ░░███  ███░░███  ░███ ░███ ░███ ░███░░░  ░░███     ███  ░░███ ███  ░███░░░   ░███     ", Color::LIGHT_RED);
    DrawCenterWText(9, L" ░░█████████ ░░████████ █████░███ █████░░██████  ░░░███████░    ░░█████   ░░██████  █████    ", Color::LIGHT_RED);
    DrawCenterWText(10, L"  ░░░░░░░░░   ░░░░░░░░ ░░░░░ ░░░ ░░░░░  ░░░░░░     ░░░░░░░       ░░░░░     ░░░░░░  ░░░░░     ", Color::LIGHT_RED);
}

static char GetRank(int score)
{
    if (score >= 15000)
        return 'S';

    if (score >= 10000)
        return 'A';

    if (score >= 7000)
        return 'B';

    if (score >= 5000)
        return 'C';

    if (score >= 3000)
        return 'D';

    return 'F';
}

static void DrawRankS(int x, int y)
{
    DrawWTextAt(x, y + 0, L"███████╗", Color::LIGHT_YELLOW);
    DrawWTextAt(x, y + 1, L"██╔════╝", Color::LIGHT_YELLOW);
    DrawWTextAt(x, y + 2, L"███████╗", Color::LIGHT_YELLOW);
    DrawWTextAt(x, y + 3, L"╚════██║", Color::LIGHT_YELLOW);
    DrawWTextAt(x, y + 4, L"███████║", Color::LIGHT_YELLOW);
    DrawWTextAt(x, y + 5, L"╚══════╝", Color::LIGHT_YELLOW);
}

static void DrawRankA(int x, int y)
{
    DrawWTextAt(x, y + 0, L"  █████╗  ", Color::LIGHT_YELLOW);
    DrawWTextAt(x, y + 1, L" ██╔══██╗ ", Color::LIGHT_YELLOW);
    DrawWTextAt(x, y + 2, L" ███████║ ", Color::LIGHT_YELLOW);
    DrawWTextAt(x, y + 3, L" ██╔══██║ ", Color::LIGHT_YELLOW);
    DrawWTextAt(x, y + 4, L" ██║  ██║ ", Color::LIGHT_YELLOW);
    DrawWTextAt(x, y + 5, L" ╚═╝  ╚═╝ ", Color::LIGHT_YELLOW);
}

static void DrawRankB(int x, int y)
{
    DrawWTextAt(x, y + 0, L" ██████╗  ", Color::LIGHT_GREEN);
    DrawWTextAt(x, y + 1, L" ██╔══██╗ ", Color::LIGHT_GREEN);
    DrawWTextAt(x, y + 2, L" ██████╔╝ ", Color::LIGHT_GREEN);
    DrawWTextAt(x, y + 3, L" ██╔══██╗ ", Color::LIGHT_GREEN);
    DrawWTextAt(x, y + 4, L" ██████╔╝ ", Color::LIGHT_GREEN);
    DrawWTextAt(x, y + 5, L" ╚═════╝  ", Color::LIGHT_GREEN);
}

static void DrawRankC(int x, int y)
{
    DrawWTextAt(x, y + 0, L"  ██████╗ ", Color::LIGHT_BLUE);
    DrawWTextAt(x, y + 1, L" ██╔════╝ ", Color::LIGHT_BLUE);
    DrawWTextAt(x, y + 2, L" ██║      ", Color::LIGHT_BLUE);
    DrawWTextAt(x, y + 3, L" ██║      ", Color::LIGHT_BLUE);
    DrawWTextAt(x, y + 4, L" ╚██████╗ ", Color::LIGHT_BLUE);
    DrawWTextAt(x, y + 5, L"  ╚═════╝ ", Color::LIGHT_BLUE);
}

static void DrawRankD(int x, int y)
{
    DrawWTextAt(x, y + 0, L" ██████╗  ", Color::WHITE);
    DrawWTextAt(x, y + 1, L" ██╔══██╗ ", Color::WHITE);
    DrawWTextAt(x, y + 2, L" ██║  ██║ ", Color::WHITE);
    DrawWTextAt(x, y + 3, L" ██║  ██║ ", Color::WHITE);
    DrawWTextAt(x, y + 4, L" ██████╔╝ ", Color::WHITE);
    DrawWTextAt(x, y + 5, L" ╚═════╝  ", Color::WHITE);
}

static void DrawRankF(int x, int y)
{
    DrawWTextAt(x, y + 0, L" ███████╗ ", Color::LIGHT_RED);
    DrawWTextAt(x, y + 1, L" ██╔════╝ ", Color::LIGHT_RED);
    DrawWTextAt(x, y + 2, L" █████╗   ", Color::LIGHT_RED);
    DrawWTextAt(x, y + 3, L" ██╔══╝   ", Color::LIGHT_RED);
    DrawWTextAt(x, y + 4, L" ██║      ", Color::LIGHT_RED);
    DrawWTextAt(x, y + 5, L" ╚═╝      ", Color::LIGHT_RED);
}

static void DrawRank(int score)
{
    int x = WIDTH / 4 - 5;
    int y = 15;

    char rank = GetRank(score);

    switch (rank)
    {
    case 'S':
        DrawRankS(x, y);
        break;

    case 'A':
        DrawRankA(x, y);
        break;

    case 'B':
        DrawRankB(x, y);
        break;

    case 'C':
        DrawRankC(x, y);
        break;

    case 'D':
        DrawRankD(x, y);
        break;

    case 'F':
        DrawRankF(x, y);
        break;
    }
}

static void DrawScore()
{
    if (prevShowScore == showScore)
        return;

    int centerX = WIDTH * 3 / 4;
    int boxWidth = 24;
    int boxX = centerX - boxWidth / 2;

    ClearArea(boxX, 17, boxWidth);
    ClearArea(boxX, 19, boxWidth);

    string scoreTitle = "SCORE";
    string scoreText = std::to_string(showScore);

    int titleX = centerX - (int)scoreTitle.length() / 2;
    int scoreX = centerX - (int)scoreText.length() / 2;

    GotoXY(titleX, 17);
    SetColor(Color::LIGHT_YELLOW);
    cout << scoreTitle;

    GotoXY(scoreX, 19);
    SetColor(Color::LIGHT_GREEN);
    cout << scoreText;

    SetColor();

    prevShowScore = showScore;
}

static void DrawRankComment(int score)
{
    char rank = GetRank(score);

    if (commentIndex == -1)
        commentIndex = rand() % 3;

    switch (rank)
    {
    case 'S':
    {
        string comments[3] =
        {
            "너무 졸려",
            "자고 싶어",
            "냄비"
        };

        DrawCenterText(23, comments[commentIndex], Color::LIGHT_YELLOW);
        break;
    }

    case 'A':
    {
        string comments[3] =
        {
            "좀 치네",
            "레전드 ",
            "오늘 완전 뱅이었어"
        };

        DrawCenterText(23, comments[commentIndex], Color::LIGHT_GREEN);
        break;
    }

    case 'B':
    {
        string comments[3] =
        {
            "Good!",
            "A ",
            "Niga"
        };

        DrawCenterText(23, comments[commentIndex], Color::LIGHT_BLUE);
        break;
    }

    case 'C':
    {
        string comments[3] =
        {
            ".",
            "Cㅂ",
            "Yo"
        };

        DrawCenterText(23, comments[commentIndex], Color::WHITE);
        break;
    }

    case 'D':
    {
        string comments[3] =
        {
            "저런, 그러게 잘하셨으면...이라고 쓰면 게임에 쓸꺼니? ",
            "죽어 한남 -한재훈",
            " ㅠㅠ 노력하면 점수를 올릴수 있을거에요!"
        };

        DrawCenterText(23, comments[commentIndex], Color::GRAY);
        break;
    }

    case 'F':
    {
        string comments[3] =
        {
            "게이년 ㅋ -우성태",
            "ㅉㅉ 짱애련 -정지한",
            "구라 안까고 성태보다 잘함 -한재훈"
        };

        DrawCenterText(23, comments[commentIndex], Color::LIGHT_RED);
        break;
    }
    }
}

static void DrawMenuText(int y, const string& text, bool selected)
{
    string line;

    if (selected)
        line = "> " + text + " <";
    else
        line = "  " + text + "  ";

    DrawCenterText(y, line, selected ? Color::LIGHT_YELLOW : Color::WHITE);
}

static void DrawGameoverMenu(const GameState& state)
{
    if (prevMenu == (int)state.curMenu)
        return;

    DrawMenuText(29, "Retry", state.curMenu == Menu::START);
    DrawMenuText(31, "Title", state.curMenu == Menu::INFO);

    prevMenu = (int)state.curMenu;
}

static std::pair<int, int> GetMouseConsolePos()
{
    POINT mousePos;
    GetCursorPos(&mousePos);

    HWND console = GetConsoleWindow();
    ScreenToClient(console, &mousePos);

    HANDLE output = GetStdHandle(STD_OUTPUT_HANDLE);

    CONSOLE_FONT_INFO fontInfo;
    GetCurrentConsoleFont(output, FALSE, &fontInfo);

    COORD fontSize = GetConsoleFontSize(output, fontInfo.nFont);

    return {
        mousePos.x / fontSize.X,
        mousePos.y / fontSize.Y
    };
}

static bool GetMouseDown()
{
    static bool prevClick = false;

    bool curClick = GetAsyncKeyState(VK_LBUTTON) & 0x8000;
    bool result = curClick && !prevClick;

    prevClick = curClick;

    return result;
}

static bool IsMouseInCenterArea(int y, int width)
{
    std::pair<int, int> pos = GetMouseConsolePos();

    int x = WIDTH / 2 - width / 2;

    return
        pos.first >= x &&
        pos.first <= x + width &&
        pos.second == y;
}

static void SelectGameoverMenu(GameState& state)
{
    switch (state.curMenu)
    {
    case Menu::START:
        state.curScene = Scene::INGAME;
        break;

    case Menu::INFO:
        state.curScene = Scene::TITLE;
        break;

    case Menu::QUIT:
        break;
    }
}

void InitGameover(GameState& state)
{
    showScore = 0;
    prevShowScore = -1;
    prevMenu = -1;
    commentIndex = -1;

    lastScoreTick = GetTickCount64();

    isDrawStatic = false;
    isDrawRank = false;
    isDrawComment = false;

    state.curMenu = Menu::START;
}

void UpdateGameover(GameState& state)
{
    ULONGLONG now = GetTickCount64();

    if (showScore < state.score && now - lastScoreTick >= 25)
    {
        int addScore = state.score / 60;

        if (addScore < 1)
            addScore = 1;

        showScore += addScore;

        if (showScore > state.score)
            showScore = state.score;

        lastScoreTick = now;
    }

    if (showScore < state.score)
        return;

    if (GetKeyDown(VK_UP) || GetKeyDown('W') || GetKeyDown(VK_DOWN) || GetKeyDown('S'))
    {
        switch (state.curMenu)
        {
        case Menu::START:
            state.curMenu = Menu::INFO;
            break;

        case Menu::INFO:
            state.curMenu = Menu::START;
            break;

        case Menu::QUIT:
            state.curMenu = Menu::START;
            break;
        }
    }

    if (IsMouseInCenterArea(29, 12))
    {
        state.curMenu = Menu::START;
    }

    if (IsMouseInCenterArea(31, 12))
    {
        state.curMenu = Menu::INFO;
    }

    if (GetKeyDown('R'))
    {
        state.curScene = Scene::INGAME;
    }

    if (GetKeyDown(VK_RETURN) || GetKeyDown(VK_SPACE) || GetMouseDown())
    {
        SelectGameoverMenu(state);
    }
}

void RenderGameover(const GameState& state)
{
    if (!isDrawStatic)
    {
        ClearScreenByLine();

        DrawGameOverLogo();

        isDrawStatic = true;
    }

    DrawScore();

    if (showScore >= state.score)
    {
        if (!isDrawRank)
        {
            DrawRank(state.score);
            isDrawRank = true;
        }

        if (!isDrawComment)
        {
            DrawRankComment(state.score);
            isDrawComment = true;
        }

        DrawGameoverMenu(state);
    }
}