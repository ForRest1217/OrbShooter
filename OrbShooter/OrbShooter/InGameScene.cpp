#include "InGameScene.h"

void InitInGame(GameState& state)
{
    for (int i = 0; i < GRID_ROWS; ++i)
    {
        for (int j = 0; j < GRID_COLS; ++j)
        {
            state.grid[i][j].color = OrbColor::NONE;
            state.grid[i][j].type = OrbType::NORMAL;
        }
    }

    state.lastDropTick = GetTickCount64();
    state.score = 0;

    state.shooter.x = WIDTH / 2;
    state.shooter.y = SHOOTER_START_Y;
    state.shooter.angle = 0.0f;
    state.shooter.prevAngle = 999.0f;

    state.shooter.currentColor = GetRandomBubbleColor();
    state.shooter.nextColor = GetRandomBubbleColor();

    state.shooter.currentType = GetRandomBubbleType();
    state.shooter.nextType = GetRandomBubbleType();

    state.ball.isActive = false;
    state.ball.isMoving = false;

    for (int i = 0; i < 3; i++)
    {
        DropNewRow(state);
    }

    RenderWall();
    RenderGrid(state);
    RenderShooter(state);
}


void UpdateInGame(GameState& state)
{
    UpdateShooter(state);

    if (GetKeyDown(VK_SPACE))
        ShootBall(state);

    if (GetKeyDown('C'))          
        HoldBall(state);

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

    RenderAimLineByAngle(state, state.shooter.angle, true);

    float rad = state.shooter.angle * 3.141592f / 180.0f;

    state.ball.x = (float)state.shooter.x;
    state.ball.y = (float)state.shooter.y - 1;

    state.ball.prevX = state.ball.x;
    state.ball.prevY = state.ball.y;

    state.ball.dirX = sin(rad);
    state.ball.dirY = -cos(rad);

    state.ball.speed = 0.05f;


    state.ball.color = state.shooter.currentColor;
    state.shooter.currentColor = state.shooter.nextColor;
    state.shooter.nextColor = GetRandomBubbleColor();

    state.ball.type = state.shooter.currentType;
    state.shooter.currentType = state.shooter.nextType;
    state.shooter.nextType = GetRandomBubbleType();


    state.ball.isActive = true;
    state.ball.isMoving = true;
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
        ClearCell((int)state.ball.prevX, (int)state.ball.prevY);
        ClearCell((int)state.ball.x, (int)state.ball.y);

        int col = (int)((state.ball.x - GetGridOffsetX()) / 2.0f);
        col = max(0, min(col, GRID_COLS - 2));

        state.grid[0][col].color = state.ball.color;
        state.grid[0][col].type = state.ball.type;

        std::vector<std::pair<int, int>> floating = FindFloating(state);
        state.score += (int)floating.size() * 5;

        for (int i = 0; i < (int)floating.size(); ++i)
            ClearBubbleAt(state, floating[i].first, floating[i].second);
        RenderGrid(state);

        state.ball.isMoving = false;
        state.ball.isActive = false;

        ProcessMatch(state, 0, col);

        state.shooter.prevAngle = 999.0f;
        RenderAimLine(state);

        return;
    }

    int hitR;
    int hitC;
    if (CheckBubbleCollision(state, hitR, hitC))
    {
        SnapBallToGrid(state, hitR, hitC);
    }
}

void HoldBall(GameState& state)
{
    if (state.ball.isMoving)
        return;

    RenderAimLineByAngle(state, state.shooter.angle, true);

    if (!state.shooter.hasHold)
    {
        state.shooter.holdColor = state.shooter.currentColor;
        state.shooter.currentColor = state.shooter.nextColor;
        state.shooter.nextColor = GetRandomBubbleColor();

        state.shooter.holdType = state.shooter.currentType;
        state.shooter.currentType = state.shooter.nextType;
        state.shooter.nextType = GetRandomBubbleType();

        state.shooter.hasHold = true;
    }
    else
    {
        OrbColor tempColor = state.shooter.currentColor;
        state.shooter.currentColor = state.shooter.holdColor;
        state.shooter.holdColor = tempColor;

        OrbType tempType = state.shooter.currentType;
        state.shooter.currentType = state.shooter.holdType;
        state.shooter.holdType = tempType;
    }

    state.shooter.prevAngle = 999.0f;
    RenderShooter(state);
    RenderAimLine(state);
}

void ProcessMatch(GameState& state, int r, int c)
{
    std::vector<std::pair<int, int>> connected = FindConnected(state, r, c);

    if ((int)connected.size() < 3)
        return;

    std::vector<std::pair<int, int>> bombs;

    ShakeConsoleWindow(10, 200, 10);
    state.score += (int)connected.size() * 10;

    for (int i = 0; i < (int)connected.size(); ++i)
    {
        int br = connected[i].first;
        int bc = connected[i].second;

        if (state.grid[br][bc].type == OrbType::BOMB)
            bombs.push_back(std::make_pair(br, bc));
    }

    for (int i = 0; i < (int)connected.size(); ++i)
        ClearBubbleAt(state, connected[i].first, connected[i].second);

    for (int i = 0; i < (int)bombs.size(); ++i)
        ExplodeAround(state, bombs[i].first, bombs[i].second);

    std::vector<std::pair<int, int>> floating = FindFloating(state);
    state.score += (int)floating.size() * 5;

    for (int i = 0; i < (int)floating.size(); ++i)
        ClearBubbleAt(state, floating[i].first, floating[i].second);

    RenderGrid(state);
}


void RenderInGame(const GameState& state)
{
    if (state.shooter.prevAngle != state.shooter.angle)
    {
        if (!state.ball.isMoving)
        {
            RenderAimLineByAngle(state, state.shooter.prevAngle, true);
            RenderAimLine(state);
        }
    }

    RenderBall(state);
    RenderShooter(state);
}

void RenderGrid(const GameState& state)
{
    int offsetX = GetGridOffsetX();

    for (int i = 0; i < GRID_ROWS; ++i)
    {
        for (int j = 0; j < GRID_COLS; ++j)
        {
            GotoXY(offsetX + j * 2, i + 1); 

            if (state.grid[i][j].color == OrbColor::NONE)
            {
                if (j == GRID_COLS - 1)
                    cout << " ";
                else
                    cout << "  ";
                continue;
            }

            SetColor(ToConsoleColor(state.grid[i][j].color));
            SetUnicodeMode();
            wcout << GetBubbleShape(state.grid[i][j].type);
            SetDefaultMode();
            SetColor();

            if (j < GRID_COLS - 1)
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

    for (int i = leftWall; i <= rightWall; i+=2)
    {
        GotoXY(i, 0);
        cout << "■";
    }

    for (int j = 1; j < HEIGHT; ++j)
    {
        GotoXY(leftWall, j);
        cout << "■";

        GotoXY(rightWall, j);
        cout << "■";
    }

    SetColor(Color::WHITE);
}

void RenderShooter(const GameState& state)
{
    //현재공
    SetColor(ToConsoleColor(state.shooter.currentColor)); 
    SetUnicodeMode();
    GotoXY(state.shooter.x, state.shooter.y);
    wcout << GetBubbleShape(state.shooter.currentType);
    SetDefaultMode();
    SetColor();

    GotoXY(2, HEIGHT - 4); cout << "LEFT/RIGHT : Aim          ";
    GotoXY(2, HEIGHT - 3); cout << "SPACE      : Shoot        ";
    GotoXY(2, HEIGHT - 2); cout << "C          : Hold        ";

    //다음공
    GotoXY(26, HEIGHT - 2);
    SetColor(Color::WHITE);
    cout << "Next: ";
    SetColor(ToConsoleColor(state.shooter.nextColor));
    SetUnicodeMode();
    wcout << GetBubbleShape(state.shooter.nextType);
    SetDefaultMode();
    SetColor(Color::WHITE);

    //홀드한공
    int holdX = WIDTH - 18;
    GotoXY(holdX, HEIGHT - 2);
    SetColor(Color::WHITE);
    cout << "Hold: ";
    if (state.shooter.hasHold)
    {
        SetColor(ToConsoleColor(state.shooter.holdColor));
        SetUnicodeMode();
        wcout << GetBubbleShape(state.shooter.holdType);
        SetDefaultMode();
    }
    else
        cout << "-";
    SetColor();

    GotoXY(2, HEIGHT - 5);
    cout << "Score: " << state.score << "   ";
}

void RenderAimLine(const GameState& state)
{
    RenderAimLineByAngle(state, state.shooter.angle, false);
}

void RenderBall(const GameState& state)
{
    if (!state.ball.isActive) 
        return;

    if ((int)state.ball.prevX != (int)state.ball.x ||
        (int)state.ball.prevY != (int)state.ball.y)
    {
        ClearCell((int)state.ball.prevX, (int)state.ball.prevY);
    }

    SetColor(ToConsoleColor(state.ball.color));
    GotoXY((int)state.ball.x,(int)state.ball.y);

    SetUnicodeMode();
    wcout << GetBubbleShape(state.ball.type);
    SetDefaultMode();
}

void RenderAimLineByAngle(const GameState& state, float angle, bool erase)
{
    if (angle > 900.0f)
        return;

    int offsetX = GetGridOffsetX();
    int leftWall = offsetX - 2;
    int rightWall = offsetX + GRID_COLS * 2;

    float rad = angle * 3.141592f / 180.0f;

    float simX = (float)state.shooter.x;
    float simY = (float)state.shooter.y;

    float simDirX = sin(rad);
    float simDirY = -cos(rad);

    SetColor(erase ? Color::BLACK : Color::WHITE);

    for (int i = 0; i < 60; ++i)
    {
        simX += simDirX;
        simY += simDirY;

        if (simX <= leftWall + 2)
        {
            simX = leftWall + 2;
            simDirX *= -1;
        }

        if (simX >= rightWall - 2)
        {
            simX = rightWall - 2;
            simDirX *= -1;
        }

        int drawX = (int)simX;
        int drawY = (int)simY;

        if (drawY <= 1)
            break;

        int row = drawY - 1;
        int col = (drawX - offsetX) / 2;

        if (row >= 0 && row < GRID_ROWS && col >= 0 && col < GRID_COLS &&
            state.grid[row][col].color != OrbColor::NONE)
        {
            if (erase)
            {
                GotoXY(GridToScreenX(col), GridToScreenY(row));
                SetColor(ToConsoleColor(state.grid[row][col].color));
                SetUnicodeMode();
                wcout << GetBubbleShape(state.grid[row][col].type);
                SetDefaultMode();
               SetColor();
            }
            else
                break;

            continue;
        }

        float currentAngle = atan2(simDirX, -simDirY) * 180.0f / 3.141592f;
        wchar_t aimChar = erase ? L' ' : GetAimChar(currentAngle);
        GotoXY(drawX, drawY);
        SetUnicodeMode();
        wcout << aimChar;
        SetDefaultMode();
    }

    SetColor();
}

wchar_t GetAimChar(float angle)
{
    if (angle < -45) 
        return L'⋱';
    if (angle < -15) 
        return L'⋱';
    if (angle < 15)  
        return L'│';
    if (angle < 45)  
        return L'⋰';

    return L'⋰';
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

void GetNeighbors(int r, int c, int out[6][2])
{
    out[0][0] = r;
    out[0][1] = c - 1;

    out[1][0] = r;
    out[1][1] = c + 1;

    out[2][0] = r - 1;
    out[3][0] = r - 1;

    out[4][0] = r + 1;
    out[5][0] = r + 1;

    if (r % 2 == 0)
    {
        out[2][1] = c - 1;
        out[3][1] = c;

        out[4][1] = c - 1;
        out[5][1] = c;
    }
    else
    {
        out[2][1] = c;
        out[3][1] = c + 1;

        out[4][1] = c;
        out[5][1] = c + 1;
    }
}

std::vector<std::pair<int, int>> FindConnected(GameState& state, int startR, int startC)
{
    OrbColor target = state.grid[startR][startC].color;
    if (target == OrbColor::NONE) 
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

        int nb[6][2];
        GetNeighbors(r, c, nb);

        for (int i = 0; i < 6; ++i)
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
        if (state.grid[0][c].color == OrbColor::NONE) 
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

        int nb[6][2];
        GetNeighbors(r, c, nb);

        for (int i = 0; i < 6; ++i)
        {
            int nr = nb[i][0], nc = nb[i][1];
            if (!InGrid(nr, nc))                         
                continue;
            if (attached.count(std::make_pair(nr, nc)))  
                continue;
            if (state.grid[nr][nc].color == OrbColor::NONE) 
                continue;

            attached.insert(std::make_pair(nr, nc));
            q.push(std::make_pair(nr, nc));
        }
    }

    std::vector<std::pair<int, int> > floating;
    for (int i = 0; i < GRID_ROWS; ++i)
        for (int j = 0; j < GRID_COLS; ++j)
            if (state.grid[i][j].color != OrbColor::NONE &&
                !attached.count(std::make_pair(i, j)))
                floating.push_back(std::make_pair(i, j));

    return floating;
}

void DropNewRow(GameState& state)
{
    for (int i = 0; i < GRID_COLS; ++i)
    {
        if (state.grid[GRID_ROWS - 1][i].color != OrbColor::NONE)
        {
            state.curScene = Scene::GAMEOVER;
            return;
        }
    }

    for (int i = GRID_ROWS - 1; i >= 1; --i)
        for (int j = 0; j < GRID_COLS; ++j)
            state.grid[i][j] = state.grid[i - 1][j];

    for (int i = 0; i < GRID_COLS; ++i)
    {
        if (rand() % 5 == 0)
        {
            state.grid[0][i].color = OrbColor::NONE;
            state.grid[0][i].type = OrbType::NORMAL;
        }
        else
        {
            state.grid[0][i].color = GetRandomBubbleColor();
            state.grid[0][i].type = OrbType::NORMAL;
        }
    }

    RenderGrid(state);
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

bool CheckBubbleCollision(const GameState& state, int& hitR, int& hitC)
{
    hitR = -1;
    hitC = -1;

    float bestDist = 4.0f;

    for (int i = 0; i < GRID_ROWS; ++i)
    {
        for (int j = 0; j < GRID_COLS; ++j)
        {
            if (state.grid[i][j].color == OrbColor::NONE)
                continue;

            float dx = state.ball.x - GridToScreenX(j);
            float dy = state.ball.y - GridToScreenY(i);
            float dist = dx * dx + dy * dy;

            if (dist < bestDist)
            {
                bestDist = dist;
                hitR = i;
                hitC = j;
            }
        }
    }

    return hitR != -1;
}

void SnapBallToGrid(GameState& state, int hitR, int hitC)
{
    ClearCell((int)state.ball.prevX, (int)state.ball.prevY);
    ClearCell((int)state.ball.x, (int)state.ball.y);

    int bestRow = -1;
    int bestCol = -1;
    float bestDist = 1e30f;

    float moveAngle = atan2(state.ball.dirX, -state.ball.dirY) * 180.0f / 3.141592f;
    float absAngle = fabs(moveAngle);

    const float SIDE_ATTACH_ANGLE = 20.0f;

    int nb[6][2];
    GetNeighbors(hitR, hitC, nb);

    for (int i = 0; i < 6; ++i)
    {
        int r = nb[i][0];
        int c = nb[i][1];

        if (!InGrid(r, c))
            continue;

        if (state.grid[r][c].color != OrbColor::NONE)
            continue;

        if (absAngle < SIDE_ATTACH_ANGLE && r == hitR)
            continue;

        float dx = state.ball.x - GridToScreenX(c);
        float dy = state.ball.y - GridToScreenY(r);
        float dist = dx * dx + dy * dy;

        if (dist < bestDist)
        {
            bestDist = dist;
            bestRow = r;
            bestCol = c;
        }
    }

    if (bestRow == -1)
    {
        state.ball.isMoving = false;
        state.ball.isActive = false;
        RenderGrid(state);
        return;
    }

    state.grid[bestRow][bestCol].color = state.ball.color;
    state.grid[bestRow][bestCol].type = state.ball.type;

    state.ball.isMoving = false;
    state.ball.isActive = false;

    RenderGrid(state);
    ProcessMatch(state, bestRow, bestCol);

    state.shooter.prevAngle = 999.0f;
    RenderAimLine(state);
}

OrbColor GetRandomBubbleColor()
{
    int value = rand() % ((int)OrbColor::COUNT - 1) + 1;
    return (OrbColor)value;
}

Color ToConsoleColor(OrbColor color)
{
    switch (color)
    {
        case OrbColor::RED: return Color::LIGHT_RED;
        case OrbColor::GREEN: return Color::LIGHT_GREEN;
        case OrbColor::BLUE: return Color::LIGHT_BLUE;
        case OrbColor::YELLOW: return Color::LIGHT_YELLOW;
        case OrbColor::VIOLET: return Color::LIGHT_VIOLET;
        case OrbColor::CYAN: return Color::CYAN;
        default: return Color::WHITE;
    }
}

const wchar_t* GetBubbleShape(OrbType type)
{
    if (type == OrbType::BOMB)
        return L"◎";

    return L"●";
}

OrbType GetRandomBubbleType()
{
    if (rand() % 5 == 0)
        return OrbType::BOMB;

    return OrbType::NORMAL;
}

void ClearBubbleAt(GameState& state, int r, int c)
{
    if (!InGrid(r, c))
        return;

    state.grid[r][c].color = OrbColor::NONE;
    state.grid[r][c].type = OrbType::NORMAL;
}

void ExplodeAround(GameState& state, int r, int c)
{
    for (int i = -1; i <= 1; ++i)
    {
        for (int j = -1; j <= 1; ++j)
        {
            if (i == 0 && j == 0)
                continue;

            int nr = r + i;
            int nc = c + j;

            if (!InGrid(nr, nc))
                continue;

            ClearBubbleAt(state, nr, nc);
        }
    }
}
