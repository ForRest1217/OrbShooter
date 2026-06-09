#include "InGameScene.h"

void InitInGame(GameState& state)
{
    for (int i = 0; i < GRID_ROWS; ++i)
        for (int j = 0; j < GRID_COLS; ++j)
            state.grid[i][j].color = BubbleColor::NONE;

    state.lastDropTick = GetTickCount64();
    state.score = 0;

    state.shooter.x = WIDTH / 2;
    state.shooter.y = HEIGHT - 5;
    state.shooter.angle = 0.0f;
    state.shooter.prevAngle = 999.0f;
    state.shooter.nextColor = GetRandomBubbleColor();

    state.ball.isActive = false;
    state.ball.isMoving = false;

    RenderWall();
    RenderShooter(state);
}


void UpdateInGame(GameState& state)
{
    UpdateShooter(state);

    if (GetKeyDown(VK_SPACE))
        ShootBall(state);

    UpdateBall(state);

    ULONGLONG now = GetTickCount64();
    if (now - state.lastDropTick >= (ULONGLONG)(DROP_SEC * 1000))
    {
        DropNewRow(state);
        state.lastDropTick = now;
    }
}

void UpdateShooter(GameState& state)
{
    state.shooter.prevAngle = state.shooter.angle;

    if (GetKey(VK_LEFT))  
        state.shooter.angle -= 0.3f;
    if (GetKey(VK_RIGHT)) 
        state.shooter.angle += 0.3f;

    if (state.shooter.angle < -65.0f) 
        state.shooter.angle = -65.0f;
    if (state.shooter.angle > 65.0f) 
        state.shooter.angle = 65.0f;
}

void ShootBall(GameState& state)
{
    if (state.ball.isMoving) 
        return;

    float rad = state.shooter.angle * 3.141592f / 180.0f;

    state.ball.x = (float)state.shooter.x;
    state.ball.y = (float)state.shooter.y - 1;
    state.ball.prevX = state.ball.x;
    state.ball.prevY = state.ball.y;
    state.ball.dirX = sin(rad);
    state.ball.dirY = -cos(rad);
    state.ball.speed = 0.7f;
    state.ball.color = state.shooter.nextColor;
    state.ball.isActive = true;
    state.ball.isMoving = true;

    state.shooter.nextColor = GetRandomBubbleColor();
}

void UpdateBall(GameState& state)
{
    if (!state.ball.isMoving) 
        return;

    state.ball.prevX = state.ball.x;
    state.ball.prevY = state.ball.y;

    state.ball.x += state.ball.dirX * state.ball.speed;
    state.ball.y += state.ball.dirY * state.ball.speed;

    int offsetX = GetGridOffsetX();
    int leftWall = offsetX - 2;
    int rightWall = offsetX + GRID_COLS * 2;

    if (state.ball.x <= leftWall + 2)
    {
        state.ball.x = (float)(leftWall + 2);
        state.ball.dirX *= -1;
    }

    if (state.ball.x >= rightWall - 2)
    {
        state.ball.x = (float)(rightWall - 2);
        state.ball.dirX *= -1;
    }

    if (state.ball.y <= 2)
    {
        int col = (int)((state.ball.x - GetGridOffsetX()) / 2.0f);
        col = max(0, min(col, GRID_COLS - 2));

        state.grid[0][col].color = state.ball.color;
        state.ball.isMoving = false;
        state.ball.isActive = false;

        ProcessMatch(state, 0, col);
        return;
    }

    if (CheckBubbleCollision(state))
    {
        SnapBallToGrid(state);
    }
}

void ProcessMatch(GameState& state, int r, int c)
{
    std::vector<std::pair<int, int>> connected = FindConnected(state, r, c);
    if ((int)connected.size() < 3) 
        return;

    ShakeConsoleWindow(10, 200, 10);
    state.score += (int)connected.size() * 10;

    for (int i = 0; i < (int)connected.size(); ++i)
        state.grid[connected[i].first][connected[i].second].color = BubbleColor::NONE;

    std::vector<std::pair<int, int> > floating = FindFloating(state);
    state.score += (int)floating.size() * 5;

    for (int i = 0; i < (int)floating.size(); ++i)
        state.grid[floating[i].first][floating[i].second].color = BubbleColor::NONE;
}


void RenderInGame(const GameState& state)
{
    DrawAimLineByAngle(state, state.shooter.prevAngle, true);

    if (state.ball.isActive || state.ball.isMoving)
        ClearCell((int)state.ball.prevX, (int)state.ball.prevY);

    RenderAimLine(state);
    RenderBall(state);
    RenderShooter(state);
    RenderGrid(state);
}

void RenderGrid(const GameState& state)
{
    int offsetX = GetGridOffsetX();

    for (int i = 0; i < GRID_ROWS; ++i)
    {
        for (int j = 0; j < GRID_COLS; ++j)
        {
            GotoXY(offsetX + j * 2, i + 1); 

            if (state.grid[i][j].color == BubbleColor::NONE)
            {
                cout << "  ";
                continue;
            }

            SetColor(ToConsoleColor(state.grid[i][j].color));
            SetUnicodeMode();
            wcout << L"\u25cf";   
            SetDefaultMode();
            SetColor();
            cout << " ";
        }
    }
}

void RenderWall()
{
    int offsetX = GetGridOffsetX();
    int leftWall = offsetX - 2;
    int rightWall = offsetX + GRID_COLS * 2;    

    SetColor(Color::WHITE);

    for (int i = leftWall; i <= rightWall; ++i)
    {
        GotoXY(i, 0);
        cout << "бс";
    }

    for (int j = 1; j < HEIGHT; ++j)
    {
        GotoXY(leftWall, j);
        cout << "бс";

        GotoXY(rightWall, j);
        cout << "бс";
    }

    SetColor(Color::WHITE);
}

void RenderShooter(const GameState& state)
{
    SetColor(Color::WHITE);

    GotoXY(state.shooter.x, state.shooter.y);
    cout << "^";

    GotoXY(2, HEIGHT - 3); cout << "LEFT/RIGHT : Aim          ";
    GotoXY(2, HEIGHT - 2); cout << "SPACE      : Shoot        ";

    GotoXY(26, HEIGHT - 2);
    SetColor(Color::WHITE);
    cout << "Next: ";
    SetColor(ToConsoleColor(state.shooter.nextColor));
    SetUnicodeMode();
    wcout << L"б▄";
    SetDefaultMode();
    SetColor(Color::WHITE);

    GotoXY(2, HEIGHT - 4);
    cout << "Score: " << state.score << "   ";
}

void RenderAimLine(const GameState& state)
{
    DrawAimLineByAngle(state, state.shooter.angle, false);
}

void RenderBall(const GameState& state)
{
    if (!state.ball.isActive) 
        return;

    SetColor(ToConsoleColor(state.ball.color));
    SetUnicodeMode();
    GotoXY((int)state.ball.x, (int)state.ball.y);
    wcout << L"б▄";
    SetDefaultMode();
    SetColor(Color::WHITE);
}

void DrawAimLineByAngle(const GameState& state, float angle, bool erase)
{
    if (angle > 900.0f) 
        return;

    int offsetX = GetGridOffsetX();
    int leftWall = offsetX - 2;
    int rightWall = offsetX + GRID_COLS * 2;

    float rad = angle * 3.141592f / 180.0f;
    float dirX = sin(rad);
    float dirY = -cos(rad);

    float x = (float)state.shooter.x;
    float y = (float)state.shooter.y;

    SetColor(erase ? Color::BLACK : Color::GRAY);

    for (int i = 1; i <= 30; ++i)
    {
        int drawX = (int)(x + dirX * i);
        int drawY = (int)(y + dirY * i);

        if (drawX <= leftWall + 1 || drawX >= rightWall - 1 || drawY <= 1)
            break;

        GotoXY(drawX, drawY);
        cout << (erase ? " " : ".");
    }

    SetColor(Color::WHITE);
}


void ClearCell(int x, int y)
{
    GotoXY(x, y);
    cout << "  ";
}

bool InGrid(int r, int c)
{
    return r >= 0 && r < GRID_ROWS && c >= 0 && c < GRID_COLS;
}

void GetNeighbors(int r, int c, int out[4][2])
{
    out[0][0] = r - 1; out[0][1] = c;     
    out[1][0] = r;     out[1][1] = c - 1; 
    out[2][0] = r;     out[2][1] = c + 1; 
    out[3][0] = r + 1; out[3][1] = c;     
}

std::vector<std::pair<int, int>> FindConnected(GameState& state, int startR, int startC)
{
    BubbleColor target = state.grid[startR][startC].color;
    if (target == BubbleColor::NONE) 
        return std::vector<std::pair<int, int> >();

    std::vector<std::pair<int, int> > result;
    std::set<std::pair<int, int> >    visited;
    std::queue<std::pair<int, int> >  q;

    q.push(std::make_pair(startR, startC));
    visited.insert(std::make_pair(startR, startC));

    while (!q.empty())
    {
        std::pair<int, int> cur = q.front(); q.pop();
        int r = cur.first, c = cur.second;
        result.push_back(cur);

        int nb[4][2];
        GetNeighbors(r, c, nb);

        for (int i = 0; i < 4; ++i)
        {
            int nr = nb[i][0], nc = nb[i][1];
            if (!InGrid(nr, nc))                          
                continue;
            if (visited.count(std::make_pair(nr, nc)))    
                continue;
            if (state.grid[nr][nc].color != target)       
                continue;

            visited.insert(std::make_pair(nr, nc));
            q.push(std::make_pair(nr, nc));
        }
    }

    return result;
}

std::vector<std::pair<int, int> > FindFloating(GameState& state)
{
    std::set<std::pair<int, int> >   attached;
    std::queue<std::pair<int, int> > q;

    for (int c = 0; c < GRID_COLS; ++c)
    {
        if (state.grid[0][c].color == BubbleColor::NONE) 
            continue;
        if (attached.count(std::make_pair(0, c)))        
            continue;

        attached.insert(std::make_pair(0, c));
        q.push(std::make_pair(0, c));
    }

    while (!q.empty())
    {
        std::pair<int, int> cur = q.front(); q.pop();
        int r = cur.first, c = cur.second;

        int nb[4][2];
        GetNeighbors(r, c, nb);

        for (int i = 0; i < 4; ++i)
        {
            int nr = nb[i][0], nc = nb[i][1];
            if (!InGrid(nr, nc))                         
                continue;
            if (attached.count(std::make_pair(nr, nc)))  
                continue;
            if (state.grid[nr][nc].color == BubbleColor::NONE) 
                continue;

            attached.insert(std::make_pair(nr, nc));
            q.push(std::make_pair(nr, nc));
        }
    }

    std::vector<std::pair<int, int> > floating;
    for (int i = 0; i < GRID_ROWS; ++i)
        for (int j = 0; j < GRID_COLS; ++j)
            if (state.grid[i][j].color != BubbleColor::NONE &&
                !attached.count(std::make_pair(i, j)))
                floating.push_back(std::make_pair(i, j));

    return floating;
}

void DropNewRow(GameState& state)
{
    for (int i = 0; i < GRID_COLS; ++i)
    {
        if (state.grid[GRID_ROWS - 1][i].color != BubbleColor::NONE)
        {
            //state.curScene = Scene::GAMEOVER;
            return;
        }
    }

    for (int i = GRID_ROWS - 1; i >= 1; --i)
        for (int j = 0; j < GRID_COLS; ++j)
            state.grid[i][j] = state.grid[i - 1][j];

    for (int i = 0; i < GRID_COLS; ++i)
        state.grid[0][i].color = (rand() % 5 == 0)
                                            ? BubbleColor::NONE
                                            : GetRandomBubbleColor();
}

int GetGridOffsetX()
{
    COORD res = GetConsoleResolution();
    return (res.X - GRID_COLS * 2) / 2;
}

int GridToScreenX(int col)
{
    return GetGridOffsetX() + col * 2;
}

int GridToScreenY(int row)
{
    return row + 1  ;
}

bool CheckBubbleCollision(const GameState& state)
{
    for (int i = 0; i < GRID_ROWS; ++i)
    {
        for (int j = 0; j < GRID_COLS; ++j)
        {
            if (state.grid[i][j].color == BubbleColor::NONE) continue;

            float dx = state.ball.x - GridToScreenX(j);
            float dy = state.ball.y - GridToScreenY(i);

            if (dx * dx + dy * dy < 4.0f)
                return true;
        }
    }
    return false;
}

void SnapBallToGrid(GameState& state)
{
    int   bestRow = 0;
    int   bestCol = 0;
    float bestDist = 1e30f;

    for (int i = 0; i < GRID_ROWS; ++i)
    {
        for (int j = 0; j < GRID_COLS; ++j)
        {
            if (state.grid[i][j].color != BubbleColor::NONE) 
                continue;

            bool hasNeighbor = false;
            int nb[4][2];
            GetNeighbors(i, j, nb);

            for (int i = 0; i < 4; ++i)
            {
                int nr = nb[i][0], nc = nb[i][1];
                if (InGrid(nr, nc) && state.grid[nr][nc].color != BubbleColor::NONE)
                {
                    hasNeighbor = true;
                    break;
                }
            }
            if (!hasNeighbor) 
                continue;

            float dx = state.ball.x - GridToScreenX(j);
            float dy = state.ball.y - GridToScreenY(i);
            float dist = dx * dx + dy * dy;

            if (dist < bestDist)
            {
                bestDist = dist;
                bestRow = i;
                bestCol = j;
            }
        }
    }

    state.grid[bestRow][bestCol].color = state.ball.color;
    state.ball.isMoving = false;
    state.ball.isActive = false;

    ProcessMatch(state, bestRow, bestCol);
}

BubbleColor GetRandomBubbleColor()
{
    int value = rand() % ((int)BubbleColor::COUNT - 1) + 1;
    return (BubbleColor)value;
}

Color ToConsoleColor(BubbleColor color)
{
    switch (color)
    {
        case BubbleColor::RED: return Color::LIGHT_RED;
        case BubbleColor::GREEN: return Color::LIGHT_GREEN;
        case BubbleColor::BLUE: return Color::LIGHT_BLUE;
        case BubbleColor::YELLOW: return Color::LIGHT_YELLOW;
        case BubbleColor::VIOLET: return Color::LIGHT_VIOLET;
        case BubbleColor::CYAN: return Color::CYAN;
        default: return Color::WHITE;
    }
}