#pragma once

#include "app.precompiled.h"

class Splash : public QDialog
{
	Q_OBJECT

public:

	Splash(QWidget *parent);
	~Splash();

	void SetupUi();

protected:
	QLabel *imageLabel;

};