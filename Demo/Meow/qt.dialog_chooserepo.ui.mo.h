#pragma once

#include "app.precompiled.h"

class ChooseRepositoryDialog : public QDialog
{
	Q_OBJECT

public:

	typedef std::unordered_set<std::string> StringList;
	typedef StringList::value_type String;

	ChooseRepositoryDialog(QDialog *parent);
	void SetupUi();
	bool IsValid();

	String repository;

protected:

	QLineEdit *lineEdit;
	QGroupBox *groupBox;
	QPushButton *pushButton;
	QGridLayout *gridLayout;
	QGridLayout *gridLayout_2;
	QGridLayout *gridLayout_3;
	QDialogButtonBox *buttonBox;
	QListView *historyView; //! @todo 

protected slots:

	void OnOkClicked();
	void OnCancelClicked();
	void OnBrowseClicked();

protected:

	void ReadHistory();
	void SaveHistory();
	BOOL AddToHistory(String entry);

	StringList repositoryHistory;
	bool isValid;

};