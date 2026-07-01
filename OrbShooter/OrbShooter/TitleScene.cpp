#include "TitleScene.h"
#include "ConsoleUI.h"

void InitTitle(GameState& state)
{
    system("cls");
    state.curMenu = Menu::START;
        
    const wchar_t* ascii[] =
    {
        L"  ███████              █████       █████████ █████                         █████                     ",
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
    DrawCenterText(1, "HOW TO PLAY?", Color::LIGHT_YELLOW);


    DrawCenterText(4, "LEFT/RIGHT : 조준", Color::WHITE);
    DrawCenterText(6, "CTRL : 미세 조준", Color::WHITE);
    DrawCenterText(8, "SPACE      : 발사", Color::WHITE);
    DrawCenterText(10, "C         : 홀드", Color::WHITE);
    DrawCenterText(12, "Z   : 아이템 사용", Color::WHITE);

    DrawCenterText(15, "3개의 공을 맞춰 제거해 내려오는 공들을 막으세요", Color::WHITE);
    DrawCenterText(17, "공이 제거될 때에는 공중에 뜬 공들도 함께 제거됩니다", Color::WHITE);

    DrawCenterText(20, "<오브 종류>", Color::WHITE);
    DrawCenterWText(22, L"● : 일반 오브 - 능력 없음          ", Color::WHITE);
    DrawCenterWText(24, L"◎ : 폭탄 오브 - 없어질 때 주변 8칸 삭제", Color::WHITE);
    DrawCenterWText(26, L"◐ : 조커 오브 - 어떤 색으로든 변할 수 있음", Color::WHITE);

    DrawCenterText(28, "<아이템 종류>", Color::WHITE);
    DrawCenterWText(30, L"▣ : 아이템 박스 - 아이템 박스 주변 오브를 없앨 때 획득", Color::WHITE);
    DrawCenterText(32, "난사 : 5초간 조커 오브를 난사 가능", Color::WHITE);
    DrawCenterText(34, "얼음 : 3초간 내려오는 오브들을 멈춤", Color::WHITE);

    DrawCenterText(38, "BACKSPACE : Back", Color::GRAY);
}