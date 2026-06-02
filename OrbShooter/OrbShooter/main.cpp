#include "Console.h"
#include "SoundManager.h"
#include "Game.h"

int main()
{
	GameState state;
	Init(state);
	while (state.isRunning)
	{
		Update(state);
		Render(state);
		FrameSync(60);
	}
	SOUND->Release();
	return 0;
}