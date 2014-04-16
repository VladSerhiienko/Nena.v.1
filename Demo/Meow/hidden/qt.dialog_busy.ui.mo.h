#pragma once

#include "app.precompiled.h"

class BusyDialog : public QDialog
{
	Q_OBJECT

public:

	BusyDialog(QDialog *parent);
	void setupUi();

	void updateStatus(QString const &status);

protected:

	QProgressBar *progressBar;
	QGroupBox *busyBox;
	QLabel *statusLabel;
	QLabel *waitLabel;

};