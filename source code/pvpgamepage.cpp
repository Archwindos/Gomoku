#include "pvpgamepage.h"
#include "ui_pvpgamepage.h"

const int kRadius = 20; // 棋子半径
const int kBlockSize = 50; // 格子的大小
const int lineWidth = 1;
const QPoint reference(55, 55);

PvPGamePage::PvPGamePage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::PvPGamePage)
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
    PlayWinNum_1 = 0;
    PlayWinNum_2 = 0;
    game = new GamePlay;
    NewGame();
    update();
}

PvPGamePage::~PvPGamePage()
{
    delete ui;
    delete game;
}

void PvPGamePage::NewGame()
{
    game->StartGame(PERSON);
    //前端参数初始化
    clickPosRow = -2;
    clickPosCol = -2;
    winR = -1;
    winC = -1;
    curManType = -1;//黑子先出
    //棋子计数
    ui->Steps->setText(QString::number(game->ReadStep()));
    ui->Steps->setFontPointSize(25);
    ui->Steps->setFontWeight(400);
    ui->Steps->setAlignment(Qt::AlignCenter);
    //玩家状态
    ui->PlayerState_1->setStyleSheet(wait);
    ui->PlayerState_1->setText("等待");
    ui->PlayerState_2->setStyleSheet(wait);
    ui->PlayerState_2->setText("等待");
    ui->PlayerChoose_1->setCurrentIndex(0);//黑子
    ui->PlayerChoose_2->setCurrentIndex(1);//白子
    //控件enable
    setMouseTracking(false);
    ui->SelectMan->setEnabled(true);
    ui->Table->setEnabled(false);
    //计分板
    ui->PlayerScore_1->display(PlayWinNum_1);
    ui->PlayerScore_2->display(PlayWinNum_2);
    //棋子插图
    ui->ManImage_1->setStyleSheet("border-image:url(:/pic/unknown.png)");
    ui->ManImage_2->setStyleSheet("border-image:url(:/pic/unknown.png)");

    update();
}

void PvPGamePage::paintEvent(QPaintEvent *event)
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

void PvPGamePage::mouseMoveEvent(QMouseEvent *event)
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

void PvPGamePage::mouseReleaseEvent(QMouseEvent *event){
    int x = event->pos().x()-reference.x();
    int y = event->pos().y()-reference.y();

    //边缘+半格之外的区域不能落子
    if (x <= kBlockSize * (BOARDSIZE-1) + kBlockSize / 2 &&
        x >= - kBlockSize / 2 &&
        y <= kBlockSize * (BOARDSIZE-1) + kBlockSize / 2 &&
        y >= - kBlockSize / 2 &&
        game->ReadBoard(clickPosRow, clickPosCol) == 0)
    {
        chessOneByPerson();
        if(game->gameStatus != PLAYING){
            Conclude();
            return;
        }else{
            Change();
        }

        ui->Steps->setText(QString::number(game->ReadStep()));
        ui->Steps->setFontPointSize(25);
        ui->Steps->setFontWeight(400);
        ui->Steps->setAlignment(Qt::AlignCenter);
    }
}

void PvPGamePage::chessOneByPerson()
{
    if (clickPosRow != -2 && clickPosCol != -2 && game->ReadBoard(clickPosRow, clickPosCol) == 0)
    {
        QSoundEffect * startSound2 = new QSoundEffect(this);//创建对象
        startSound2->setSource(QUrl::fromLocalFile(":/bgm/in.wav"));;//添加资源
        startSound2->setLoopCount(1);//设置循环次数int；
        startSound2->play();//软件启动自动播放
        game->ActionPerson(clickPosRow, clickPosCol, curManType);
        update();
        if(game->IsWin(clickPosRow, clickPosCol) == true){
            if(curManType == -1){
                game->gameStatus = BLACKWIN;
            }else{
                game->gameStatus = WHITEWIN;
            }
            winC = clickPosCol;
            winR = clickPosRow;
        }else if(game->IsDraw()){
            game->gameStatus = DRAW;
        }
    }
}

void PvPGamePage::Change()
{
    //棋子颜色互换
    curManType = -curManType;
    //双方状态互换
    QString tempText, tempStyle;
    tempText = ui->PlayerState_1->text();
    ui->PlayerState_1->setText(ui->PlayerState_2->text());
    ui->PlayerState_2->setText(tempText);

    tempStyle = ui->PlayerState_1->styleSheet();
    ui->PlayerState_1->setStyleSheet(ui->PlayerState_2->styleSheet());
    ui->PlayerState_2->setStyleSheet(tempStyle);
}

void PvPGamePage::Conclude()
{
    if(game->gameStatus == DRAW){
        QMessageBox::warning(this,tr("平局"),tr("平分秋色"),QMessageBox::Ok);
    }else if((PlayerManType_1 == -1 && game->gameStatus == BLACKWIN)||(PlayerManType_1 == 1 && game->gameStatus == WHITEWIN)){
        PlayWinNum_1++;
        ui->PlayerScore_1->display(PlayWinNum_1);
        QMessageBox::warning(this,tr("玩家1获胜"),tr("你服不服气？"),QMessageBox::Ok);
        QSoundEffect * startSound2 = new QSoundEffect(this);//创建对象
        startSound2->setSource(QUrl::fromLocalFile(":/bgm/over.wav"));;//添加资源
        startSound2->setLoopCount(1);//设置循环次数int；
        startSound2->play();//软件启动自动播放
    }else{
        PlayWinNum_2++;
        ui->PlayerScore_2->display(PlayWinNum_2);
        QMessageBox::warning(this,tr("玩家2获胜"),tr("你服不服气？"),QMessageBox::Ok);
    }

    NewGame();
}

void PvPGamePage::on_PlayerChoose_1_currentIndexChanged(int index)
{
    if(index == 0){
        ui->PlayerChoose_2->setCurrentIndex(1);
    }else{
        ui->PlayerChoose_2->setCurrentIndex(0);
    }
}


void PvPGamePage::on_PlayerChoose_2_currentIndexChanged(int index)
{
    if(index == 0){
        ui->PlayerChoose_1->setCurrentIndex(1);
    }else{
        ui->PlayerChoose_1->setCurrentIndex(0);
    }
}


void PvPGamePage::on_Fight_clicked()
{
    if(ui->PlayerChoose_1->currentIndex() == 0){
        PlayerManType_1 = -1;
        PlayerManType_2 = 1;
        ui->PlayerState_1->setStyleSheet(action);
        ui->PlayerState_1->setText("行动");
        ui->ManImage_1->setStyleSheet("border-image:url(:/pic/black.png)");
        ui->ManImage_2->setStyleSheet("border-image:url(:/pic/white.png)");
    }else{
        PlayerManType_1 = 1;
        PlayerManType_2 = -1;
        ui->PlayerState_2->setStyleSheet(action);
        ui->PlayerState_2->setText("行动");
        ui->ManImage_1->setStyleSheet("border-image:url(:/pic/white.png)");
        ui->ManImage_2->setStyleSheet("border-image:url(:/pic/black.png)");
    }
    //控件enable
    setMouseTracking(true);
    ui->SelectMan->setEnabled(false);
    ui->Table->setEnabled(true);
}


void PvPGamePage::on_Surrender_1_clicked()
{
    if(PlayerManType_2 == 1){
        game->gameStatus = WHITEWIN;
    }else{
        game->gameStatus = BLACKWIN;
    }
    Conclude();
}


void PvPGamePage::on_Surrender_2_clicked()
{
    if(PlayerManType_1 == 1){
        game->gameStatus = WHITEWIN;
    }else{
        game->gameStatus = BLACKWIN;
    }
    Conclude();
}

void PvPGamePage::on_Quit_clicked()
{
    PlayWinNum_1 = 0;
    PlayWinNum_2 = 0;
    NewGame();
    emit(PVPReturen());
}
