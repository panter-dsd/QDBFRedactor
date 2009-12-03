#include <QtGui/QTabBar>
#include <QtGui/QTableView>
#include <QtGui/QVBoxLayout>
#include <QtGui/QAction>
#include <QtGui/QMenuBar>
#include <QtGui/QToolBar>

#include "dbfredactormainwindow.h"


DBFRedactorMainWindow::DBFRedactorMainWindow(QWidget* parent, Qt::WFlags f)
		: QMainWindow(parent, f)
{
	tabBar = new QTabBar(this);

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


	QMenuBar *menuBar = new QMenuBar(this);
	setMenuBar(menuBar);

	QMenu *fileMenu = new QMenu(tr("File"), menuBar);
	fileMenu->addAction(actionOpen);

	menuBar->addMenu(fileMenu);

	QToolBar *fileToolBar = new QToolBar(this);
	fileToolBar->addAction(actionOpen);
	addToolBar(fileToolBar);
}

DBFRedactorMainWindow::~DBFRedactorMainWindow()
{

}

void DBFRedactorMainWindow::open()
{

}
