#include "Game.h"
#include "Console.h"
#include "SoundManager.h"
#include "InGameScene.h" 

void Init(GameState& state)
{
	srand((unsigned int)time(nullptr));
	SetConsoleWindowSize(WIDTH, HEIGHT);
	SetConsoleWindowStyle(true);
	SetConsoleMouseInputDisabled();
	state = GameState();
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
