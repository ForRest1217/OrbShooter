#include "TitleScene.h"
#include "ConsoleUI.h"
#include "ConsoleUI.h"

void InitTitle(GameState& state)
{
    system("cls");
    state.curMenu = Menu::START;
}

static void SelectTitleMenu(GameState& state)
{
    switch (state.curMenu)
    {
    case Menu::START:
        state.curScene = Scene::INGAME;
        break;

    case Menu::INFO:
        state.curScene = Scene::INFO;
        break;

    case Menu::QUIT:
        state.isRunning = false;
        break;
    }
}

void UpdateTitle(GameState& state)
{
    if (GetKeyDown(VK_UP) || GetKeyDown('W'))
    {
        switch (state.curMenu)
        {
        case Menu::START:
            state.curMenu = Menu::QUIT;
            break;

        case Menu::INFO:
            state.curMenu = Menu::START;
            break;

        case Menu::QUIT:
            state.curMenu = Menu::INFO;
            break;
        }
    }

    if (GetKeyDown(VK_DOWN) || GetKeyDown('S'))
    {
        switch (state.curMenu)
        {
        case Menu::START:
            state.curMenu = Menu::INFO;
            break;

        case Menu::INFO:
            state.curMenu = Menu::QUIT;
            break;

        case Menu::QUIT:
            state.curMenu = Menu::START;
            break;
        }
    }

    int menuX = WIDTH / 2 - 5;
    bool mouseOnMenu = false;

    if (IsMouseInArea(menuX, 20, menuX + 10, 20))
    {
        state.curMenu = Menu::START;
        mouseOnMenu = true;
    }

    if (IsMouseInArea(menuX, 22, menuX + 10, 22))
    {
        state.curMenu = Menu::INFO;
        mouseOnMenu = true;
    }

    if (IsMouseInArea(menuX, 24, menuX + 10, 24))
    {
        state.curMenu = Menu::QUIT;
        mouseOnMenu = true;
    }

    if (GetKeyDown(VK_RETURN) || GetKeyDown(VK_SPACE) || mouseOnMenu && GetMouseDown())
    {
        SelectTitleMenu(state);
    }
}

void RenderTitle(const GameState& state)
{
    const wchar_t* ascii[] =
    {
        L"  ███████              █████      █████████  █████                         █████                     ",
        L" ███░░░░░███           ░░███      ███░░░░░███░░███                         ░░███                     ",
        L" ███     ░░███ ████████  ░███████ ░███    ░░░  ░███████    ██████   ██████  ███████    ██████  ████████ ",
        L"░███      ░███░░███░░███ ░███░░███░░█████████  ░███░░███  ███░░███ ███░░███░░░███░    ███░░███░░███░░███",
        L"░███      ░███ ░███ ░░░  ░███ ░███ ░░░░░░░░███ ░███ ░███ ░███ ░███░███ ░███  ░███    ░███████  ░███ ░░░ ",
        L"░░███     ███  ░███      ░███ ░███ ███    ░███ ░███ ░███ ░███ ░███░███ ░███  ░███ ███░███░░░   ░███     ",
        L" ░░░███████░   █████     ████████ ░░█████████  ████ █████░░██████ ░░██████   ░░█████ ░░██████  █████    ",
        L"   ░░░░░░░    ░░░░░     ░░░░░░░░   ░░░░░░░░░  ░░░░ ░░░░░  ░░░░░░   ░░░░░░     ░░░░░   ░░░░░░  ░░░░░     "
    };

    int titleX = 0;
    int titleY = 5;

    for (int i = 0; i < 8; ++i)
    {
        DrawWTextAt(titleX, titleY + i, ascii[i], Color::LIGHT_YELLOW);
    }

    DrawMenuText(20, "START", state.curMenu == Menu::START);
    DrawMenuText(22, "INFO", state.curMenu == Menu::INFO);
    DrawMenuText(24, "QUIT", state.curMenu == Menu::QUIT);

    DrawCenterText(30, "UP / DOWN : Select", Color::GRAY);
    DrawCenterText(31, "ENTER / SPACE : Decide", Color::GRAY);
}

void InitInfo(GameState& state)
{
    system("cls");
}

void UpdateInfo(GameState& state)
{
    if (GetKeyDown(VK_BACK))
    {
        state.curScene = Scene::TITLE;
    }
}

void RenderInfo(const GameState& state)
{
    DrawCenterText(6, "HOW TO PLAY?", Color::LIGHT_YELLOW);

    DrawCenterText(12, "LEFT / RIGHT : Aim", Color::WHITE);
    DrawCenterText(14, "SPACE        : Shoot!!", Color::WHITE);
    DrawCenterText(16, "Match 3 same color bubbles!", Color::WHITE);

    DrawCenterText(26, "BACKSPACE : Back", Color::GRAY);
}