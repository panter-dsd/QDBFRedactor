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
	connect(tabBar, SIGNAL(currentChanged(int)), this, SLOT(tabChanged(int)));

	view = new QTableView(this);

	QWidget *centralWidget = new QWidget(this);

	QVBoxLayout *mainLayout = new QVBoxLayout();
	mainLayout->addWidget(tabBar);
	mainLayout->addWidget(view);
	centralWidget->setLayout(mainLayout);
	setCentralWidget(centralWidget);

	actionOpen = new QAction(tr("&Open"), this);
	actionOpen->setShortcut(QKeySequence::Open);
	connect(actionOpen, SIGNAL(triggered()), this, SLOT(open()));
	addAction(actionOpen);

//Menus
	QMenuBar *menuBar = new QMenuBar(this);
	setMenuBar(menuBar);

	QMenu *fileMenu = new QMenu(tr("File"), menuBar);
	fileMenu->addAction(actionOpen);

	menuBar->addMenu(fileMenu);
//ToolBars
	QToolBar *fileToolBar = new QToolBar(this);
	fileToolBar->addAction(actionOpen);
	addToolBar(fileToolBar);
}

DBFRedactorMainWindow::~DBFRedactorMainWindow()
{

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
}

void DBFRedactorMainWindow::tabChanged(int index)
{
	view->setModel(models.value(tabBar->tabData(index).toString()));
}
