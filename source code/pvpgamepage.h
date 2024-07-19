#ifndef PVPGAMEPAGE_H
#define PVPGAMEPAGE_H

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
#include <QSoundEffect>

namespace Ui {
class PvPGamePage;
}

class PvPGamePage : public QWidget
{
    Q_OBJECT

public:
    explicit PvPGamePage(QWidget *parent = nullptr);
    ~PvPGamePage();

    int winR, winC;

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
    // 换人
    void Change();

    int PlayWinNum_1 = 0;
    int PlayWinNum_2 = 0;
    int PlayerManType_1;
    int PlayerManType_2;
    int curManType;
    QString action = "QLabel{background-color:rgb(255, 244, 97);}";
    QString wait = "QLabel{background-color:(212, 212, 212);}";

private:
    GamePlay *game; // 游戏指针
    GameType game_type; // 存储游戏类型
    int clickPosRow, clickPosCol; // 存储将点击的位置

signals:
    void PVPReturen();

private slots:
    void chessOneByPerson();// 人执行=

    void on_Quit_clicked();

    void on_PlayerChoose_1_currentIndexChanged(int index);

    void on_PlayerChoose_2_currentIndexChanged(int index);

    void on_Fight_clicked();

    void on_Surrender_1_clicked();

    void on_Surrender_2_clicked();

private:
    Ui::PvPGamePage *ui;
};

#endif // PVPGAMEPAGE_H
