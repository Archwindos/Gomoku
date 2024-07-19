#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDialog>
#include <QMessageBox>
#include "gameplay.h"
#include <QMediaPlayer>
#include <QSoundEffect>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void PvPOffline();
    void PvE();
    void PvPOnline();

private slots:
    void on_PvPButton_clicked();

    void on_PvEButton_clicked();

    void on_OnlineButton_clicked();

    void PVEtomainSlot();

    void PVPtomainSlot();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
