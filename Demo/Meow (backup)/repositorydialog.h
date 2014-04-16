#ifndef REPOSITORYDIALOG_H
#define REPOSITORYDIALOG_H

#include <QDialog>

namespace Ui {
class RepositoryDialog;
}


class RepositoryDialog : public QDialog
{

    Q_OBJECT

public:
    explicit RepositoryDialog(QWidget *parent = 0);
    ~RepositoryDialog();

private:

    Ui::RepositoryDialog *ui;
};


#endif // REPOSITORYDIALOG_H
