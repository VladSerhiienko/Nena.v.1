#include "app.precompiled.h"
#include "qt.splash.ui.mo.h"

Splash::Splash(QWidget *widget) : QDialog(widget)
{
	setWindowFlags(Qt::FramelessWindowHint | windowFlags());
}

void Splash::SetupUi()
{

	imageLabel = new QLabel;
	imageLabel->setBackgroundRole(QPalette::Base);
	imageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
	imageLabel->setScaledContents(true);

	QImage image("splash1.jpg");
	//QImage image("splash2.png");

	imageLabel->setPixmap(QPixmap::fromImage(image));
	imageLabel->resize(imageLabel->pixmap()->size());
	resize(imageLabel->size());

}
