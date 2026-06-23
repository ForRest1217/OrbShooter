#include "InGameScene.h"

void InitInGame(GameState& state)
{
    //시작 시 그리드 초기화
    for (int i = 0; i < GRID_ROWS; ++i)
    {
        for (int j = 0; j < GRID_COLS; ++j)
        {
            state.grid[i][j].color = OrbColor::NONE;
            state.grid[i][j].type = OrbType::NORMAL;
            state.grid[i][j].item = ItemType::NONE;
        }
    }

    //게임 시작 틱카운트 초기화
    ULONGLONG now = GetTickCount64();
    state.gameStartTick = now;
    state.lastDropTick = now;

    //점수 초기화
    state.score = 0;

    //슈터 위치, 각도 초기화
    state.shooter.x = WIDTH / 2;
    state.shooter.y = SHOOTER_START_Y;
    state.shooter.angle = 0.0f;
    state.shooter.prevAngle = 999.0f;
    //슈터 오브 색 초기화
    state.shooter.currentColor = GetRandomBubbleColor();
    state.shooter.nextColor = GetRandomBubbleColor();
    //슈터 오브 타입 초기화
    state.shooter.currentType = GetRandomOrbType();
    state.shooter.nextType = GetRandomOrbType();
    //슈터 공 초기화
    state.balls.clear();

    //현재 아이템 초기화
    state.currentItem = ItemType::DropFreeze;
    state.hasItem = true;
    //난사 초기화
    state.isBarrage = false;
    state.barrageEndTick = 0;
    state.barrageLastShotTick = 0;  
    //얼음 초기화
    state.isDropFreeze = false;
    state.dropFreezeEndTick = 0;

    //시작시 3줄 내려오게 하기
    for (int i = 0; i < 3; i++)
    {
        DropNewRow(state);
    }

    //벽, 그리드, 슈터 그리기
    RenderWall(state);
    RenderGrid(state);
    RenderShooter(state);
}

ULONGLONG GetCurrentDropInterval(GameState& state)
{
    ULONGLONG now = GetTickCount64();
    ULONGLONG elapsed = now - state.gameStartTick;

    ULONGLONG speedUpCount = elapsed / DROP_SPEED_UP_INTERVAL_MS;
    ULONGLONG decrease = speedUpCount * DROP_SPEED_UP_MS;

    if (decrease >= START_DROP_MS - MIN_DROP_MS)
        return MIN_DROP_MS;

    return START_DROP_MS - decrease;
}

void UpdateInGame(GameState& state)
{
     UpdateShooter(state);

    ULONGLONG now = GetTickCount64();

    //아이템 사용
    if (GetKeyDown('Z') && state.hasItem)
    {
        //난사
        if (state.currentItem == ItemType::OrbBarrage)
        {
            state.isBarrage = true;
            state.barrageEndTick = now + 5000;
            state.barrageLastShotTick = 0;
        }
        //얼음
        else if (state.currentItem == ItemType::DropFreeze)
        {
            state.isDropFreeze = true;
            state.dropFreezeEndTick = now + DROP_FREEZE_DURATION;
            state.lastDropTick = now;
        }

        state.hasItem = false;
        state.currentItem = ItemType::NONE;

        RenderWall(state);
        RenderShooter(state);
    }

    //난사시 발사
    if (state.isBarrage)
    {
        if (GetKey(VK_SPACE) && now - state.barrageLastShotTick >= BARRAGE_SHOT_INTERVAL)
        {
            state.barrageLastShotTick = now;
            ShootBall(state);
        }

        if (now >= state.barrageEndTick)
        {
            state.isBarrage = false;
            state.shooter.prevAngle = 999.0f;

            RenderWall(state);
            RenderAimLine(state);
        }
    }
    //일반 발사
    else
    {
        if (GetKeyDown(VK_SPACE))
            ShootBall(state);
    }

    //홀드 교체
    if (GetKeyDown('C'))          
        HoldBall(state);

    UpdateBall(state);

    //줄 떨어지기
    ULONGLONG dropInterval = GetCurrentDropInterval(state);
    //얼음 시 안떨어지게
    if (state.isDropFreeze)
    {
        if (now >= state.dropFreezeEndTick)
        {
            state.isDropFreeze = false;
            state.lastDropTick = now;
            RenderWall(state);
        }
    }
    //얼음 아니면 줄 떨어지기
    else
    {
        if (now - state.lastDropTick >= dropInterval)
        {
            DropNewRow(state);
            state.lastDropTick = now;
        }
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
    if (!state.isBarrage && !state.balls.empty())
        return;

    RenderAimLineByAngle(state, state.shooter.angle, true);

    float rad = state.shooter.angle * 3.141592f / 180.0f;

    Ball newBall;

    newBall.x = (float)state.shooter.x;
    newBall.y = (float)state.shooter.y - 1;

    newBall.prevX = newBall.x;
    newBall.prevY = newBall.y;

    newBall.dirX = sin(rad);
    newBall.dirY = -cos(rad);

    if (state.isBarrage)
        newBall.speed = BARRAGE_BALL_SPEED;
    else
        newBall.speed = NORMAL_BALL_SPEED;

    if (state.isBarrage)
    {
        newBall.type = OrbType::JOKER;
        newBall.color = OrbColor::JOKER;
    }
    else
    {
        newBall.type = state.shooter.currentType;

        if (newBall.type == OrbType::JOKER)
            newBall.color = OrbColor::JOKER;
        else
            newBall.color = state.shooter.currentColor;
    }

    newBall.isActive = true;
    newBall.isMoving = true;

    state.balls.push_back(newBall);

    if (!state.isBarrage)
    {
        state.shooter.currentType = state.shooter.nextType;
        state.shooter.nextType = GetRandomOrbType();

        state.shooter.currentColor = state.shooter.nextColor;
        state.shooter.nextColor = GetRandomBubbleColor();

        RenderShooter(state);
    }
}

void UpdateBall(GameState& state)
{
    for (int i = 0; i < (int)state.balls.size(); )
    {
        Ball& ball = state.balls[i];

        ball.prevX = ball.x;
        ball.prevY = ball.y;

        ball.x += ball.dirX * ball.speed;
        ball.y += ball.dirY * ball.speed;

        int offsetX = GetGridOffsetX();
        int leftWall = offsetX - 2;
        int rightWall = offsetX + GRID_COLS * 2;

        if (ball.x <= leftWall + 2)
        {
            ball.x = (float)(leftWall + 2);
            ball.dirX *= -1;
        }

        if (ball.x >= rightWall - 2)
        {
            ball.x = (float)(rightWall - 2);
            ball.dirX *= -1;
        }

        bool removeBall = false;

        if (ball.y <= 2)
        {
            ClearCell((int)ball.prevX, (int)ball.prevY);
            ClearCell((int)ball.x, (int)ball.y);

            int col = (int)((ball.x - GetGridOffsetX()) / 2.0f);
            col = max(0, min(col, GRID_COLS - 1));

            state.grid[0][col].color = ball.color;
            state.grid[0][col].type = ball.type;
            state.grid[0][col].item = ItemType::NONE;

            std::vector<std::pair<int, int>> floating = FindFloating(state);
            state.score += (int)floating.size() * 5;

            for (int j = 0; j < (int)floating.size(); ++j)
                ClearOrbAt(state, floating[j].first, floating[j].second);

            RenderGrid(state);

            ProcessMatch(state, 0, col);

            state.shooter.prevAngle = 999.0f;
            RenderAimLine(state);

            removeBall = true;
        }
        else
        {
            int hitR;
            int hitC;

            if (CheckOrbCollision(state, ball, hitR, hitC))
            {
                SnapBallToGrid(state, ball, hitR, hitC);
                removeBall = true;
            }
        }

        if (removeBall)
            state.balls.erase(state.balls.begin() + i);
        else
            ++i;
    }
}

void HoldBall(GameState& state)
{
    if (!state.balls.empty())
        return;

    RenderAimLineByAngle(state, state.shooter.angle, true);

    if (!state.shooter.hasHold)
    {
        state.shooter.holdColor = state.shooter.currentColor;
        state.shooter.currentColor = state.shooter.nextColor;
        state.shooter.nextColor = GetRandomBubbleColor();

        state.shooter.holdType = state.shooter.currentType;
        state.shooter.currentType = state.shooter.nextType;
        state.shooter.nextType = GetRandomOrbType();

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

    ShakeConsoleWindow(10, 50, 10);
    state.score += (int)connected.size() * 10;

    for (int i = 0; i < (int)connected.size(); ++i)
    {
        int br = connected[i].first;
        int bc = connected[i].second;

        if (state.grid[br][bc].type == OrbType::BOMB)
            bombs.push_back(std::make_pair(br, bc));
    }

    //아이템먹기
    for (int i = 0; i < (int)connected.size(); ++i)
        CollectNearItems(state, connected[i].first, connected[i].second);

    //오브지우기
    for (int i = 0; i < (int)connected.size(); ++i)
        ClearOrbAt(state, connected[i].first, connected[i].second);

    //폭탄오브 터트리기
    for (int i = 0; i < (int)bombs.size(); ++i)
        ExplodeAround(state, bombs[i].first, bombs[i].second);

    //떠있는 오브 지우기
    std::vector<std::pair<int, int>> floating = FindFloating(state);
    state.score += (int)floating.size() * 5;
    for (int i = 0; i < (int)floating.size(); ++i)
        ClearOrbAt(state, floating[i].first, floating[i].second);

    RenderGrid(state);
}


void RenderInGame(const GameState& state)
{
    RenderBall(state);

    if (state.balls.empty() || state.isBarrage)
    {
        if (state.shooter.prevAngle != state.shooter.angle)
            RenderAimLineByAngle(state, state.shooter.prevAngle, true);

        RenderAimLine(state);
    }

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

            if (state.grid[i][j].item == ItemType::OrbBarrage)
            {
                SetColor(Color::WHITE);
                SetUnicodeMode();
                wcout << L"▣";
                SetDefaultMode();
                SetColor();
            }
            else
            {
                SetColor(ToConsoleColor(state.grid[i][j].color));
                SetUnicodeMode();
                wcout << GetOrbShape(state.grid[i][j].type);
                SetDefaultMode();
                SetColor();
            }

            if (j < GRID_COLS - 1)
                cout << " ";
        }
    }
}

void RenderWall(const GameState& state)
{
    int offsetX = GetGridOffsetX();
    int leftWall = offsetX - 2;
    int rightWall = offsetX + GRID_COLS * 2;

    if (state.isDropFreeze)
        SetColor(Color::LIGHT_BLUE);
    else if (state.isBarrage)
        SetColor(Color::VIOLET);
    else
        SetColor();

    for (int i = leftWall; i <= rightWall; i += 2)
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

    SetColor();
}

void RenderShooter(const GameState& state)
{
    //현재공
    if (state.shooter.currentType == OrbType::JOKER)
        SetColor(ToConsoleColor(OrbColor::JOKER));
    else
        SetColor(ToConsoleColor(state.shooter.currentColor));
    SetUnicodeMode();
    GotoXY(state.shooter.x, state.shooter.y);
    wcout << GetOrbShape(state.shooter.currentType);
    SetDefaultMode();

    //조작키
    SetColor();
    GotoXY(2, 1); cout << "LEFT/RIGHT : 조준";
    GotoXY(2, 2); cout << "SPACE      : 발사";
    GotoXY(2, 3); cout << "C          : 홀드";
    GotoXY(2, 4); cout << "Z          : 아이템 사용";

    //다음공
    GotoXY(23, HEIGHT - 2);
    SetColor();
    cout << "다음 공: ";
    if (state.shooter.nextType == OrbType::JOKER)
        SetColor(ToConsoleColor(OrbColor::JOKER));
    else
        SetColor(ToConsoleColor(state.shooter.nextColor));
    SetUnicodeMode();
    wcout << GetOrbShape(state.shooter.nextType);
    SetDefaultMode();
    SetColor();

    //홀드한공
    int holdX = WIDTH - 18;
    GotoXY(holdX, HEIGHT - 2);
    SetColor();
    cout << "홀드: ";
    if (state.shooter.hasHold)
    {
        if (state.shooter.holdType == OrbType::JOKER)
            SetColor(ToConsoleColor(OrbColor::JOKER));
        else
            SetColor(ToConsoleColor(state.shooter.holdColor));
        SetUnicodeMode();
        wcout << GetOrbShape(state.shooter.holdType);
        SetDefaultMode();
    }
    else
        cout << "-";
    SetColor();

    //아이템
    int itemX = WIDTH - 18;
    GotoXY(itemX, HEIGHT - 3);
    SetColor();
    cout << "현재 아이템: ";

    if (state.hasItem)
    {
        if (state.currentItem == ItemType::OrbBarrage)
            cout << "난사 ";
        else if (state.currentItem == ItemType::DropFreeze)
            cout << "얼음  ";
    }
    else
    {
        cout << "-       ";
    }

    //점수
    GotoXY(2, HEIGHT - 2);
    cout << "점수: " << state.score << "   ";

    //알려주는거
    GotoXY(66, 1); SetUnicodeMode(); wcout << L"●"; SetDefaultMode(); cout << " : 능력 없음";
    GotoXY(66, 2); SetUnicodeMode(); wcout << L"◎"; SetDefaultMode(); cout << " : 없어질 때 주변 8칸 삭제";
    GotoXY(66, 3); SetUnicodeMode(); wcout << L"◐"; SetDefaultMode(); cout << " : 어떤 색으로든 변할 수 있음";
}

void RenderAimLine(const GameState& state)
{
    RenderAimLineByAngle(state, state.shooter.angle, false);
}

void RenderBall(const GameState& state)
{
    for (int i = 0; i < (int)state.balls.size(); ++i)
    {
        const Ball& ball = state.balls[i];

        if (!ball.isActive)
            continue;

        if ((int)ball.prevX != (int)ball.x ||
            (int)ball.prevY != (int)ball.y)
        {
            ClearCell((int)ball.prevX, (int)ball.prevY);
        }

        if (ball.type == OrbType::JOKER)
            SetColor(ToConsoleColor(OrbColor::JOKER));
        else
            SetColor(ToConsoleColor(ball.color));

        GotoXY((int)ball.x, (int)ball.y);

        SetUnicodeMode();
        wcout << GetOrbShape(ball.type);
        SetDefaultMode();
        SetColor();
    }
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

                if (state.grid[row][col].item == ItemType::OrbBarrage)
                {
                    SetColor(Color::WHITE);
                    SetUnicodeMode();
                    wcout << L"▣";
                    SetDefaultMode();
                    SetColor();
                }
                else
                {
                    SetColor(ToConsoleColor(state.grid[row][col].color));
                    SetUnicodeMode();
                    wcout << GetOrbShape(state.grid[row][col].type);
                    SetDefaultMode();
                    SetColor();
                }
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

void GetNeighbors(int r, int c, int out[4][2])
{
    //위 찾기
    out[0][0] = r - 1;
    out[0][1] = c;

    //아래 찾기
    out[1][0] = r + 1;
    out[1][1] = c;

    //왼쪽 찾기
    out[2][0] = r;
    out[2][1] = c - 1;

    //오른쪽 찾기
    out[3][0] = r;
    out[3][1] = c + 1;
}

std::vector<std::pair<int, int>> FindConnected(GameState& state, int startR, int startC)
{
    //범위 안에 없으면 넘어가기
    if (!InGrid(startR, startC))
        return std::vector<std::pair<int, int>>();

    //빈공간이거나 아이템이면 넘어가기
    if (state.grid[startR][startC].color == OrbColor::NONE ||
        state.grid[startR][startC].item != ItemType::NONE)
        return std::vector<std::pair<int, int>>();

    bool startIsJoker = state.grid[startR][startC].type == OrbType::JOKER ||state.grid[startR][startC].color == OrbColor::JOKER;

    std::vector<std::pair<int, int>> bestResult;

    int startColor = (int)OrbColor::RED;
    int endColor = (int)OrbColor::CYAN;

    if (!startIsJoker)
    {
        startColor = (int)state.grid[startR][startC].color;
        endColor = startColor;
    }

    for (int colorValue = startColor; colorValue <= endColor; ++colorValue)
    {
        OrbColor target = (OrbColor)colorValue;

        std::vector<std::pair<int, int>> result;
        std::set<std::pair<int, int>> visited;
        std::queue<std::pair<int, int>> q;

        q.push(std::make_pair(startR, startC));
        visited.insert(std::make_pair(startR, startC));

        while (!q.empty())
        {
            std::pair<int, int> cur = q.front();
            q.pop();

            int r = cur.first;
            int c = cur.second;

            result.push_back(cur);

            int nb[4][2];
            GetNeighbors(r, c, nb);
            for (int i = 0; i < 4; ++i)
            {
                int nr = nb[i][0];
                int nc = nb[i][1];

                //범위 안에 없으면 넘어가기
                if (!InGrid(nr, nc))
                    continue;

                if (visited.count(std::make_pair(nr, nc)))
                    continue;

                //빈공간이거나 아이템이면 넘어가기
                if (state.grid[nr][nc].color == OrbColor::NONE ||
                    state.grid[nr][nc].item != ItemType::NONE)
                    continue;

                //조커가 아니면서 같은색이 아닐때 넘어가기
                bool isJoker = state.grid[nr][nc].type == OrbType::JOKER || state.grid[nr][nc].color == OrbColor::JOKER;
                if (!isJoker && state.grid[nr][nc].color != target)
                    continue;

                visited.insert(std::make_pair(nr, nc));
                q.push(std::make_pair(nr, nc));
            }
        }

        if (result.size() > bestResult.size())
            bestResult = result;
    }

    return bestResult;
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

        int nb[4][2];
        GetNeighbors(r, c, nb);
        for (int i = 0; i < 4; ++i)
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
    //게임오버 판정
    for (int i = 0; i < GRID_COLS; ++i)
    {
        if (state.grid[GAMEOVER_Y - 1][i].color != OrbColor::NONE)
        {
            state.curScene = Scene::GAMEOVER;
            return;
        }
    }

    //줄들 하나씩 아래로 당기기
    for (int i = GAMEOVER_Y - 1; i >= 1; --i)
        for (int j = 0; j < GRID_COLS; ++j)
            state.grid[i][j] = state.grid[i - 1][j];

    for (int i = 0; i < GRID_COLS; ++i)
    {
        //빈칸 생성
        if (rand() % 5 == 0)
        {
            state.grid[0][i].color = OrbColor::NONE;
            state.grid[0][i].type = OrbType::NORMAL;
            state.grid[0][i].item = ItemType::NONE;
        }
        else
        {
            //랜덤 색으로 줄 생성
            state.grid[0][i].color = GetRandomBubbleColor();
            state.grid[0][i].type = OrbType::NORMAL;

            //줄에 아이템 나오게 하기
            if (rand() % ITEM_DROP_CHANCE == 0)
            {
                if (rand() % 2 == 0)
                    state.grid[0][i].item = ItemType::OrbBarrage; //난사
                else
                    state.grid[0][i].item = ItemType::DropFreeze; //얼음
            }
            else
                state.grid[0][i].item = ItemType::NONE;
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

bool CheckOrbCollision(const GameState& state, const Ball& ball, int& hitR, int& hitC)
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

            float dx = ball.x - GridToScreenX(j);
            float dy = ball.y - GridToScreenY(i);
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

void SnapBallToGrid(GameState& state, Ball& ball, int hitR, int hitC)
{
    ClearCell((int)ball.prevX, (int)ball.prevY);
    ClearCell((int)ball.x, (int)ball.y);

    int bestRow = -1;
    int bestCol = -1;
    float bestDist = 1e30f;

    bool canStickSide = false;

    if (ball.dirX > 0.35f || ball.dirX < -0.35f)
        canStickSide = true;

    int nb[4][2];
    GetNeighbors(hitR, hitC, nb);
    for (int i = 0; i < 4; ++i)
    {
        int r = nb[i][0];
        int c = nb[i][1];

        if (!InGrid(r, c))
            continue;

        if (state.grid[r][c].color != OrbColor::NONE)
            continue;

        if (!canStickSide && r == hitR)
            continue;

        float dx = ball.x - GridToScreenX(c);
        float dy = ball.y - GridToScreenY(r);
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
        int r = hitR + 1;
        int c = hitC;

        if (InGrid(r, c) && state.grid[r][c].color == OrbColor::NONE)
        {
            bestRow = r;
            bestCol = c;
        }
    }

    if (bestRow == -1)
    {
        RenderGrid(state);
        return;
    }

    state.grid[bestRow][bestCol].color = ball.color;
    state.grid[bestRow][bestCol].type = ball.type;
    state.grid[bestRow][bestCol].item = ItemType::NONE;

    RenderGrid(state);
    ProcessMatch(state, bestRow, bestCol);

    state.shooter.prevAngle = 999.0f;
    RenderAimLine(state);
}

OrbColor GetRandomBubbleColor()
{
    int value = rand() % ((int)OrbColor::CYAN - (int)OrbColor::RED + 1)  + (int)OrbColor::RED;

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
        case OrbColor::JOKER: return Color::WHITE;
        default: return Color::WHITE;
    }
}

const wchar_t* GetOrbShape(OrbType type)
{
    if (type == OrbType::BOMB)
        return L"◎";
    else if (type == OrbType::JOKER)
        return L"◐";
    else if (type == OrbType::NORMAL)
        return L"●";
}

OrbType GetRandomOrbType()
{
    if (rand() % BOMB_ORB_CHANCE == 0)
        return OrbType::BOMB;

    if (rand() % JOKER_ORB_CHANCE == 0)
        return OrbType::JOKER;

    return OrbType::NORMAL;
}

void ClearOrbAt(GameState& state, int r, int c)
{
    if (!InGrid(r, c))
        return;

    if (state.grid[r][c].item != ItemType::NONE && !state.hasItem)
    {
        state.currentItem = state.grid[r][c].item;
        state.hasItem = true;
    }

    state.grid[r][c].color = OrbColor::NONE;
    state.grid[r][c].type = OrbType::NORMAL;
    state.grid[r][c].item = ItemType::NONE;

    RenderShooter(state);
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

            ClearOrbAt(state, nr, nc);
        }
    }
}

void CollectNearItems(GameState& state, int r, int c)
{
    int nb[4][2];
    GetNeighbors(r, c, nb);

    for (int i = 0; i < 4; ++i)
    {
        int nr = nb[i][0];
        int nc = nb[i][1];

        CollectItemAt(state, nr, nc);
    }
}
void CollectItemAt(GameState& state, int r, int c)
{
    if (!InGrid(r, c))
        return;

    if (state.grid[r][c].item == ItemType::NONE ||
        state.hasItem)
        return;

    state.currentItem = state.grid[r][c].item;
    state.hasItem = true;

    state.grid[r][c].color = OrbColor::NONE;
    state.grid[r][c].type = OrbType::NORMAL;
    state.grid[r][c].item = ItemType::NONE;

    RenderShooter(state);
}