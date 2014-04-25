#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "repositorydialog.h"

namespace Ui {
class MainWindow;
}

class RepositoryDialog;
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    virtual void resizeEvent(QResizeEvent *event);

private:
    Ui::MainWindow *ui;
    RepositoryDialog *askRepo;
};

#endif // MAINWINDOW_H
