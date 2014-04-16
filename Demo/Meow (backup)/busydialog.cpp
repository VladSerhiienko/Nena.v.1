#include "busydialog.h"
#include "ui_busydialog.qt.h"

BusyDialog::BusyDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BusyDialog)
{
    ui->setupUi(this);
}

BusyDialog::~BusyDialog()
{
    delete ui;
}
