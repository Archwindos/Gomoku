#include "gameplay.h"

GamePlay::GamePlay()
{
    memset(gameBoard, 0, sizeof(int)*BOARDSIZE*BOARDSIZE);
    step = 0;
    //黑子先手，-1
    manType = -1;
}

void GamePlay::StartGame(GameType type)
{
    memset(gameBoard, 0, sizeof(int)*BOARDSIZE*BOARDSIZE);
    step = 0;
    manType = -1;
    gameStatus = PLAYING;

    if (type == PERSON) {
        //empty
        return;
    }
    else if (type == BOT) {
        //initialization for ai
        memset(scoremap, 0, sizeof(int)*BOARDSIZE*BOARDSIZE);
        return;
    }
    else {
        //initialization for online play
        return;
    }
}

int GamePlay::CurManType()
{
    return manType;
}

int GamePlay::ReadBoard(int row, int col)
{
    return gameBoard[row][col];
}

int GamePlay::ReadStep()
{
    return step;
}

bool GamePlay::ActionPerson(int raw, int col, int manTypeInput)
{
    if (manTypeInput != manType || gameBoard[raw][col]!=0) return false;
    gameBoard[raw][col] = manTypeInput;
    manType = -manType;
    step++;
    return true;
}

bool GamePlay::ActionAI(int &clickraw,int &clickcol,int manTypeInput)//mantype指棋子颜色，mantypeinput指AI所执的颜色
{
    if (manTypeInput != manType) return false;
    memset(scoremap, 0, sizeof(int)*BOARDSIZE*BOARDSIZE);
    calculatescore();
    int maxscore = 0;
    std::vector<std::pair<int, int>> maxpoints;
    for (int raw = 0; raw < BOARDSIZE; raw++)
    {
        for (int col = 0; col < BOARDSIZE; col++)
        {
            if (gameBoard[raw][col] == 0)
            {
                if (scoremap[raw][col] > maxscore)
                {
                    maxpoints.clear();
                    maxscore = scoremap[raw][col];
                    maxpoints.push_back(std::make_pair(raw, col));
                }
                else if (scoremap[raw][col] == maxscore)
                    maxpoints.push_back(std::make_pair(raw, col));
            }
        }
    }
    srand((unsigned)time(0));
    int index = rand() % maxpoints.size();
    std::pair<int, int>pointpair = maxpoints.at(index);
    gameBoard[pointpair.first][pointpair.second] = manTypeInput;
    clickraw = pointpair.first;
    clickcol = pointpair.second;
    manType = -manType;
    step++;
    return true;
}
void GamePlay::calculatescore()
{
    int personNum = 0;
    int botNum = 0;
    int emptyNum = 0;
    memset(scoremap, 0, sizeof(int)*BOARDSIZE*BOARDSIZE);
    for (int row = 0; row < BOARDSIZE; row++)
        for (int col = 0; col < BOARDSIZE; col++)
        {
            // 空白点就算
            if (row > 0 && col > 0 &&
                gameBoard[row][col] == 0)
            {
                // 遍历周围八个方向
                for (int y = -1; y <= 1; y++)
                    for (int x = -1; x <= 1; x++)
                    {
                        // 重置
                        personNum = 0;
                        botNum = 0;
                        emptyNum = 0;

                        // 原坐标不算
                        if (!(y == 0 && x == 0))
                        {
                            // 每个方向延伸4个子

                            // 对玩家白子评分（正反两个方向）
                            for (int i = 1; i <= 4; i++)
                            {
                                if (row + i * y > 0 && row + i * y < BOARDSIZE &&
                                    col + i * x > 0 && col + i * x < BOARDSIZE &&
                                    gameBoard[row + i * y][col + i * x] == -manType) // 玩家的子
                                {
                                    personNum++;
                                }
                                else if (row + i * y > 0 && row + i * y < BOARDSIZE &&
                                         col + i * x > 0 && col + i * x < BOARDSIZE &&
                                         gameBoard[row + i * y][col + i * x] == 0) // 空白位
                                {
                                    emptyNum++;
                                    break;
                                }
                                else            // 出边界
                                    break;
                            }

                            for (int i = 1; i <= 4; i++)
                            {
                                if (row - i * y > 0 && row - i * y < BOARDSIZE &&
                                    col - i * x > 0 && col - i * x < BOARDSIZE &&
                                    gameBoard[row - i * y][col - i * x] == -manType) // 玩家的子
                                {
                                    personNum++;
                                }
                                else if (row - i * y > 0 && row - i * y < BOARDSIZE &&
                                         col - i * x > 0 && col - i * x < BOARDSIZE &&
                                         gameBoard[row - i * y][col - i * x] == 0) // 空白位
                                {
                                    emptyNum++;
                                    break;
                                }
                                else            // 出边界
                                    break;
                            }

                            if (personNum == 1)                      // 杀二
                                scoremap[row][col] += 10;
                            else if (personNum == 2)                 // 杀三
                            {
                                if (emptyNum == 1)
                                    scoremap[row][col] += 30;
                                else if (emptyNum == 2)
                                    scoremap[row][col] += 40;
                            }
                            else if (personNum == 3)                 // 杀四
                            {
                                // 量变空位不一样，优先级不一样
                                if (emptyNum == 1)
                                    scoremap[row][col] += 60;
                                else if (emptyNum == 2)
                                    scoremap[row][col] += 110;
                            }
                            else if (personNum == 4)                 // 杀五
                                scoremap[row][col] += 10100;

                            // 进行一次清空
                            emptyNum = 0;

                            // 对AI黑子评分
                            for (int i = 1; i <= 4; i++)
                            {
                                if (row + i * y > 0 && row + i * y < BOARDSIZE &&
                                    col + i * x > 0 && col + i * x < BOARDSIZE &&
                                    gameBoard[row + i * y][col + i * x] == manType) // 玩家的子
                                {
                                    botNum++;
                                }
                                else if (row + i * y > 0 && row + i * y < BOARDSIZE &&
                                         col + i * x > 0 && col + i * x < BOARDSIZE &&
                                         gameBoard[row + i * y][col + i * x] == 0) // 空白位
                                {
                                    emptyNum++;
                                    break;
                                }
                                else            // 出边界
                                    break;
                            }

                            for (int i = 1; i <= 4; i++)
                            {
                                if (row - i * y > 0 && row - i * y < BOARDSIZE &&
                                    col - i * x > 0 && col - i * x < BOARDSIZE &&
                                    gameBoard[row - i * y][col - i * x] == manType) // AI的子
                                {
                                    botNum++;
                                }
                                else if (row - i * y > 0 && row - i * y < BOARDSIZE &&
                                         col - i * x > 0 && col - i * x < BOARDSIZE &&
                                         gameBoard[row - i * y][col - i * x] == 0) // 空白位
                                {
                                    emptyNum++;
                                    break;
                                }
                                else            // 出边界
                                    break;
                            }

                            if (botNum == 0)                      // 普通下子
                                scoremap[row][col] += 5;
                            else if (botNum == 1)                 // 活二
                                scoremap[row][col] += 10;
                            else if (botNum == 2)
                            {
                                if (emptyNum == 1)                // 死三
                                    scoremap[row][col] += 25;
                                else if (emptyNum == 2)
                                    scoremap[row][col] += 50;  // 活三
                            }
                            else if (botNum == 3)
                            {
                                if (emptyNum == 1)                // 死四
                                    scoremap[row][col] += 55;
                                else if (emptyNum == 2)
                                    scoremap[row][col] += 100; // 活四
                            }
                            else if (botNum >= 4)
                                scoremap[row][col] += 10000;   // 活五

                        }
                    }

            }
        }
}



bool GamePlay::IsWin(int raw, int col)
{
    //分四个轴进行搜索，最大搜索距离为XMax和YMax
    int XMaxL = col;
    int XMaxR = 14 - col;
    int YMaxU = raw;
    int YMaxD = 14 - raw;
    int manColor = gameBoard[raw][col];
    int sameColorNum1;
    int sameColorNum2;
    int sameColorNum3;
    int sameColorNum4;


    //X轴
    sameColorNum1 = 1;
    for (int i = 1; i <= XMaxL; i++)
    {
        if (gameBoard[raw][col - i] == manColor) sameColorNum1++;
        else break;
    }
    for (int i = 1; i <= XMaxR; i++)
    {
        if (gameBoard[raw][col + i] == manColor) sameColorNum1++;
        else break;
    }
    qDebug()<< sameColorNum1 ;
    if (sameColorNum1 >= 5) return true;

    //Y轴
    sameColorNum2 = 1;
    for (int i = 1; i <= YMaxU; i++)
    {
        if (gameBoard[raw - i][col] == manColor) sameColorNum2++;
        else break;
    }
    for (int i = 1; i <= YMaxD; i++)
    {
        if (gameBoard[raw + i][col] == manColor) sameColorNum2++;
        else break;
    }
    qDebug()<< sameColorNum2 ;
    if (sameColorNum2 >= 5) return true;
    //y=x轴
    sameColorNum3 = 1;
    int rightUp = min(YMaxU, XMaxR);
    for (int i = 1; i <= rightUp; i++)
    {
        if (gameBoard[raw - i][col + i] == manColor) sameColorNum3++;
        else break;
    }
    int leftDown = min(YMaxD, XMaxL);
    for (int i = 1; i <= leftDown; i++)
    {
        if (gameBoard[raw + i][col - i] == manColor) sameColorNum3++;
        else break;
    }
    qDebug()<< sameColorNum3 ;
    if (sameColorNum3 >= 5) return true;

    //y=-x轴
    sameColorNum4 = 1;
    int leftUp = min(YMaxU, XMaxL);
    for (int i = 1; i <= leftUp; i++)
    {
        if (gameBoard[raw - i][col - i] == manColor) sameColorNum4++;
        else break;
    }
    int rightDown = min(YMaxD, XMaxR);
    for (int i = 1; i <= rightDown; i++)
    {
        if (gameBoard[raw + i][col + i] == manColor) sameColorNum4++;
        else break;
    }
    if (sameColorNum4 >= 5) return true;

    return false;
}

bool GamePlay::IsDraw()
{
    if (step == 225) return true;
    else return false;
}
