#include "TitleScene.h"
#include <utility>

static void DrawCenterText(int y, const string& text, Color color = Color::WHITE)
{
	int x = WIDTH / 2 - (int)text.length() / 2;
	GotoXY(x, y);
	SetColor(color);
	cout << text;
	SetColor();
}

static void DrawCenterUniCodeText(int y, const wstring& text, Color color = Color::WHITE)
{
	int x = WIDTH / 2 - (int)text.length() / 2;
	GotoXY(x, y);
	SetColor(color);
	SetUnicodeMode();
	wcout << text;
	SetDefaultMode();
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

static bool IsMouseInArea(std::pair<int, int> start, std::pair<int, int> end)
{
	std::pair<int, int> pos = GetMouseConsolePos();

	int x = pos.first;
	int y = pos.second;

	return
		x >= start.first &&
		x <= end.first &&
		y >= start.second &&
		y <= end.second;
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

	std::pair<int, int> startArea = { menuX, 20 };
	std::pair<int, int> infoArea = { menuX, 22 };
	std::pair<int, int> quitArea = { menuX, 24 };

	if (IsMouseInArea(startArea, { menuX + 10, 20 }))
	{
		state.curMenu = Menu::START;
	}

	if (IsMouseInArea(infoArea, { menuX + 10, 22 }))
	{
		state.curMenu = Menu::INFO;
	}

	if (IsMouseInArea(quitArea, { menuX + 10, 24 }))
	{
		state.curMenu = Menu::QUIT;
	}

	if (GetKeyDown(VK_RETURN) || GetKeyDown(VK_SPACE) || GetMouseDown())
	{
		SelectTitleMenu(state);
	}
}

void RenderTitle(const GameState& state)
{
	const wstring ascii[] =
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
	if (GetKeyDown(VK_BACK))
	{
		state.curScene = Scene::TITLE;
	}
}

void RenderInfo(const GameState& state)
{
	DrawCenterText(1, "HOW TO PLAY?", Color::LIGHT_YELLOW);


	DrawCenterText(4, "LEFT/RIGHT : 조준", Color::WHITE);
	DrawCenterText(6, "SPACE      : 발사", Color::WHITE);
	DrawCenterText(8, "C         : 홀드", Color::WHITE);
	DrawCenterText(10, "Z   : 아이템 사용", Color::WHITE);

	DrawCenterText(14, "3개의 공을 맞춰 없애 내려오는 공들을 막으세요", Color::WHITE);

	DrawCenterText(18, "<오브 종류>", Color::WHITE);
	DrawCenterUniCodeText(20, L"● : 일반 오브 - 능력 없음          ", Color::WHITE);
	DrawCenterUniCodeText(22, L"◎ : 폭탄 오브 - 없어질 때 주변 8칸 삭제", Color::WHITE);
	DrawCenterUniCodeText(24, L"◐ : 조커 오브 - 어떤 색으로든 변할 수 있음", Color::WHITE);

	DrawCenterText(28, "<아이템 종류>", Color::WHITE);
	DrawCenterUniCodeText(30, L"▣ : 아이템 박스 - 아이템 박스 주변 오브를 없앨 때 획득", Color::WHITE);
	DrawCenterText(32, "난사 : 5초간 조커 오브를 난사 가능", Color::WHITE);
	DrawCenterText(34, "얼음 : 3내려오는 오브들을 멈춤     ", Color::WHITE);

	DrawCenterText(38, "BACKSPACE : Back", Color::GRAY);
}