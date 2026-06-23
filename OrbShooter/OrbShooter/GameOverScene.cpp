#include "GameOverScene.h"
#include "ConsoleUI.h"
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
            "오늘 밤 바라본~~",
            "저 달이 너무 처량해~~",
            "너도 나처럼~~ 외~~로운 텅 빈 가슴 안고  사는구나~~"
        };

        DrawCenterText(23, comments[commentIndex], Color::LIGHT_YELLOW);
        break;
    }

    case 'A':
    {
        string comments[3] =
        {
            " ",
            " ",
            "ㄴ"
        };

        DrawCenterText(23, comments[commentIndex], Color::LIGHT_GREEN);
        break;
    }

    case 'B':
    {
        string comments[3] =
        {
            "애매하게 잘했네♡ 허접치고는 봐줄 만해~",
            "잘했는데 자랑하긴 애매하네♡ 허접한테는 큰 발전인가?",
            "아쉽게도 여기까지네♡ 허접 실력 귀여워♡"
        };

        DrawCenterText(23, comments[commentIndex], Color::LIGHT_BLUE);
        break;
    }

    case 'C':
    {
        string comments[3] =
        {
            "이제 눈을 뜨고 플레이 해주세요!",
            "으음… 조금만 더 힘내볼까요?",
            "괜찮아요. 다음 판이 있잖아요."
        };

        DrawCenterText(23, comments[commentIndex], Color::WHITE);
        break;
    }

    case 'D':
    {
        string comments[3] =
        {
            "저런, 그러게 잘하셨으면... 라고하면 게임에 쓸거니? -장준민",
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