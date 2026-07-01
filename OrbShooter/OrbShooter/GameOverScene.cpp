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
    if (score >= 2500)
        return 'S';

    if (score >= 1800)
        return 'A';

    if (score >= 1200)
        return 'B';

    if (score >= 700)
        return 'C';

    if (score >= 300)
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
            "기~가~막~히~네 - 정현진",
            "옛날사람들은 완전히 멘탈이 나가버렸습니다",
            "당신은 전설입니다"
        };

        DrawCenterText(23, comments[commentIndex], Color::LIGHT_YELLOW);
        break;
    }

    case 'A':
    {
        string comments[3] =
        {
            "A등급 맛있네요 - 노현우",
            "아듘",
            "정말 잘하시네요!"
        };

        DrawCenterText(23, comments[commentIndex], Color::LIGHT_GREEN);
        break;
    }

    case 'B':
    {
        string comments[3] =
        {
            "ㄲㅂ 아깝다 - 우성태",
            "야 ~ 호- 조윤규",
            "발전했어요"
        };

        DrawCenterText(23, comments[commentIndex], Color::LIGHT_BLUE);
        break;
    }

    case 'C':
    {
        string comments[3] =
        {
            "이제 눈을 뜨고 플레이 해주세요!",
            "준민이니? - 김예루"
            "더 힘내세요"
        };

        DrawCenterText(23, comments[commentIndex], Color::WHITE);
        break;
    }

    case 'D':
    {
        string comments[3] =
        {
            "아직 손이 안풀림 - 신동완",
            "죽어 -한재훈",
            "와 어렵네 이거 - 유현우"
        };

        DrawCenterText(23, comments[commentIndex], Color::GRAY);
        break;
    }

    case 'F':
    {
        string comments[3] =
        {
            "허♡접 -우성태",
            "저런, 그러게 잘하셨으면... 라고하면 게임에 쓸거니 ? -장준민",
            "사람 아니야 ㅠㅠ"
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
    SOUND->StopBGM();
    SOUND->Play("Dead");

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