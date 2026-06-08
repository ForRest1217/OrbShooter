#include "Game.h"
#include "Console.h"
#include "SoundManager.h"
#include "InGameScene.h" 

#include <cmath>
#include <cstdlib>
#include <ctime>

void InitInGame(GameState& state);
void UpdateInGame(GameState& state);
void RenderInGame(const GameState& state);

OrbColor GetRandomOrbColor();
Color ToConsoleColor(OrbColor color);

void UpdateShooter(GameState& state);
void ShootBall(GameState& state);
void UpdateBall(GameState& state);

void RenderWall();
void RenderShooter(const GameState& state);
void RenderAimLine(const GameState& state);
void RenderBall(const GameState& state);

void ClearCell(int x, int y);
void ClearPrevBall(const GameState& state);
void ClearPrevAimLine(const GameState& state);
void DrawAimLineByAngle(const GameState& state, float angle, bool erase);

void Init(GameState& state)
{
	srand((unsigned int)time(nullptr));
	SetConsoleWindowSize(WIDTH, HEIGHT);
	SetConsoleWindowStyle(true);
	SetConsoleMouseInputDisabled();
	SetCursorVisible(false);

	state = GameState();
	state.shooter.nextColor = GetRandomOrbColor();

	SOUND->Init();
}
void Update(GameState& state)
{
	SOUND->Update();

	bool sceneChanged = state.prevScene != state.curScene;
	state.prevScene = state.curScene;

	UpdateInput();

	if (GetKeyDown(VK_ESCAPE))
		state.isRunning = false;

	switch (state.curScene)
	{
	case Scene::TITLE:
		if (sceneChanged)
			//InitTitle();
		//UpdateTitle(state);
		break;
	case Scene::INFO:
		//UpdateInfo(state);
		break;
	case Scene::INGAME:
		if (sceneChanged)
			InitInGame(state);
		UpdateInGame(state);
		break;
	case Scene::GAMEOVER:
		break;
	}
}

void Render(const GameState& state)
{
	if (state.prevScene != state.curScene)
		system("cls");
	GotoXY(0, 0);
	switch (state.curScene)
	{
	case Scene::TITLE:
		//RenderTitle(state);
		break;
	case Scene::INFO:
		//RenderInfo(state);
		break;
	case Scene::INGAME:
		RenderInGame(state);
		break;
	case Scene::GAMEOVER:
		break;
	}

}
void InitInGame(GameState& state)
{
	if (state.prevScene != state.curScene)
		system("cls");
	state.shooter.x = WIDTH / 2;
	state.shooter.y = HEIGHT - 5;
	state.shooter.angle = 0.0f;
	state.shooter.prevAngle = 0.0f;

	state.ball.isActive = false;
	state.ball.isMoving = false;

	RenderWall();
	RenderShooter(state);
}
OrbColor GetRandomOrbColor()
{
	int value = rand() % 4;

	switch (value)
	{
	case 0:
		return OrbColor::RED;
	case 1:
		return OrbColor::BLUE;
	case 2:
		return OrbColor::GREEN;
	default:
		return OrbColor::YELLOW;
	}
}

Color ToConsoleColor(OrbColor color)
{
	switch (color)
	{
	case OrbColor::RED:
		return Color::LIGHT_RED;
	case OrbColor::BLUE:
		return Color::LIGHT_BLUE;
	case OrbColor::GREEN:
		return Color::LIGHT_GREEN;
	case OrbColor::YELLOW:
		return Color::LIGHT_YELLOW;
	default:
		return Color::WHITE;
	}
}

void UpdateShooter(GameState& state)
{
	state.shooter.prevAngle = state.shooter.angle;

	if (GetKey(VK_LEFT))
	{
		state.shooter.angle -= 2.0f;
	}

	if (GetKey(VK_RIGHT))
	{
		state.shooter.angle += 2.0f;
	}

	if (state.shooter.angle < -65.0f)
	{
		state.shooter.angle = -65.0f;
	}

	if (state.shooter.angle > 65.0f)
	{
		state.shooter.angle = 65.0f;
	}
}

void ShootBall(GameState& state)
{
	if (state.ball.isMoving)
	{
		return;
	}

	float rad = state.shooter.angle * 3.141592f / 180.0f;

	state.ball.x = (float)state.shooter.x;
	state.ball.y = (float)state.shooter.y - 1;

	state.ball.dirX = sin(rad);
	state.ball.dirY = -cos(rad);

	state.ball.speed = 0.7f;
	state.ball.color = state.shooter.nextColor;

	state.ball.isActive = true;
	state.ball.isMoving = true;

	state.shooter.nextColor = GetRandomOrbColor();
}
void UpdateBall(GameState& state)
{
	if (!state.ball.isMoving)
		return;

	state.ball.prevX = state.ball.x;
	state.ball.prevY = state.ball.y;

	state.ball.x += state.ball.dirX * state.ball.speed;
	state.ball.y += state.ball.dirY * state.ball.speed;

	int leftWall = 1;
	int rightWall = WIDTH - 2;
	int topWall = 1;

	if (state.ball.x <= leftWall)
	{
		state.ball.x = (float)leftWall;
		state.ball.dirX *= -1.0f;
	}

	if (state.ball.x >= rightWall)
	{
		state.ball.x = (float)rightWall;
		state.ball.dirX *= -1.0f;
	}

	if (state.ball.y <= topWall)
	{
		state.ball.y = (float)topWall;
		state.ball.isMoving = false;
	}
}	
void UpdateInGame(GameState& state)
{
	UpdateShooter(state);

	if (GetKeyDown(VK_SPACE))
	{
		ShootBall(state);
	}

	UpdateBall(state);
}
void RenderWall()
{
	SetColor(Color::WHITE);

	for (int x = 0; x < WIDTH; x++)
	{
		GotoXY(x, 0);
		cout << "#";
	}

	for (int y = 0; y < HEIGHT; y++)
	{
		GotoXY(0, y);
		cout << "#";

		GotoXY(WIDTH - 1, y);
		cout << "#";
	}

	SetColor(Color::WHITE);
}
void RenderAimLine(const GameState& state)
{
	DrawAimLineByAngle(state, state.shooter.angle, false);
}
void RenderShooter(const GameState& state)
{
	SetColor(Color::WHITE);

	GotoXY(state.shooter.x, state.shooter.y);
	cout << "^";

	GotoXY(2, HEIGHT - 3);
	cout << "LEFT / RIGHT : Aim";

	GotoXY(2, HEIGHT - 2);
	cout << "SPACE : Shoot";

	GotoXY(30, HEIGHT - 2);
	cout << "Next : ";

	SetColor(ToConsoleColor(state.shooter.nextColor));

	SetUnicodeMode();
	wcout << L"◯";
	SetDefaultMode();

	SetColor(Color::WHITE);
}
void RenderBall(const GameState& state)
{
	if (!state.ball.isActive)
		return;

	SetColor(ToConsoleColor(state.ball.color));

	SetUnicodeMode();

	GotoXY((int)state.ball.x, (int)state.ball.y);
	wcout << L"◯";

	SetDefaultMode();

	SetColor(Color::WHITE);
}
void RenderInGame(const GameState& state)
{
	ClearPrevAimLine(state);
	ClearPrevBall(state);

	RenderAimLine(state);
	RenderBall(state);

	RenderShooter(state);
}
void ClearCell(int x, int y)
{
	GotoXY(x, y);
	cout << " ";
}
void ClearPrevBall(const GameState& state)
{
	if (!state.ball.isActive)
		return;

	ClearCell((int)state.ball.prevX, (int)state.ball.prevY);
}
void DrawAimLineByAngle(const GameState& state, float angle, bool erase)
{
	float rad = angle * 3.141592f / 180.0f;

	float dirX = sin(rad);
	float dirY = -cos(rad);

	float x = (float)state.shooter.x;
	float y = (float)state.shooter.y;

	if (erase)
	{
		SetColor(Color::WHITE);
	}
	else
	{
		SetColor(Color::GRAY);
	}

	for (int i = 1; i <= 12; i++)
	{
		int drawX = (int)(x + dirX * i * 2);
		int drawY = (int)(y + dirY * i);

		if (drawX <= 1 || drawX >= WIDTH - 2 || drawY <= 1)
		{
			break;
		}

		GotoXY(drawX, drawY);

		if (erase)
			cout << " ";
		else
			cout << ".";
	}

	SetColor(Color::WHITE);
}
void ClearPrevAimLine(const GameState& state)
{
	DrawAimLineByAngle(state, state.shooter.prevAngle, true);
}


