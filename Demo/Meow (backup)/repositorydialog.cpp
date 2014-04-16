
#include "build\debug\ui\ui_repositorydialog.h"
#include "repositorydialog.h"

RepositoryDialog::RepositoryDialog(QWidget *parent) :
    QDialog(parent), ui(new Ui::RepositoryDialog())
{
    ui->setupUi(this);
}

RepositoryDialog::~RepositoryDialog()
{
    delete ui;
}
