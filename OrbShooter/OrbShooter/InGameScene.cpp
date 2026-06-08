#include "GameState.h"
#include "Console.h"
#include <queue>
#include <set>
#include "InGameScene.h"

void InitInGame(GameState& state)
{
    for (int r = 0; r < GRID_ROWS; ++r)
        for (int c = 0; c < GRID_COLS; ++c)
            state.grid[r][c].color = BubbleColor::NONE;

    state.lastDropTick = GetTickCount64();
    state.score = 0;
}

bool InGrid(int r, int c)
{
    return r >= 0 && r < GRID_ROWS
        && c >= 0 && c < GRID_COLS;
}

void GetNeighbors(int r, int c, int out[6][2])
{
    out[0][0] = r - 1; out[0][1] = c;
    out[1][0] = r + 1; out[1][1] = c;
    out[2][0] = r;     out[2][1] = c - 1;
    out[3][0] = r;     out[3][1] = c + 1;
    out[4][0] = r - 1; out[4][1] = c - 1;
    out[5][0] = r - 1; out[5][1] = c + 1;
}

std::vector<std::pair<int, int>> FindConnected(GameState& state, int startR, int startC)
{
    BubbleColor target = state.grid[startR][startC].color;
    if (target == BubbleColor::NONE) return {};
    std::vector<std::pair<int, int>> result;
    std::set<std::pair<int, int>>    visited;
    std::queue<std::pair<int, int>>  q;
    q.push({ startR, startC });
    visited.insert({ startR, startC });
    while (!q.empty())
    {
        auto [r, c] = q.front(); q.pop();
        result.push_back({ r, c });
        int nb[6][2];
        GetNeighbors(r, c, nb);
        for (auto& n : nb)
        {
            int nr = n[0], nc = n[1];
            if (!InGrid(nr, nc))                    continue;
            if (visited.count({ nr, nc }))          continue;
            if (state.grid[nr][nc].color != target) continue;
            visited.insert({ nr, nc });
            q.push({ nr, nc });
        }
    }
    return result;
}

std::set<std::pair<int, int>> FindFloating(GameState& state)
{
    std::set<std::pair<int, int>>   attached;
    std::queue<std::pair<int, int>> q;
    for (int c = 0; c < GRID_COLS; ++c)
    {
        if (state.grid[0][c].color == BubbleColor::NONE) continue;
        if (attached.count({ 0, c }))                    continue;
        attached.insert({ 0, c });
        q.push({ 0, c });
    }
    while (!q.empty())
    {
        auto [r, c] = q.front(); q.pop();
        int nb[6][2];
        GetNeighbors(r, c, nb);
        for (auto& n : nb)
        {
            int nr = n[0], nc = n[1];
            if (!InGrid(nr, nc))                               continue;
            if (attached.count({ nr, nc }))                    continue;
            if (state.grid[nr][nc].color == BubbleColor::NONE) continue;
            attached.insert({ nr, nc });
            q.push({ nr, nc });
        }
    }
    std::set<std::pair<int, int>> floating;
    for (int r = 0; r < GRID_ROWS; ++r)
        for (int c = 0; c < GRID_COLS; ++c)
            if (state.grid[r][c].color != BubbleColor::NONE
                && !attached.count({ r, c }))
                floating.insert({ r, c });
    return floating;
}

void ProcessMatch(GameState& state, int r, int c)
{
    auto connected = FindConnected(state, r, c);

    if (connected.size() < 3)
        return;

    ShakeConsoleWindow(10, 200, 10);

    state.score += (int)connected.size() * 10;

    for (auto& [pr, pc] : connected)
        state.grid[pr][pc].color = BubbleColor::NONE;

    auto floating = FindFloating(state);

    state.score += (int)floating.size() * 5;

    for (auto& [pr, pc] : floating)
        state.grid[pr][pc].color = BubbleColor::NONE;
}

void DropNewRow(GameState& state)
{
    for (int c = 0; c < GRID_COLS; ++c)
        if (state.grid[GRID_ROWS - 1][c].color != BubbleColor::NONE)
        {
            state.curScene = Scene::GAMEOVER;
            return;
        }
    for (int r = GRID_ROWS - 1; r >= 1; --r)
        for (int c = 0; c < GRID_COLS; ++c)
            state.grid[r][c] = state.grid[r - 1][c];
    for (int c = 0; c < GRID_COLS; ++c)
        state.grid[0][c].color =   (rand() % 5 == 0)
        ? BubbleColor::NONE
        : (BubbleColor)(rand() % ((int)BubbleColor::COUNT - 1) + 1);

    COORD res = GetConsoleResolution();
    int offsetX = (res.X - GRID_COLS * 2) / 2;
    for (int y = -5; y < 0; ++y)
    {
        for (int c = 0; c < GRID_COLS; ++c)
        {
            GotoXY(offsetX + c * 2, y + 5);
            cout << "O ";
        }

        Sleep(30);

        for (int c = 0; c < GRID_COLS; ++c)
        {
            GotoXY(offsetX + c * 2, y + 5);
            cout << "  ";
        }
    }
}


void RenderInGame(const GameState& state)
{
    COORD res = GetConsoleResolution();

    int offsetX = (res.X - GRID_COLS * 2) / 2;

    for (int r = 0; r < GRID_ROWS; ++r)
        for (int c = 0; c < GRID_COLS; ++c)
        {
            GotoXY(offsetX + c * 2, r);

            if (state.grid[r][c].color == BubbleColor::NONE)
            {
                cout << "  ";
                continue;
            }

            switch (state.grid[r][c].color)
            {
            case BubbleColor::RED:    SetColor(Color::LIGHT_RED);    break;
            case BubbleColor::GREEN:  SetColor(Color::LIGHT_GREEN);  break;
            case BubbleColor::BLUE:   SetColor(Color::LIGHT_BLUE);   break;
            case BubbleColor::YELLOW: SetColor(Color::LIGHT_YELLOW); break;
            case BubbleColor::VIOLET: SetColor(Color::LIGHT_VIOLET); break;
            case BubbleColor::CYAN:   SetColor(Color::CYAN);         break;
            }

            cout << "O ";
            SetColor();
        }
}

void UpdateInGame(GameState& state)
{
    ULONGLONG now = GetTickCount64();
    if (now - state.lastDropTick >= (ULONGLONG)(DROP_SEC * 1000))
    {
        DropNewRow(state);
        state.lastDropTick = now;
    }
}