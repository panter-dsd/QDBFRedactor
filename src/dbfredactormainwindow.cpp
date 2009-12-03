#include <QtCore/QSettings>
#include <QtCore/QTextStream>
#include <QtCore/QCoreApplication>

#include <QtGui/QTabBar>
#include <QtGui/QTableView>
#include <QtGui/QVBoxLayout>
#include <QtGui/QAction>
#include <QtGui/QMenuBar>
#include <QtGui/QToolBar>
#include <QtGui/QFileDialog>
#include <QtGui/QStatusBar>
#include <QtGui/QLabel>
#include <QtGui/QScrollBar>
#include <QtGui/QHeaderView>
#include <QtGui/QApplication>
#include <QtGui/QClipboard>
#include <QtGui/QProgressBar>
#include <QtGui/QMessageBox>

#include "dbfredactormainwindow.h"
#include "dbfredactorpage.h"

DBFRedactorMainWindow::DBFRedactorMainWindow(QWidget* parent, Qt::WFlags f)
		: QMainWindow(parent, f), currentPage(0)
{
	tabBar = new QTabBar(this);
	tabBar->setTabsClosable(true);
	tabBar->setContextMenuPolicy(Qt::ActionsContextMenu);
	connect(tabBar, SIGNAL(currentChanged(int)), this, SLOT(tabChanged(int)));
	connect(tabBar, SIGNAL(tabCloseRequested(int)), this, SLOT(closeTab(int)));

	view = new QTableView(this);
	view->setVisible(false);
	view->setContextMenuPolicy(Qt::ActionsContextMenu);

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

	actionClose = new QAction(QIcon(":/share/images/close.png"), tr("C&lose"), this);
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

	actionCopy = new QAction(QIcon(":/share/images/copy.png"), tr("&Copy"), this);
	actionCopy->setToolTip(tr("Copy current cell to clipboard"));
	actionCopy->setShortcut(Qt::CTRL + Qt::Key_C);
	connect(actionCopy, SIGNAL(triggered()), this, SLOT(copyToClipboard()));
	addAction(actionCopy);
	view->addAction(actionCopy);

	acionExportToHtml = new QAction(QIcon(":/share/images/exportToHtml.png"), tr("&Export to html"), this);
	acionExportToHtml->setToolTip(tr("Export to html"));
	connect(acionExportToHtml, SIGNAL(triggered()), this, SLOT(exportToHtml()));
	addAction(acionExportToHtml);
//Menus
	QMenuBar *menuBar = new QMenuBar(this);
	setMenuBar(menuBar);

	QMenu *fileMenu = new QMenu(tr("&File"), menuBar);
	fileMenu->addAction(actionOpen);
	fileMenu->addAction(actionClose);
	fileMenu->addSeparator();
	fileMenu->addAction(actionExit);

	menuBar->addMenu(fileMenu);

	QMenu *exportMenu = new QMenu(tr("&Export"), menuBar);
	exportMenu->addAction(acionExportToHtml);

	menuBar->addMenu(exportMenu);

//ToolBars
	QToolBar *fileToolBar = new QToolBar(this);
	fileToolBar->setObjectName("file_toolbar");
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
		if (pages.contains(fileName)) {
			for (int i = 0; i < tabBar->count(); i++) {
				if (tabBar->tabData(i).toString() == fileName) {
					index = i;
					break;
				}
			}
			continue;
		}
		DBFRedactorPage *page = new DBFRedactorPage(fileName, this);

		pages.insert(fileName, page);
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
	if (currentPage) {
		disconnect(currentPage->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
				this, SLOT(selectionChanged()));
		currentPage->setPos(view->horizontalScrollBar()->value(),
							view->verticalScrollBar()->value());
		currentPage = 0;
	}

	DBFRedactorPage *page = pages.value(tabBar->tabData(index).toString());
	if (!page) {
		view->setModel(0);
		return;
	}
	view->setModel(page->model());
	view->setSelectionModel(page->selectionModel());
	view->horizontalScrollBar()->setValue(page->pos().x());
	view->verticalScrollBar()->setValue(page->pos().y());

	connect(page->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
			this, SLOT(selectionChanged()));

	currentPage = page;
	currentFile->setText(tabBar->tabToolTip(index));
	selectionChanged();
}

void DBFRedactorMainWindow::closeTab(int index)
{
	DBFRedactorPage *page = pages.value(tabBar->tabData(index).toString());
	if (!page)
		return;
	delete page;
	pages.remove(tabBar->tabData(index).toString());
	currentPage = 0;
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
	DBFRedactorPage *page = pages.value(tabBar->tabData(tabBar->currentIndex()).toString());
	if (page)
		page->model()->refresh();
}

void DBFRedactorMainWindow::selectionChanged()
{
	double sum = 0.0, value = 0.0;
	bool ok = false;

	foreach(const QModelIndex& index, view->selectionModel()->selection().indexes()) {
		value = index.data(Qt::DisplayRole).toDouble(&ok);
		if (ok)
			sum += value;
	}
	sumLabel->setText(tr("Sum") + " = " + QString::number(sum, 'f', 2));
}

void DBFRedactorMainWindow::copyToClipboard()
{
	QMap<int, QString> text;
	foreach(const QModelIndex& index, view->selectionModel()->selection().indexes()) {
		if (!text.contains(index.row()))
			text.insert(index.row(), QString());
		if (text.value(index.row()).isEmpty())
			text[index.row()] += index.data(Qt::DisplayRole).toString();
		else
			text[index.row()] += "\t" + index.data(Qt::DisplayRole).toString();
	}
	QApplication::clipboard()->setText(QStringList(text.values()).join("\n"));
}

QStringList DBFRedactorMainWindow::prepareHtml()
{
	QStringList html;
	html << "<html><head><meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\" /></head><body>";
	html << "<TABLE BORDER=1 BORDERCOLOR=\"#000000\" CELLPADDING=4 CELLSPACING=0 align=left>";
	html << "<TR VALIGN=TOP>";
	for (int i = 0; i < view->model()->columnCount(); i++)
		html << "<TD align=center> " + view->model()->headerData(i, Qt::Horizontal, Qt::DisplayRole).toString() + "</TD>";
	html << "</TR>";
	QProgressBar *bar = new QProgressBar(this);
	bar->setFormat(tr("Preparing. %p% to finish."));
	bar->setRange(0, view->model()->rowCount());
	bar->setValue(0);
	bar->resize(statusBar()->size());
	bar->move(statusBar()->pos());
	bar->show();
	for (int i = 0; i < view->model()->rowCount(); i++) {
		bar->setValue(i);
		if (i / 100 * 100 == i)
			QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
		html << "<TR>";
		for (int j = 0; j < view->model()->columnCount(); j++)
			html << "<TD align=left> " + view->model()->index(i, j).data(Qt::DisplayRole).toString() + "</TD>";
		html << "</TR>";
	}
	html << "</TABLE></body></html>";
	delete bar;
	return html;
}

void DBFRedactorMainWindow::exportToHtml()
{
	QSettings settings;
	const QString& fileName = QFileDialog::getSaveFileName(this,
														   tr("Save"),
														   settings.value("Global/ExportPath", "'").toString(),
														   tr("HTML files (*.html)"));
	if (fileName.isEmpty())
		return;

	settings.setValue("Global/ExportPath", fileName);

	const QStringList& l(prepareHtml());

	QProgressBar *bar = new QProgressBar(this);
	bar->setFormat(tr("Saving. %p% to finish."));
	bar->setRange(0, l.size());
	bar->setValue(0);
	bar->resize(statusBar()->size());
	bar->move(statusBar()->pos());
	bar->show();

	QFile file(fileName);
	file.open(QIODevice::WriteOnly);


	QTextStream stream(&file);
	stream.setCodec("UTF-8");

	for (int i = 0; i < l.size(); i++) {
		if (i / 100 * 100 == i)
			QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
		stream << l.at(i) << endl;
		bar->setValue(i);
	}
	file.close();
	delete bar;

	QMessageBox::information(this, "", tr("Export finished"));
}
