
#include "app.precompiled.h"
#include "qt.dialog_mainwnd.ui.mo.h"
#include "qsplashscreen.h"


MainWindowDialog::MainWindowDialog() 
	: gitPicker(this)
	, revwalkData(&revwalk)
	, lock()
{
	InitializeSRWLock(&lock);

	SetupUi();
}

void MainWindowDialog::SetupUi()
{
	QPixmap pixmap("splash3.jpg");
	//QPixmap pixmap("splash2.png");
	//QPixmap pixmap("splash1.jpg");
	QSplashScreen splash(pixmap);

	splash.setForegroundRole(QPalette::ColorRole::Light);
	splash.showMessage("Loading...");
	splash.show();

	resize(600, 800);
	setMinimumSize(QSize(500, 500));

	fileMenu = new QMenu(tr("&File"), this);
	discoverAction = fileMenu->addAction(tr("&Discover..."));
	exitAction = fileMenu->addAction(tr("E&xit"));

	menuBar = new QMenuBar;
	menuBar->addMenu(fileMenu);

	revwalkGroupBox = new QGroupBox(tr("Revwalk"));
	descGroupBox = new QGroupBox(tr("Desc"));
	diffGroupBox = new QGroupBox(tr("Diff"));

	revwalkGroupBox->setMinimumWidth(200);
	revwalkGroupBox->setMaximumWidth(500);
	descGroupBox->setMinimumWidth(200);
	descGroupBox->setMaximumWidth(500);

	QGridLayout *revwalkLayout = new QGridLayout;

	revwalkView = new QTableView;
	revwalkView->setSelectionMode(QAbstractItemView::SingleSelection);
	revwalkView->setSelectionBehavior(QAbstractItemView::SelectRows);
	revwalkView->setEditTriggers(QAbstractItemView::NoEditTriggers);

	revwalkLayout->addWidget(revwalkView, 0, 0);
	revwalkLayout->setColumnStretch(0, 10);

	QGridLayout *descLayout = new QGridLayout;
	QGridLayout *diffLayout = new QGridLayout;
	diffView = new QTextBrowser;
	descView = new QTextBrowser;

	diffView->setWordWrapMode(QTextOption::WrapMode::NoWrap);
	diffLayout->addWidget(diffView, 0, 0);
	diffLayout->setColumnStretch(0, 10);

	revwalkGroupBox->setLayout(revwalkLayout);
	descGroupBox->setLayout(descLayout);
	diffGroupBox->setLayout(diffLayout);

	QFont monofont;
	monofont.setStyleHint(QFont::StyleHint::Monospace);
	monofont.setFamily("Console");

	QSizePolicy policy;
	policy.setVerticalPolicy(QSizePolicy::Preferred);
	policy.setHorizontalPolicy(QSizePolicy::Preferred);

	diffView->setFont(monofont);
	descView->setSizePolicy(policy);

	buttonBox = new QDialogButtonBox(
		QDialogButtonBox::Close //QDialogButtonBox::Ok | QDialogButtonBox::Cancel
		);

	//connect(buttonBox, SIGNAL(rejected()), this, SLOT(close()));
	//connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
	//connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

	descLayout->addWidget(descView);

	QGridLayout *mainLayout = new QGridLayout;
	QVBoxLayout *diffPanel = new QVBoxLayout;
	diffProgressBar = new QProgressBar;
	diffProgressBar->setTextVisible(false);
	diffProgressBar->setMaximum(0);
	diffProgressBar->setMinimum(0);
	SetBusy(false);

	diffProgressBar->setStyleSheet(
		"QProgressBar { border-radius: 0px; text-align: center; } "
		"QProgressBar::chunk { background-color: #cccccc; width: 10px; } "
		);

	diffPanel->addWidget(diffGroupBox);
	diffLayout->addWidget(diffProgressBar, 1, 0);

	mainLayout->setMenuBar(menuBar);
	mainLayout->addWidget(revwalkGroupBox, 0, 0);
	mainLayout->addWidget(descGroupBox, 1, 0);
	mainLayout->addLayout(diffPanel, 0, 1, 2, 1);
	mainLayout->addWidget(buttonBox, 2, 0, 1, 2);

	setLayout(mainLayout);

	setWindowTitle(tr("Vakoms Test"));

	QObject::connect(buttonBox, SIGNAL(rejected()), this, SLOT(accept()));
	QObject::connect(exitAction, SIGNAL(triggered()), this, SLOT(accept()));
	QObject::connect(discoverAction, SIGNAL(triggered()), this, SLOT(OnDiscoverOpened()));
	QObject::connect(&gitPicker, SIGNAL(accepted()), this, SLOT(OnDiscoverClosed()));
	QObject::connect(&gitPicker, SIGNAL(rejected()), this, SLOT(OnDiscoverClosed()));

	QObject::connect(
		revwalkView, SIGNAL(doubleClicked(QModelIndex const&)), 
		this, SLOT(OnCellDoubleClicked(QModelIndex const&))
		);

	QMetaObject::connectSlotsByName(this);

	if (gitPicker.repository.empty())
	{
		splash.showMessage("First launch...", 1, Qt::white);
		OnDiscoverOpened();

		if (gitPicker.repository.empty())
		{
			splash.showMessage("Closing...", 1, Qt::white);
			Sleep(1000);
			splash.close();
			exit(1);
		}
	}
	else
	{
		TryLoadRepository(gitPicker.repository);
		splash.showMessage("Opening...", 1, Qt::white);
		Sleep(1500); // imitate hard work...
		splash.close();
	}
}

void MainWindowDialog::DisableUi()
{
	revwalkGroupBox->setEnabled(false);
	descGroupBox->setEnabled(false);
	diffGroupBox->setEnabled(false);
	buttonBox->setEnabled(false);
}

void MainWindowDialog::EnableUi()
{
	revwalkGroupBox->setEnabled(true);
	descGroupBox->setEnabled(true);
	diffGroupBox->setEnabled(true);
	buttonBox->setEnabled(true);
}

bool MainWindowDialog::SetBusy(bool busy)
{
	auto acquired = false;
	if (lock.Ptr && busy) printf_s("qt-main> panic\n");

	if (busy && (acquired = TryAcquireSRWLockExclusive(&lock)))
	{
		diffProgressBar->show();
		diffGroupBox->update();
		return true;
	}
	else if (!busy)
	{
		if (lock.Ptr) ReleaseSRWLockExclusive(&lock);
		diffProgressBar->hide();
		diffGroupBox->update();
		return true;
	}

	return false;
}

bool MainWindowDialog::TryLoadRepository(String start)
{
	if (SetBusy(true))
		Nena::QtCoWait2(concurrency::create_task([this, start]()
	{
		std::string gitLocation;
		if (!Nena::Git::Revwalk::DiscoverRepository(start, gitLocation))
			return FALSE;
		revwalk.Run(gitLocation);
		return TRUE;
	}), [this](BOOL result)
	{
		revwalkView->setModel(revwalk.Size() ? &revwalkData : nullptr);
		revwalkView->resizeColumnsToContents();
		revwalkView->horizontalHeader()->setStretchLastSection(true);

		QObject::connect(
			revwalkView->selectionModel(), SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
			this, SLOT(OnSelectionChanged(const QItemSelection &, const QItemSelection &))
			);

		OnDiscoverClosed();
		SetBusy(false);
	});
	else return false;
	return true;
}

void MainWindowDialog::OnDiscoverOpened()
{
	QGraphicsBlurEffect *blurEffect = new QGraphicsBlurEffect;
	blurEffect->setBlurHints(
		QGraphicsBlurEffect::BlurHint::QualityHint |
		QGraphicsBlurEffect::BlurHint::AnimationHint
		);
	blurEffect->setBlurRadius(25);
	setGraphicsEffect(blurEffect);
	update();

	gitPicker.exec();
	if (gitPicker.IsValid())
		TryLoadRepository(gitPicker.repository);
}

void MainWindowDialog::OnDiscoverClosed()
{
	setGraphicsEffect(nullptr);
	update();
}

void MainWindowDialog::OnCellClicked(QModelIndex const &)
{
	OutputDebugStringA("OnCellClicked\n");
}

void MainWindowDialog::OnCellDoubleClicked(QModelIndex const &i)
{
	auto index = i.row();

	//printf_s("qt-main> diff: %d\n", index);

	if (SetBusy(true))
	Nena::QtCoWait2(concurrency::create_task([this, index]()
	{
		auto commit = (*std::next(revwalk.cbegin(), index));
		//Sleep(5000);
		return commit->GenerateDiffPatch();
	}), [this](std::string patch)
	{
		diffView->setHtml(patch.c_str());
		SetBusy(false);
	});
}

void MainWindowDialog::OnSelectionChanged(
	QItemSelection const &selection, 
	QItemSelection const &deselection
	)
{
	auto index = selection.indexes().first().row();

	//printf_s("qt-main> desc: %d\n", index);

	auto commit = (*std::next(revwalk.cbegin(), index));
	descView->setHtml(commit->GenerateDesc().c_str());
}

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	MainWindowDialog dialog;
	dialog.show();

	return app.exec();
}