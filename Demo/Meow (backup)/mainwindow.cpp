#include <iostream>

#include "build\debug\ui\ui_mainwindow.h"
//#include "repositorydialog.h"

#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    askRepo(new RepositoryDialog(this))
{
    ui->setupUi(this);
    askRepo->show();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
    //std::cout << "MainWindow::resizeEvent()" << std::endl;
}
