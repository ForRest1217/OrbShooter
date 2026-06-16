#pragma once
enum class Scene
{
	NONE,
	TITLE,
	INFO,
	INGAME,
	GAMEOVER
};
enum class Menu
{
	START,
	INFO,
	QUIT
};
enum class OrbColor
{
	NONE = 0,
	RED, GREEN, BLUE, YELLOW, VIOLET, CYAN,
	COUNT
};
enum class OrbType
{
	NORMAL,
	BOMB
};