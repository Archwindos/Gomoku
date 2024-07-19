#include "mainwindow.h"
#include "pvegamepage.h"
#include "pvpgamepage.h"
#include "ui_mainwindow.h"

PvEGamePage* PvEPage;
PvPGamePage* PvPPage;


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->centralwidget->setFixedSize(1400, 800);
    QPalette PAllbackground = this->palette();
    QImage ImgAllbackground(":/pic/firstBackground.png");
    QImage fitimgpic=ImgAllbackground.scaled(this->width(),this->height(), Qt::IgnoreAspectRatio);
    PAllbackground.setBrush(QPalette::Window, QBrush(fitimgpic));
    this->setPalette(PAllbackground);

    PvPPage = new PvPGamePage();
    PvEPage = new PvEGamePage();

    connect(PvEPage,SIGNAL(PVEReturen()),this,SLOT(PVEtomainSlot()));
    connect(PvPPage,SIGNAL(PVPReturen()),this,SLOT(PVPtomainSlot()));
    QSoundEffect * startSound = new QSoundEffect(this);//创建对象
    startSound->setSource(QUrl::fromLocalFile(":/bgm/gaming.wav"));//添加资源
    startSound->setLoopCount(QSoundEffect::Infinite);//设置循环次数int；  QSoundEffect::Infinite 枚举值 无限循环
    startSound->setVolume(0.05f);
    startSound->play();//软件启动自动播放
}

MainWindow::~MainWindow()
{
    delete ui;
    delete PvPPage;
    PvPPage = nullptr;
    delete PvEPage;
    PvEPage = nullptr;
}

void MainWindow::on_PvPButton_clicked()
{
    this->close();
    PvPPage->show();
}

void MainWindow::on_PvEButton_clicked()
{
    this->close();
    PvEPage->show();
}

void MainWindow::on_OnlineButton_clicked()
{
    QMessageBox::warning(this,tr("温馨提示"),tr("本功能正在调试中"),QMessageBox::Ok);
}

void MainWindow::PVEtomainSlot()
{
    PvEPage->close();
    this->show();
}

void MainWindow::PVPtomainSlot()
{
    PvPPage->close();
    this->show();
}

