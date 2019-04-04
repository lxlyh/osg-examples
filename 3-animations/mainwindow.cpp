#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_paly_clicked()
{
    //从动画列表中选择一个动画，播放
    ui->openGLWidget->play1();
}

void MainWindow::on_pushButton_paly2_clicked()
{
    ui->openGLWidget->play2();
}
