#include "pvegamepage.h"
#include "ui_pvegamepage.h"

const int kRadius = 20; // 棋子半径
const int kBlockSize = 50; // 格子的大小
const int lineWidth = 1;
const QPoint reference(55, 55);

PvEGamePage::PvEGamePage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::PvEGamePage)
{
    ui->setupUi(this);
    this->setFixedSize(1400, 800);
    //绘制背景贴图
    QPalette PAllbackground = this->palette();
    QImage ImgAllbackground(":/pic/secondBackground.png");
    QImage fitimgpic=ImgAllbackground.scaled(this->width(),this->height(), Qt::IgnoreAspectRatio);
    PAllbackground.setBrush(QPalette::Window, QBrush(fitimgpic));
    this->setPalette(PAllbackground);

    // 配置游戏
    AIWinNum = 0;
    personWinNum = 0;
    game = new GamePlay;
    NewGame();
    update();


}

PvEGamePage::~PvEGamePage()
{
    delete game;
    game = nullptr;
}

void PvEGamePage::NewGame()
{

    game->StartGame(BOT);
    //前端参数初始化
    clickPosRow = -2;
    clickPosCol = -2;
    winR = -1;
    winC = -1;
    //棋子计数
    ui->PVESteps->setText(QString::number(game->ReadStep()));
    ui->PVESteps->setFontPointSize(25);
    ui->PVESteps->setFontWeight(400);
    ui->PVESteps->setAlignment(Qt::AlignCenter);
    //玩家状态
    ui->PVEPlayerState->setStyleSheet(wait);
    ui->PVEPlayerState->setText("等待");
    ui->PVEAIState->setStyleSheet(wait);
    ui->PVEAIState->setText("等待");
    //控件enable
    setMouseTracking(false);
    ui->PVESelect->setEnabled(true);
    ui->PVESurrender->setEnabled(false);
    ui->PVEGroupBox->setEnabled(true);
    ui->radioButton_4->setChecked(true);
    //难度
    AIHardness = 0;
    ui->PVEAI->setText(AIName[0]);
    //头衔
    if(personWinNum > AIWinNum) ui->PVEPlayer->setText(PlayerName[2]);
    else if(personWinNum == AIWinNum) ui->PVEPlayer->setText(PlayerName[1]);
    else ui->PVEPlayer->setText(PlayerName[0]);
    //计分板
    ui->PVEPlayerScore->display(personWinNum);
    ui->PVEAIScore->display(AIWinNum);

    update();
}

void PvEGamePage::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    QPixmap blackManImage(":/pic/blackSmall.png");
    QPixmap whiteManImage(":/pic/whiteSmall.png");
    // 绘制棋盘
    painter.setRenderHint(QPainter::Antialiasing, true); // 抗锯齿
    //画线棋盘的左上顶点
    int borderR = reference.y();
    int borderC = reference.x();
    QPen pen(QColor(qRgba(0,0,0,1.0)) ,lineWidth);
    painter.setPen(pen);
    int rowIndex = 1;
    std::string colIndex = "A";
    painter.setFont(QFont("Times New Roman", 10, QFont::Bold));
    for (int i = 0; i < BOARDSIZE; i++)
    {
        painter.drawLine(borderR + kBlockSize * i, borderC, borderR + kBlockSize * i, borderC+kBlockSize * (BOARDSIZE-1));//横着
        painter.drawText(borderC - 15, borderR + kBlockSize * i+5, QString::number(rowIndex++));
        painter.drawLine(borderR, borderC + kBlockSize * i, borderR+kBlockSize * (BOARDSIZE-1), borderC + kBlockSize * i);//竖着
        painter.drawText(borderC + kBlockSize * i-5, borderR-15, QString::fromStdString(colIndex));
        colIndex[0]++;
    }

    // 绘制落子标记(防止鼠标出框越界)
    if (clickPosRow >= 0 && clickPosRow < BOARDSIZE &&
        clickPosCol >= 0 && clickPosCol < BOARDSIZE &&
        game->ReadBoard(clickPosRow, clickPosCol) == 0)
    {
        painter.setOpacity(0.7);
        if(game->CurManType() == 1){
            painter.drawPixmap(borderR + clickPosCol*kBlockSize-kRadius, borderC + clickPosRow*kBlockSize-kRadius, whiteManImage);
        }else{
            painter.drawPixmap(borderR + clickPosCol*kBlockSize-kRadius, borderC + clickPosRow*kBlockSize-kRadius, blackManImage);
        }
    }

    //绘制棋子
    painter.setOpacity(1.00);
    for (int i = 0; i < BOARDSIZE; i++){
        for (int j = 0; j < BOARDSIZE; j++)
        {
            if (game->ReadBoard(i, j) == -1){
                painter.drawPixmap(borderC + j*kBlockSize-kRadius, borderR + i*kBlockSize-kRadius, blackManImage);

            }
            else if (game->ReadBoard(i, j) == 1){
                painter.drawPixmap(borderC + j*kBlockSize-kRadius, borderR + i*kBlockSize-kRadius, whiteManImage);
            }
        }
    }
    if(winC != -1){
        QBrush brush;
        brush.setStyle(Qt::SolidPattern);
        brush.setColor(Qt::red);
        painter.setBrush(brush);
        painter.drawEllipse(borderC + kBlockSize * winC - kRadius/2, borderR + kBlockSize * winR - kRadius/2, kRadius, kRadius);
    }
}

void PvEGamePage::mouseMoveEvent(QMouseEvent *event)
{
    // 均以相对棋盘刻度线起始位置计算
    // 鼠标当前的坐标
    int x = event->pos().x()-reference.x();
    int y = event->pos().y()-reference.y();

    //边缘+半格之外的区域不能落子
    clickPosRow = -2; // 初始化最终的值
    clickPosCol = -2;
    if (x <= kBlockSize * (BOARDSIZE-1) + kBlockSize / 2 &&
        x >= - kBlockSize / 2 &&
        y <= kBlockSize * (BOARDSIZE-1) + kBlockSize / 2 &&
        y >= - kBlockSize / 2)
    {
        // 获取最近的左上角的点索引
        int col = x / kBlockSize;
        int row = y / kBlockSize;
        // 左上角的点坐标
        int leftTopPosX = kBlockSize * col;
        int leftTopPosY = kBlockSize * row;

        // 根据距离算出合适的点击位置，一共四个点，根据半径距离选最近的
        // 确定一个误差在范围内的点，且只可能确定一个出来
        int deltaX = x-(leftTopPosX+kBlockSize/2);
        int deltaY = y-(leftTopPosY+kBlockSize/2);
        if(deltaX<0 && deltaY<0){
            clickPosRow = row;
            clickPosCol = col;
        }else if(deltaX>=0 && deltaY<0){
            clickPosRow = row;
            clickPosCol = col+1;
        }else if(deltaX<0 && deltaY>=0){
            clickPosRow = row+1;
            clickPosCol = col;
        }else{
            clickPosRow = row + 1;
            clickPosCol = col + 1;
        }
    }

    // 存了坐标后也要重绘
    update();
}

void PvEGamePage::mouseReleaseEvent(QMouseEvent *event)
{
    int x = event->pos().x()-reference.x();
    int y = event->pos().y()-reference.y();
    AIR = clickPosRow;
    AIC = clickPosCol;

    //边缘+半格之外的区域不能落子
    if (x <= kBlockSize * (BOARDSIZE-1) + kBlockSize / 2 &&
        x >= - kBlockSize / 2 &&
        y <= kBlockSize * (BOARDSIZE-1) + kBlockSize / 2 &&
        y >= - kBlockSize / 2 &&
        game->CurManType() == -1 &&
        game->ReadBoard(clickPosRow, clickPosCol) == 0)
    {
        //black给人用
        chessOneByPerson();
        qDebug()<<"Person coordinate: "<<"("<<clickPosRow<<", "<<clickPosCol<<")";
        if(game->gameStatus != PLAYING){
            Conclude();
            return;
        }

        chessOneByAI();
        qDebug()<<"AI coordinate: "<<"("<<AIR<<", "<<AIC<<")";
        if(game->gameStatus != PLAYING) {
            Conclude();
            return;
        }
        ui->PVESteps->setText(QString::number(game->ReadStep()));
        ui->PVESteps->setFontPointSize(25);
        ui->PVESteps->setFontWeight(400);
        ui->PVESteps->setAlignment(Qt::AlignCenter);
        qDebug()<<"——————————";
    }
}

void PvEGamePage::chessOneByPerson()
{
    if (clickPosRow != -2 && clickPosCol != -2 && game->ReadBoard(clickPosRow, clickPosCol) == 0)
    {
        qDebug()<<"Person color correct:"<<game->ActionPerson(clickPosRow, clickPosCol, -1);//校验棋子颜色是否正确
        update();
        if(game->IsWin(clickPosRow, clickPosCol) == true){
            game->gameStatus = BLACKWIN;
            winC = clickPosCol;
            winR = clickPosRow;
        }else if(game->IsDraw()){
            game->gameStatus = DRAW;
        }
        QSoundEffect * startSound2 = new QSoundEffect(this);//创建对象
        startSound2->setSource(QUrl::fromLocalFile(":/bgm/in.wav"));;//添加资源
        startSound2->setLoopCount(1);//设置循环次数int；
        startSound2->play();//软件启动自动播放
    }
}

void PvEGamePage::chessOneByAI()
{
    qDebug()<<"AI color correct:"<<game->ActionAI(AIR, AIC, 1);
    update();
    if(game->IsWin(AIR, AIC) == true){
        game->gameStatus = WHITEWIN;
        winC = AIC;
        winR = AIR;
    }else if(game->IsDraw()){
        game->gameStatus = DRAW;
    }
}

void PvEGamePage::Conclude()
{
    if(game->gameStatus == BLACKWIN){
        QSoundEffect * startSound2 = new QSoundEffect(this);//创建对象
        startSound2->setSource(QUrl::fromLocalFile(":/bgm/win.wav"));;//添加资源
        startSound2->setLoopCount(1);//设置循环次数int；
        startSound2->play();//软件启动自动播放
        personWinNum++;
        ui->PVEPlayerScore->display(personWinNum);
        QMessageBox::warning(this,tr("获胜"),tr("你的饭碗不可撼动"),QMessageBox::Ok);


    }else if(game->gameStatus == WHITEWIN){
        QSoundEffect * startSound2 = new QSoundEffect(this);//创建对象
        startSound2->setSource(QUrl::fromLocalFile(":/bgm/lose.wav"));;//添加资源
        startSound2->setLoopCount(1);//设置循环次数int；
        startSound2->play();//软件启动自动播放
        AIWinNum++;
        ui->PVEAIScore->display(AIWinNum);
        QMessageBox::warning(this,tr("战败"),tr("这就是AI，你要失业了朋友"),QMessageBox::Ok);

    }else{
        QMessageBox::warning(this,tr("平局"),tr("平分秋色"),QMessageBox::Ok);
    }

    NewGame();
}

void PvEGamePage::on_radioButton_4_clicked()
{
    AIHardness = 0;
    ui->PVEAI->setText(AIName[AIHardness]);
}


void PvEGamePage::on_radioButton_5_clicked()
{
    AIHardness = 1;
    ui->PVEAI->setText(AIName[AIHardness]);
}



void PvEGamePage::on_radioButton_6_clicked()
{
    AIHardness = 2;
    ui->PVEAI->setText(AIName[AIHardness]);
}



void PvEGamePage::on_PVESurrender_clicked()
{
    game->gameStatus = WHITEWIN;
    Conclude();
}


void PvEGamePage::on_PVESelect_clicked()
{
    ui->PVESelect->setEnabled(false);
    ui->PVESurrender->setEnabled(true);
    ui->PVEGroupBox->setEnabled(false);
    setMouseTracking(true);

    ui->PVEPlayerState->setStyleSheet(action);
    ui->PVEPlayerState->setText("行动");
    ui->PVEAIState->setStyleSheet(wait);
    ui->PVEAIState->setText("等待");
}


void PvEGamePage::on_PVEQuit_clicked()
{
    AIWinNum = 0;
    personWinNum = 0;
    NewGame();
    emit(PVEReturen());
}
