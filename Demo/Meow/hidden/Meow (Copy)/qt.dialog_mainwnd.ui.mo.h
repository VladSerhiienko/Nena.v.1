#pragma once

#ifndef DIALOG_H
#define DIALOG_H

#include "qt_git.revwalk_modelview.mo.h"
#include "qt.dialog_chooserepo.ui.mo.h"
#include "qt.concurrent_ui.h"
#include "git.revwalk.h"

class MainWindowDialog : public QDialog
{
	Q_OBJECT

public:

	MainWindowDialog();

public:

	void SetupUi();
	void DisableUi();
	void EnableUi();
	bool SetBusy(bool);

public slots:

	void OnDiscoverOpened();
	void OnDiscoverClosed();

	void OnCellClicked(QModelIndex const &);
	void OnCellDoubleClicked(QModelIndex const &);
	void OnSelectionChanged(QItemSelection const &, QItemSelection const &);

private:

	QMenu *fileMenu;
	QMenuBar *menuBar;
	QGroupBox *revwalkGroupBox;
	QGroupBox *diffGroupBox;
	QGroupBox *descGroupBox;
	QTableView *revwalkView;
	QTextBrowser *diffView;
	QTextBrowser *descView;
	QDialogButtonBox *buttonBox;
	QProgressBar *diffProgressBar;

	QAction *exitAction;
	QAction *discoverAction;

	ChooseRepositoryDialog gitPicker;

private:

	typedef std::string String;

	bool TryLoadRepository(String git);

	::Nena::Qt::Revwalk revwalkData;
	::Nena::Git::Revwalk revwalk;
	::SRWLOCK lock;
};

#endif