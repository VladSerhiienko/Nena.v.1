#include "app.precompiled.h"
#include "qt.dialog_busy.ui.mo.h"

BusyDialog::BusyDialog(QDialog *parent) : QDialog(parent)
{
}

void BusyDialog::setupUi()
{
	busyBox = new QGroupBox;
	busyBox->setTitle("Busy");
	busyBox->setObjectName("busyBox");

	waitLabel = new QLabel;
	statusLabel = new QLabel;
	waitLabel->setText("Revwalking, please wait ...");
	statusLabel->setText("Initializing ...");

	progressBar = new QProgressBar;
	progressBar->setTextVisible(false);
	progressBar->setMaximum(0);
	progressBar->setMinimum(0);

	QGridLayout *layout;
	layout = new QGridLayout;
	layout->addWidget(waitLabel, 0, 0);
	layout->addWidget(progressBar, 1, 0);
	layout->addWidget(statusLabel, 2, 0);
	busyBox->setLayout(layout);

	QVBoxLayout *mainLayout = new QVBoxLayout;
	mainLayout->addWidget(busyBox);
	setLayout(mainLayout);

	// ...
	QMetaObject::connectSlotsByName(this);
}
