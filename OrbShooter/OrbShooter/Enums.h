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
	JOKER,
	COUNT
};
enum class OrbType
{
	NORMAL,
	BOMB,
	JOKER
};
enum class ItemType
{
	NONE,
	OrbBarrage,
	DropFreeze
};