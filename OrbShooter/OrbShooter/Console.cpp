#include "Console.h"

void SetUnicodeMode()
{
	(void)/*반환값무시(경고 안띄우게)*/_setmode(_fileno(stdout), _O_U16TEXT);
}

void SetDefaultMode()
{
	(void)_setmode(_fileno(stdout), _O_TEXT);
}

void RenderChar(char ch, int delayTime, bool isSkip)
{
	cout << ch;

	if (strchr("@#$%^&", ch))
	{
		if (!isSkip)
			// 스킵아닐때만
			Sleep(200);
		cout << "\b \b";

		if (!isSkip)
			// 스킵아닐때만
			Sleep(100);
	}

	if (isSkip)
		return;


	int calcDelay = delayTime + (rand() % 11 - 5);
	switch (ch)
	{
	case '!':
	case '?':
	case '.':
		calcDelay += 400; // 0.4초
		break;
	case ',':
		calcDelay += 200; // 0.2초
		break;
	case '\n':
		calcDelay += 500; // 0.5초
		break;
	case ' ':
		calcDelay += 50; // 0.05초
		break;
	}
	Sleep(std::max(0, calcDelay));
}

void RenderDialogue(const string& text, int delay)
{
	bool isSkip = false;
	for (int i = 0; i < text.size(); ++i)
	{
		if (!isSkip && _kbhit())
		{
			_getch();
			isSkip = true;
		}
		RenderChar(text[i], delay, isSkip);
	}
}

void SetConsoleGameTiltle(const wstring& title)
{
	SetConsoleTitle(title.c_str());
}

void SetConsoleWindowSize(int width, int height)
{
	//콘솔 내부에 세팅, 값을 얻거나
	//GetStdHandle(STD_OUTPUT_HANDLE);

	//콘솔 외부: 콘솔 창 자체를 건들리때
	//GetConsoleWindow();

	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
	SMALL_RECT rect = { 0,0,1,1 };
	SetConsoleWindowInfo(handle, true, &rect);

	//버퍼 해상도에 맞게 설정
	COORD size = { (short)width, (short)height };
	SetConsoleScreenBufferSize(handle, size);

	//뷰포트 맞추기
	rect.Right = width - 1;
	rect.Bottom = height - 1;
	SetConsoleWindowInfo(handle, true, &rect);

	//가운데 정렬
	HWND hwnd = GetConsoleWindow();
	CONSOLE_FONT_INFOEX fontInfo = { sizeof(CONSOLE_FONT_INFOEX) };
	GetCurrentConsoleFontEx(handle, false, &fontInfo);

	int windowWidth = fontInfo.dwFontSize.X * width;
	int windowHeight = fontInfo.dwFontSize.Y * height;

	int screenWidth = GetSystemMetrics(SM_CXSCREEN);
	int screenHeight = GetSystemMetrics(SM_CYSCREEN);

	int posX = (screenWidth - windowWidth) / 2;
	int posY = (screenHeight - windowHeight) / 2;

	SetWindowPos(hwnd, nullptr, posX, posY, 0, 0, SWP_NOSIZE);
}

void SetConsoleFullScreen()
{
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);

	CONSOLE_FONT_INFOEX fontInfo = { sizeof(CONSOLE_FONT_INFOEX) };
	GetCurrentConsoleFontEx(handle, false, &fontInfo);

	SetConsoleDisplayMode(handle, CONSOLE_FULLSCREEN_MODE, 0);

	int width = GetSystemMetrics(SM_CXSCREEN);
	int height = GetSystemMetrics(SM_CYSCREEN);
	SetConsoleWindowSize(width, height);
}

void SetConsoleWindowStyle(bool showTitleBar)
{
	HWND hWnd = GetConsoleWindow();
	LONG style = GetWindowLong(hWnd, GWL_STYLE);
	style &= ~WS_SIZEBOX & ~WS_MAXIMIZEBOX & ~WS_MINIMIZEBOX
				  & ~WS_SYSMENU; //NAND 연산
	if (!showTitleBar)
		style &= ~WS_CAPTION;
	SetWindowLong(hWnd, GWL_STYLE, style);

																						//사이즈 안바꿈, 움직이지 않음, 지금 설정창에 바로적용
	SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_FRAMECHANGED);
}

void ShakeConsoleWindow(int intensity, int duration, int interval)
{
	//intensity: 흔들림 강도 duration: 흔들리는 시간 interval: 한번 흔들고 다음 흔드는 텀
	HWND hWnd = GetConsoleWindow();
	RECT rt = {};
	GetWindowRect(hWnd, &rt);
	int originX = rt.left;
	int originY = rt.top;

	//쉐이크
	int count = duration / interval;
	for (int i = 0; i < count; ++i)
	{
		int offsetX = rand() % (2 * intensity + 1) - intensity;
		int offsetY = rand() % (2 * intensity + 1) - intensity;

		SetWindowPos(hWnd, nullptr, originX + offsetX, originY + offsetY, 0, 0, SWP_NOSIZE);
		Sleep(interval);
	}

	SetWindowPos(hWnd, nullptr, originX, originY, 0, 0, SWP_NOSIZE);
}

void SetConsoleMouseInputDisabled()
{
	HANDLE handle = GetStdHandle(STD_INPUT_HANDLE);;//수행나옴
	DWORD mode;
	GetConsoleMode(handle, &mode);
	mode &= ~ENABLE_QUICK_EDIT_MODE;
	mode &= ~ENABLE_INSERT_MODE;
	mode &= ~ENABLE_MOUSE_INPUT;

	mode |= ENABLE_EXTENDED_FLAGS;

	SetConsoleMode(handle, mode);
}

COORD GetConsoleResolution()  //////////////////////////////////////////////////////////////////
{
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO info;
	GetConsoleScreenBufferInfo(handle, &info);

	short width = info.srWindow.Right - info.srWindow.Left + 1;
	short height = info.srWindow.Bottom - info.srWindow.Top + 1;

	//return COORD{ width, height };
	return { width, height };
}

void GotoXY(int x, int y)
{
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD curPos = { x,y };
	SetConsoleCursorPosition(handle, curPos);
}

BOOL IsGotoXY(int x, int y)
{
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD curPos = { x,y };
	return SetConsoleCursorPosition(handle, curPos);
}

void SetCursorVisible(bool visible, DWORD size)
{
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO curInfo;
	curInfo.bVisible = visible; //보임 on/off
	curInfo.dwSize = size; // 두께 1 ~ 100
	SetConsoleCursorInfo(handle, &curInfo);
}

void SetColor(Color textColor, Color bgColor)
{
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
	int text = (int)textColor;
	int bg = (int)bgColor;
	SetConsoleTextAttribute(handle, (bg << 4) | text);
}

void DrawBar(int x, int y, const string& label, int value, int maxValue, int barWidth, const string& fillChar, const string& emptyChar)
{
	Color color;
	if (value > maxValue * 0.6f)
		color = Color::LIGHT_GREEN;
	else if(value > maxValue * 0.3f)
		color = Color::LIGHT_YELLOW;
	else
		color = Color::LIGHT_RED;

	GotoXY(x, y);
	SetColor();
	cout << label;

	//바 출력
	SetColor(color);
	int fillValue = barWidth * value / maxValue;
	for (int i = 0; i < barWidth; ++i)
		cout << (i < fillValue ? fillChar : emptyChar);
	SetColor();
	int digits = std::to_string(maxValue).length();

	cout << " " << std::setw(digits) << value << "/" << std::setw(digits) << maxValue << " ";
}

void DrawLine(char ch, int width)
{
	cout << std::setfill(ch) << std::setw(width)
		<< "" << std::setfill(' ');
}

void SetConsoleFont(const wstring& fontName, COORD size, UINT weight)
{
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_FONT_INFOEX info = {};
	info.cbSize = sizeof(CONSOLE_FONT_INFOEX);
	info.dwFontSize = size; // 폭, 높이
	info.FontWeight = weight; // FW_ ~ 
	// 폰트 이름 복사
	wcscpy_s(info.FaceName, fontName.c_str());
	SetCurrentConsoleFontEx(handle, false, &info);
}

bool GetKey(int vKey)
{
	//딱 이 프레임에 눌렸냐.
	return GetAsyncKeyState(vKey) & 0x8000;
}

//전역변수 금지
//전역변수 수정x -> 가능함
static bool prevDown[256] = {}; //이전 프레임에 눌렸는지 저장하는 배열
static bool curDown[256] = {}; 
bool GetKeyDown(int vKey)
{
	return curDown[vKey] && !prevDown[vKey];
	/*지금 딱 눌렀냐.
	bool cur = GetAsyncKeyState(vKey) & 0x8000;
	bool down = cur && !prevDown[vKey]; 
	prevDown[vKey] = cur;
	return down;*/
}

void UpdateInput()
{
	for(int i  = 0; i < 256; ++i)
	{
		prevDown[i] = curDown[i];
		curDown[i] = GetAsyncKeyState(i) & 0x8000;
	}
}

void FrameSync(int fps)
{
	static ULONGLONG prevTick = GetTickCount64();

	ULONGLONG curTick = GetTickCount64();
	ULONGLONG elapsed = curTick - prevTick;
	ULONGLONG targetTick = 1000 / fps;

	if(elapsed < targetTick)
		Sleep(targetTick - elapsed);
}

POINT GetMouseCellPos()
{
	POINT pt;
	GetCursorPos(&pt);
	HWND hWnd = GetConsoleWindow();
	ScreenToClient(hWnd, &pt);

	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_FONT_INFOEX fontInfo = { sizeof(CONSOLE_FONT_INFOEX) };
	GetCurrentConsoleFontEx(handle, false, &fontInfo);

	POINT cellPos = { pt.x / fontInfo.dwFontSize.X,
								  pt.y / fontInfo.dwFontSize.Y};
	return cellPos;
}

int GetCursorX()
{
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
	return csbi.dwCursorPosition.X;
}