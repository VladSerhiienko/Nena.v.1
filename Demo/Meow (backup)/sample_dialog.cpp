#include <QtGui>
#include <qtextedit.h>
#include <qdialogbuttonbox.h>
#include <qboxlayout.h>
#include <qmenubar.h>
#include <qgroupbox.h>
#include <qpushbutton.h>
#include <qlineedit.h>
#include <qlabel.h>
#include <qformlayout.h>
#include <qspinbox.h>
#include <qcombobox.h>
#include <qtableview.h>
#include <qaction.h>

#include "sample_dialog.ui.mo.h"

Dialog::Dialog()
{
	createMenu();
	createHorizontalGroupBox();
	createGridGroupBox();
	createFormGroupBox();

	bigEditor = new QTextEdit;
	bigEditor->setPlainText(tr("This widget takes up all the remaining space "
		"in the top-level layout."));

	buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
		| QDialogButtonBox::Cancel);

	connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
	connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

	QVBoxLayout *mainLayout = new QVBoxLayout;
	mainLayout->setMenuBar((QWidget*) menuBar);
	mainLayout->addWidget((QWidget*) horizontalGroupBox);
	mainLayout->addWidget((QWidget*) gridGroupBox);
	mainLayout->addWidget((QWidget*) formGroupBox);
	mainLayout->addWidget((QWidget*) bigEditor);
	mainLayout->addWidget((QWidget*) buttonBox);
	setLayout(mainLayout);

	setWindowTitle(tr("Basic Layouts"));
}

void Dialog::createMenu()
{
	menuBar = new QMenuBar;

	fileMenu = new QMenu(tr("&File"), this);
	exitAction = fileMenu->addAction(tr("E&xit"));
	menuBar->addMenu(fileMenu);

	connect(exitAction, SIGNAL(triggered()), this, SLOT(accept()));
}

void Dialog::createHorizontalGroupBox()
{
	horizontalGroupBox = new QGroupBox(tr("Horizontal layout"));
	QHBoxLayout *layout = new QHBoxLayout;

	for (int i = 0; i < NumButtons; ++i)
	{
		buttons[i] = new QPushButton(tr("Button %1").arg(i + 1));
		connect(buttons[i], SIGNAL(released()), this, SLOT(handleButton()));
		layout->addWidget(buttons[i]);
	}
	horizontalGroupBox->setLayout(layout);
}

void Dialog::createGridGroupBox()
{
	gridGroupBox = new QGroupBox(tr("Commits"));
	QGridLayout *layout = new QGridLayout;

	/*for (int i = 0; i < NumGridRows; ++i)
	{
		labels[i] = new QLabel(tr("Line %1:").arg(i + 1));
		lineEdits[i] = new QLineEdit;
		layout->addWidget(labels[i], i + 1, 0);
		layout->addWidget(lineEdits[i], i + 1, 1);
	}*/

	tableView = new QTableView;
	//smallEditor = new QTextEdit;
	//smallEditor->setPlainText(tr("This widget takes up about two thirds of the "
	//	"grid layout."));

	QStandardItemModel *model = new QStandardItemModel(2, 3, this); //2 Rows and 3 Columns
	model->setHorizontalHeaderItem(0, new QStandardItem(QString("Column1 Header")));
	model->setHorizontalHeaderItem(1, new QStandardItem(QString("Column2 Header")));
	model->setHorizontalHeaderItem(2, new QStandardItem(QString("Column3 Header")));

	//auto i = tableView->model()->index(0, 0);
	//tableView->setIndexWidget(
	//	tableView->model()->index(0, 0),
	//	new QPushButton()
	//	);

	tableView->setModel(model);
	tableView->setVisible(false);
	tableView->resizeColumnToContents(0);
	//tableView->resizeColumnsToContents();
	tableView->setVisible(true);

	layout->addWidget(tableView, 0, 0);
	//layout->addWidget(tableView, 0, 2, 4, 1);
	//layout->addWidget(smallEditor, 0, 2, 4, 1);

	layout->setColumnStretch(0, 10);
	//layout->setColumnStretch(1, 10);
	//layout->setColumnStretch(2, 20);
	gridGroupBox->setLayout(layout);
}

void Dialog::createFormGroupBox()
{
	formGroupBox = new QGroupBox(tr("Form layout"));
	QFormLayout *layout = new QFormLayout;
	layout->addRow(new QLabel(tr("Line 1:")), new QLineEdit);
	layout->addRow(new QLabel(tr("Line 2, long text:")), new QComboBox);
	layout->addRow(new QLabel(tr("Line 3:")), new QSpinBox);
	formGroupBox->setLayout(layout);
}

void Dialog::handleButton()
{
	OutputDebugStringA("void Dialog::handleButton()\n");
}