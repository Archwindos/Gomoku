#ifndef GAMEPLAY_H
#define GAMEPLAY_H
#include <CString>
#include <cstdlib>
#include <QDebug>
#include <time.h>
#include <vector>

#define min(x,y) (x<y?x:y)
#define BOARDSIZE 15
#define aggressivefactor 0.5 //进攻性因子，取值范围0-1，默认0.5

// 游戏类型，本地双人、AI、联网
enum GameType
{
    PERSON,
    BOT,
    ONLINE
};

// 游戏状态
enum GameStatus
{
    PLAYING,
    BLACKWIN,
    WHITEWIN,
    DRAW
};
//

//黑子先行
class GamePlay
{
public:
    GamePlay();

    void StartGame(GameType type);
    bool ActionPerson(int raw, int col, int manType);
    bool ActionAI(int &,int &,int);
    void calculatescore();
    bool IsWin(int raw, int col);
    bool IsDraw();
    int CurManType();
    int ReadStep();
    int ReadBoard(int, int);

    GameStatus gameStatus;

protected:
    //1白, -1黑
    int gameBoard[BOARDSIZE][BOARDSIZE];
    int step;
    int manType;
    //AI parameters
    int scoremap[BOARDSIZE][BOARDSIZE];
};

#endif // GAMEPLAY_H
