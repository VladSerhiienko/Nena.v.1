
#if QT_VISUALSTUDIOIDE
#include <git2.h>
#endif

#include <iostream>
#include <QApplication>
#include <qdialog.h>
#include "mainwindow.h"

int main(int argc, char *argv[])
{

	if (argc)
		std::cout << argv[0] << std::endl;

    QApplication a(argc, argv);
    MainWindow w;
	//QDialog d(&w);
    w.show();
	//d.show();

    return a.exec();
}
