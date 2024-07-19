#ifndef PVEGAMEPAGE_H
#define PVEGAMEPAGE_H

#include <QWidget>
#include <QPainter>
#include <QTimer>
#include <QMouseEvent>
#include <QMessageBox>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QDebug>
#include <math.h>
#include "gameplay.h"
#include <QMediaPlayer>
#include <QSoundEffect>

namespace Ui {
class PvEGamePage;
}

class PvEGamePage : public QWidget
{
    Q_OBJECT

public:
    explicit PvEGamePage(QWidget *parent = nullptr);
    ~PvEGamePage();

    int winR, winC;
    int AIR, AIC;
    int humanManType;

protected:
    // 绘制
    void paintEvent(QPaintEvent *event);
    // 监听鼠标移动情况，方便落子
    void mouseMoveEvent(QMouseEvent *event);
    // 实际落子
    void mouseReleaseEvent(QMouseEvent *event);
    //结算
    void Conclude();
    // 再来一局
    void NewGame();

    int AIWinNum = 0;
    int personWinNum = 0;
    int AIHardness = 0;
    QString AIName[3] = {"人工智（障）能", "旗鼓相当的对手", "来自底特律的AI"};
    QString PlayerName[3] = {"弱小的玩家", "挑战者", "AI杀手"};
    QString action = "QLabel{background-color:rgb(255, 244, 97);}";
    QString wait = "QLabel{background-color:(212, 212, 212);}";

private:
    GamePlay *game; // 游戏指针
    GameType game_type; // 存储游戏类型
    int clickPosRow, clickPosCol; // 存储将点击的位置

signals:
    void PVEReturen();

private slots:
    void chessOneByPerson(); // 人执行

    void chessOneByAI(); // AI下棋

    void on_radioButton_4_clicked();

    void on_radioButton_5_clicked();

    void on_radioButton_6_clicked();

    void on_PVESurrender_clicked();

    void on_PVESelect_clicked();

    void on_PVEQuit_clicked();

private:
    Ui::PvEGamePage *ui;
};

#endif // PVEGAMEPAGE_H
