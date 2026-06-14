#include "TitleScene.h"

static void DrawCenterText(int y, const string& text, Color color = Color::WHITE)
{
	int x = WIDTH / 2 - (int)text.length() / 2;
	GotoXY(x, y);
	SetColor(color);
	cout << text;
	SetColor();
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

void InitTitle(GameState& state)
{
	system("cls");
	state.curMenu = Menu::START;
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

	if (GetKeyDown(VK_RETURN) || GetKeyDown(VK_SPACE))
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
}

void RenderTitle(const GameState& state)
{
	const wstring ascii[] =
	{
		L" ███████              █████      █████████  █████                         █████                     ",
		L"███░░░░░███           ░░███      ███░░░░░███░░███                         ░░███                     ",
		L"███     ░░███ ████████  ░███████ ░███    ░░░  ░███████    ██████   ██████  ███████    ██████  ████████ ",
		L"░███      ░███░░███░░███ ░███░░███░░█████████  ░███░░███  ███░░███ ███░░███░░░███░    ███░░███░░███░░███",
		L"░███      ░███ ░███ ░░░  ░███ ░███ ░░░░░░░░███ ░███ ░███ ░███ ░███░███ ░███  ░███    ░███████  ░███ ░░░ ",
		L"░░███     ███  ░███      ░███ ░███ ███    ░███ ░███ ░███ ░███ ░███░███ ░███  ░███ ███░███░░░   ░███     ",
		L" ░░░███████░   █████     ████████ ░░█████████  ████ █████░░██████ ░░██████   ░░█████ ░░██████  █████    ",
		L"   ░░░░░░░    ░░░░░     ░░░░░░░░   ░░░░░░░░░  ░░░░ ░░░░░  ░░░░░░   ░░░░░░     ░░░░░   ░░░░░░  ░░░░░     "
	};

	COORD res = GetConsoleResolution();
	SetUnicodeMode();
	SetColor(Color::LIGHT_YELLOW);

	int titleX = 0;
	int titleY = 5;

	for (int i = 0; i < 8; ++i)
	{
		GotoXY(titleX, titleY + i);
		wcout << ascii[i];
	}

	SetDefaultMode();
	SetColor();

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
	if (GetKeyDown(VK_ESCAPE) || GetKeyDown(VK_BACK) || GetKeyDown(VK_SPACE))
	{
		state.curScene = Scene::TITLE;
	}
}

void RenderInfo(const GameState& state)
{
	DrawCenterText(6, "HOW TO PLAY?", Color::LIGHT_YELLOW);

	DrawCenterText(12, "LEFT / RIGHT : Aim", Color::WHITE);
	DrawCenterText(14, "SPACE        : Shoot", Color::WHITE);
	DrawCenterText(16, "Match 3 same color bubbles!", Color::WHITE);

	DrawCenterText(26, "BACKSPACE : Back", Color::GRAY);
}