#include <QtCore/QSettings>

#include <QtGui/QTabBar>
#include <QtGui/QTableView>
#include <QtGui/QVBoxLayout>
#include <QtGui/QAction>
#include <QtGui/QMenuBar>
#include <QtGui/QToolBar>
#include <QtGui/QFileDialog>
#include <QtGui/QStatusBar>
#include <QtGui/QLabel>

#include "dbfredactormainwindow.h"

DBFRedactorMainWindow::DBFRedactorMainWindow(QWidget* parent, Qt::WFlags f)
		: QMainWindow(parent, f)
{
	tabBar = new QTabBar(this);
	tabBar->setTabsClosable(true);
	tabBar->setContextMenuPolicy(Qt::ActionsContextMenu);
	connect(tabBar, SIGNAL(currentChanged(int)), this, SLOT(tabChanged(int)));
	connect(tabBar, SIGNAL(tabCloseRequested(int)), this, SLOT(closeTab(int)));

	view = new QTableView(this);
	view->setVisible(false);

	setStatusBar(new QStatusBar(this));

	currentFile = new QLabel(this);
	statusBar()->insertWidget(0, currentFile);

	sumLabel = new QLabel(this);
	statusBar()->insertWidget(1, sumLabel);

	QWidget *centralWidget = new QWidget(this);

	QVBoxLayout *mainLayout = new QVBoxLayout();
	mainLayout->addWidget(tabBar);
	mainLayout->addWidget(view);
	centralWidget->setLayout(mainLayout);
	setCentralWidget(centralWidget);
//Actions
	actionOpen = new QAction(QIcon(":/share/images/open.png"), tr("&Open"), this);
	actionOpen->setToolTip(tr("Open file"));
	actionOpen->setShortcut(QKeySequence::Open);
	connect(actionOpen, SIGNAL(triggered()), this, SLOT(open()));
	addAction(actionOpen);

	actionExit = new QAction(QIcon(":/share/images/exit.png"), tr("E&xit"), this);
	actionExit->setToolTip(tr("Exit from program"));
	actionExit->setShortcut(Qt::ALT + Qt::Key_X);
	connect(actionExit, SIGNAL(triggered()), this, SLOT(close()));
	addAction(actionExit);

	actionClose = new QAction(QIcon(":/share/images/close.png"), tr("&Close"), this);
	actionClose->setToolTip(tr("Close current file"));
	actionClose->setShortcut(Qt::CTRL + Qt::Key_W);
	connect(actionClose, SIGNAL(triggered()), this, SLOT(closeCurrentTab()));
	addAction(actionClose);
	tabBar->addAction(actionClose);

	actionRefresh = new QAction(QIcon(":/share/images/refresh.png"), tr("&Refresh"), this);
	actionRefresh->setToolTip(tr("Refresh current file"));
	actionRefresh->setShortcut(Qt::CTRL + Qt::Key_R);
	connect(actionRefresh, SIGNAL(triggered()), this, SLOT(refreshModel()));
	addAction(actionRefresh);
	tabBar->addAction(actionRefresh);
//Menus
	QMenuBar *menuBar = new QMenuBar(this);
	setMenuBar(menuBar);

	QMenu *fileMenu = new QMenu(tr("File"), menuBar);
	fileMenu->addAction(actionOpen);
	fileMenu->addAction(actionClose);
	fileMenu->addSeparator();
	fileMenu->addAction(actionExit);

	menuBar->addMenu(fileMenu);
//ToolBars
	QToolBar *fileToolBar = new QToolBar(this);
	fileToolBar->addAction(actionOpen);
	fileToolBar->addAction(actionClose);
	fileToolBar->addSeparator();
	fileToolBar->addAction(actionExit);
	addToolBar(fileToolBar);

	loadSettings();
	updateActions();
}

DBFRedactorMainWindow::~DBFRedactorMainWindow()
{
	saveSettings();
	while(tabBar->count() > 0)
		closeTab(0);
}

void DBFRedactorMainWindow::loadSettings()
{
	QSettings settings;

	settings.beginGroup("MainWindow");
	move(settings.value("pos", QPoint(0, 0)).toPoint());
	resize(settings.value("size", QSize(640, 480)).toSize());
	bool isMaximized = settings.value("IsMaximized", false).toBool();
	if (isMaximized)
		setWindowState(Qt::WindowMaximized);
	restoreState(settings.value("State", "").toByteArray());
	settings.endGroup();
}

void DBFRedactorMainWindow::saveSettings()
{
	QSettings settings;

	settings.beginGroup("MainWindow");
	if (windowState() != Qt::WindowMaximized) {
		settings.setValue("pos", pos());
		settings.setValue("size", size());
		settings.setValue("IsMaximized", false);
	} else
		settings.setValue("IsMaximized", true);
	settings.setValue("State", saveState());

	settings.sync();
}

void DBFRedactorMainWindow::open()
{
	QSettings settings;
	const QStringList& fileNames = QFileDialog::getOpenFileNames(this,
															tr("Open files"),
															settings.value("Global/OpenPath", "'").toString(),
															tr("DBase files (*.dbf)"));
	if (fileNames.isEmpty())
		return;

	settings.setValue("Global/OpenPath", QFileInfo(fileNames.first()).absolutePath());

	openFiles(fileNames);
}

void DBFRedactorMainWindow::openFiles(const QStringList& fileList)
{
	int index = 0;
	foreach(const QString& fileName, fileList) {
		if (models.contains(fileName)) {
			for (int i = 0; i < tabBar->count(); i++) {
				if (tabBar->tabData(i).toString() == fileName) {
					index = i;
					break;
				}
			}
			continue;
		}
		DBFRedactorModel *model = new DBFRedactorModel(fileName, this);

		models.insert(fileName, model);
		index = tabBar->addTab(QFileInfo(fileName).fileName());
		tabBar->setTabData(index, fileName);
		tabBar->setTabToolTip(index, QDir::toNativeSeparators(fileName));
	}
	tabBar->setCurrentIndex(index);
	tabChanged(index);
	view->setVisible(true);
	updateActions();
}

void DBFRedactorMainWindow::tabChanged(int index)
{
	view->setModel(models.value(tabBar->tabData(index).toString()));
	connect(view->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
			this, SLOT(selectionChanged()));

	currentFile->setText(tabBar->tabToolTip(index));
	selectionChanged();
}

void DBFRedactorMainWindow::closeTab(int index)
{
	DBFRedactorModel *model = models.value(tabBar->tabData(index).toString());
	if (!model)
		return;
	delete model;
	models.remove(tabBar->tabData(index).toString());
	tabBar->removeTab(index);
	if (tabBar->count() <= 0) {
		view->setVisible(false);
		currentFile->setText("");
	}
	updateActions();
}

void DBFRedactorMainWindow::closeCurrentTab()
{
	closeTab(tabBar->currentIndex());
}

void DBFRedactorMainWindow::updateActions()
{
	actionClose->setEnabled(tabBar->count() > 0);
	currentFile->setVisible(!currentFile->text().isEmpty());
	sumLabel->setVisible(!currentFile->text().isEmpty());
}

void DBFRedactorMainWindow::refreshModel()
{
	DBFRedactorModel *model = models.value(tabBar->tabData(tabBar->currentIndex()).toString());
	if (model)
		model->refresh();
}

void DBFRedactorMainWindow::selectionChanged()
{
	const QModelIndexList& l = view->selectionModel()->selection().indexes();
	double sum = 0.0;
	foreach(const QModelIndex& index, l) {
		bool ok = false;
		double value = index.data(Qt::DisplayRole).toDouble(&ok);
		if (ok)
			sum += value;
	}
	sumLabel->setText(tr("Sum") + " = " + QString::number(sum, 'f', 2));
}
