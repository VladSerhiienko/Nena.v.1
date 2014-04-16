#include "app.precompiled.h"
#include "qt.dialog_chooserepo.ui.mo.h"

#include "git.revwalk.h"

ChooseRepositoryDialog::ChooseRepositoryDialog(QDialog *parent)
	: QDialog(parent), isValid(false)
{
	SetupUi();
	ReadHistory();

	//setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
	setWindowFlags(Qt::FramelessWindowHint | windowFlags());
}

void ChooseRepositoryDialog::SetupUi()
{
	if (this->objectName().isEmpty())
		this->setObjectName(QStringLiteral("dialog_choose_repo"));

	this->resize(600, 127);
	gridLayout_2 = new QGridLayout(this);
	gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
	groupBox = new QGroupBox(this);
	groupBox->setObjectName(QStringLiteral("groupBox"));
	gridLayout_3 = new QGridLayout(groupBox);
	gridLayout_3->setObjectName(QStringLiteral("gridLayout_3"));
	gridLayout = new QGridLayout();
	gridLayout->setObjectName(QStringLiteral("gridLayout"));
	pushButton = new QPushButton(groupBox);
	pushButton->setObjectName(QStringLiteral("pushButton"));

	gridLayout->addWidget(pushButton, 0, 1, 1, 1);

	lineEdit = new QLineEdit(groupBox);
	lineEdit->setObjectName(QStringLiteral("lineEdit"));

	gridLayout->addWidget(lineEdit, 0, 0, 1, 1);

	buttonBox = new QDialogButtonBox(groupBox);
	buttonBox->setObjectName(QStringLiteral("buttonBox"));
	buttonBox->setOrientation(Qt::Horizontal);
	buttonBox->setStandardButtons(QDialogButtonBox::Cancel | QDialogButtonBox::Ok);

	gridLayout->addWidget(buttonBox, 1, 0, 1, 2);
	gridLayout_3->addLayout(gridLayout, 0, 0, 1, 1);
	gridLayout_2->addWidget(groupBox, 0, 0, 1, 1);

	setWindowTitle(QApplication::translate("Pick", "Dialog", 0));
	groupBox->setTitle(QApplication::translate("dialog_choose_repo", "Repository", 0));
	pushButton->setText(QApplication::translate("dialog_choose_repo", "Browse...", 0));

	QObject::connect(buttonBox, SIGNAL(accepted()), this, SLOT(OnOkClicked()));
	QObject::connect(buttonBox, SIGNAL(rejected()), this, SLOT(OnCancelClicked()));
	QObject::connect(pushButton, SIGNAL(clicked()), this, SLOT(OnBrowseClicked()));

	QMetaObject::connectSlotsByName(this);
}

void ChooseRepositoryDialog::ReadHistory()
{
	std::string line;
	std::ifstream input("Meow.History");

	repositoryHistory.clear();
	if (input.is_open())
	while (std::getline(input, line))
		repositoryHistory.insert(line);
	input.close();

	if (repositoryHistory.size())
		lineEdit->setText((repository = *repositoryHistory.begin()).c_str());
	else std::cout << "qt-pick> first launch\n";
}

void ChooseRepositoryDialog::SaveHistory()
{
	std::ofstream output("Meow.History");
	if (output.is_open())
	for (auto &line : repositoryHistory)
		output << line << "\n";
	output.flush();
	output.close();
}

BOOL ChooseRepositoryDialog::AddToHistory(String entry)
{
	String git;
	if (Nena::Git::Revwalk::DiscoverRepository(entry, git))
	{
		/*auto 
			begin = repositoryHistory.begin(), 
			end = repositoryHistory.begin();

		if (std::find(begin, end, git) == end)
		{
			repositoryHistory.push_back(git);
			repository = git;
			return TRUE;
		}
		else
		{
			repositoryHistory.push_back(git);
			std::unique(repositoryHistory.begin(), repositoryHistory.end());
		}*/

		lineEdit->setText(git.c_str());
		repositoryHistory.insert(git);
		repository = git;
		return TRUE;
	}

	return FALSE;
}

//! @see http://stackoverflow.com/a/10616741
void ChooseRepositoryDialog::OnBrowseClicked()
{
	QString dir = QFileDialog::getExistingDirectory(
		this, tr("Pick git repository"), "", // current directory 
		QFileDialog::DontResolveSymlinks |
		QFileDialog::ShowDirsOnly
		);
	lineEdit->setText(dir);
}

bool ChooseRepositoryDialog::IsValid()
{
	return isValid;
}

void ChooseRepositoryDialog::OnOkClicked()
{
	auto dir = lineEdit->text();
	if (dir.size() && !AddToHistory(dir.toStdString()))
	{
		QMessageBox::information(
			this, "Oops...",
			"Cannot find any git repository at this location:\n" + dir,
			QMessageBox::Ok
			);
	}
	else
	{
		SaveHistory();
		isValid = !!repository.size();
		QDialog::accept();
	}
}

void ChooseRepositoryDialog::OnCancelClicked()
{
	isValid = false;
	QDialog::reject();
}
