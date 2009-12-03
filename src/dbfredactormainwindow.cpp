#include <QtCore/QSettings>

#include <QtGui/QTabBar>
#include <QtGui/QTableView>
#include <QtGui/QVBoxLayout>
#include <QtGui/QAction>
#include <QtGui/QMenuBar>
#include <QtGui/QToolBar>
#include <QtGui/QFileDialog>

#include "dbfredactormainwindow.h"

DBFRedactorMainWindow::DBFRedactorMainWindow(QWidget* parent, Qt::WFlags f)
		: QMainWindow(parent, f)
{
	tabBar = new QTabBar(this);
	tabBar->setTabsClosable(true);
	connect(tabBar, SIGNAL(currentChanged(int)), this, SLOT(tabChanged(int)));
	connect(tabBar, SIGNAL(tabCloseRequested(int)), this, SLOT(closeTab(int)));

	view = new QTableView(this);
	view->setVisible(false);

	QWidget *centralWidget = new QWidget(this);

	QVBoxLayout *mainLayout = new QVBoxLayout();
	mainLayout->addWidget(tabBar);
	mainLayout->addWidget(view);
	centralWidget->setLayout(mainLayout);
	setCentralWidget(centralWidget);
//Actions
	actionOpen = new QAction(QIcon(":/share/images/open.png"), tr("&Open"), this);
	actionOpen->setShortcut(QKeySequence::Open);
	connect(actionOpen, SIGNAL(triggered()), this, SLOT(open()));
	addAction(actionOpen);

	actionExit = new QAction(QIcon(":/share/images/exit.png"), tr("E&xit"), this);
	actionExit->setShortcut(Qt::ALT + Qt::Key_X);
	connect(actionExit, SIGNAL(triggered()), this, SLOT(close()));
	addAction(actionExit);

	actionClose = new QAction(QIcon(":/share/images/close.png"), tr("&Close"), this);
	actionClose->setShortcut(Qt::ALT + Qt::Key_X);
	connect(actionClose, SIGNAL(triggered()), this, SLOT(closeCurrentTab()));
	addAction(actionClose);

	actionRefresh = new QAction(QIcon(":/share/images/refresh.png"), tr("&Refresh"), this);
	actionRefresh->setShortcut(Qt::CTRL + Qt::Key_R);
	connect(actionRefresh, SIGNAL(triggered()), this, SLOT(refreshModel()));
	addAction(actionRefresh);
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
		DBFRedactorModel *model = new DBFRedactorModel(fileName, this);

		models.insert(fileName, model);
		index = tabBar->addTab(QFileInfo(fileName).fileName());
		tabBar->setTabData(index, fileName);
	}
	tabBar->setCurrentIndex(index);
	tabChanged(index);
	view->setVisible(true);
	updateActions();
}

void DBFRedactorMainWindow::tabChanged(int index)
{
	view->setModel(models.value(tabBar->tabData(index).toString()));
}

void DBFRedactorMainWindow::closeTab(int index)
{
	DBFRedactorModel *model = models.value(tabBar->tabData(index).toString());
	if (!model)
		return;
	delete model;
	models.remove(tabBar->tabData(index).toString());
	tabBar->removeTab(index);
	if (tabBar->count() <= 0)
		view->setVisible(false);
	updateActions();
}

void DBFRedactorMainWindow::closeCurrentTab()
{
	closeTab(tabBar->currentIndex());
}

void DBFRedactorMainWindow::updateActions()
{
	actionClose->setEnabled(tabBar->count() > 0);
}

void DBFRedactorMainWindow::refreshModel()
{
	DBFRedactorModel *model = models.value(tabBar->tabData(tabBar->currentIndex()).toString());
	if (model)
		model->refresh();
}
