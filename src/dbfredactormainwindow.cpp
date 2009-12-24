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
#include <QtGui/QComboBox>

#include <QtXml/QXmlStreamWriter>

#include "dbfredactormainwindow.h"
#include "dbfredactorpage.h"
#include "sortdialog.h"
#include "filterdialog.h"
#include "dbfredactordelegate.h"

#define ProcessEventsPeriod 500

DBFRedactorMainWindow::DBFRedactorMainWindow(QWidget* parent, Qt::WFlags f)
		: QMainWindow(parent, f), currentPage(0), progressBar(0)
{
	tabBar = new QTabBar(this);
	tabBar->setTabsClosable(true);
	tabBar->setContextMenuPolicy(Qt::ActionsContextMenu);
	connect(tabBar, SIGNAL(currentChanged(int)), this, SLOT(tabChanged(int)));
	connect(tabBar, SIGNAL(tabCloseRequested(int)), this, SLOT(closeTab(int)));

	view = new QTableView(this);
	view->setVisible(false);
	view->setContextMenuPolicy(Qt::ActionsContextMenu);
	view->verticalHeader()->setContextMenuPolicy(Qt::ActionsContextMenu);
	view->horizontalHeader()->setContextMenuPolicy(Qt::ActionsContextMenu);
	connect(view->horizontalHeader(), SIGNAL(sectionClicked(int)), this, SLOT(sort(int)));


#ifndef Q_CC_MSVC
	#warning "+ 10 is harcod"
#endif
	view->verticalHeader()->setDefaultSectionSize(view->fontMetrics().width("W") + 10);

	setStatusBar(new QStatusBar(this));

	currentFile = new QLabel(this);
	statusBar()->insertWidget(0, currentFile);

	functionComboBox = new QComboBox(this);
	functionComboBox->addItems(QStringList() << tr("Sum")
							   << tr("Count")
							   << tr("Minimum")
							   << tr("Maximum")
							   << tr("Clear"));
	functionComboBox->setCurrentIndex(-1);
	connect(functionComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(selectionChanged()));
	statusBar()->insertWidget(1, functionComboBox);

	informationLabel = new QLabel(this);
	statusBar()->insertWidget(2, informationLabel);

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

	acionExportToXml = new QAction(QIcon(":/share/images/exportToXml.png"), tr("&Export to xml"), this);
	acionExportToXml->setToolTip(tr("Export to xml"));
	connect(acionExportToXml, SIGNAL(triggered()), this, SLOT(exportToXml()));
	addAction(acionExportToXml);

	acionExportToCsv = new QAction(QIcon(":/share/images/exportToCsv.png"), tr("&Export to csv"), this);
	acionExportToCsv->setToolTip(tr("Export to csv"));
	connect(acionExportToCsv, SIGNAL(triggered()), this, SLOT(exportToCsv()));
	addAction(acionExportToCsv);

	actionResizeColumnsToContents = new QAction(tr("Resize columns to contents"), this);
	actionResizeColumnsToContents->setToolTip(tr("Resize columns to contents"));
	connect(actionResizeColumnsToContents, SIGNAL(triggered()), view, SLOT(resizeColumnsToContents()));
	view->horizontalHeader()->addAction(actionResizeColumnsToContents);

	actionUnsort = new QAction(tr("Unsort"), this);
	actionUnsort->setToolTip(tr("Remove sort"));
	connect(actionUnsort, SIGNAL(triggered()), this, SLOT(unsort()));
	view->horizontalHeader()->addAction(actionUnsort);

	actionChangeSort = new QAction(tr("Change sort"), this);
	actionChangeSort->setToolTip(tr("Change sort"));
	connect(actionChangeSort, SIGNAL(triggered()), this, SLOT(changeSort()));
	view->horizontalHeader()->addAction(actionChangeSort);

	actionChangeFilter = new QAction(tr("Change filter"), this);
	actionChangeFilter->setToolTip(tr("Change filter"));
	actionChangeFilter->setIcon(QIcon(":/share/images/filter.png"));
	connect(actionChangeFilter, SIGNAL(triggered()), this, SLOT(changeFilter()));
	view->horizontalHeader()->addAction(actionChangeFilter);

	actionAddIncludeFilter = new QAction(tr("Add include filter"), this);
	actionAddIncludeFilter->setToolTip(tr("Add include filter"));
	connect(actionAddIncludeFilter, SIGNAL(triggered()), this, SLOT(addIncludeFilter()));
	view->addAction(actionAddIncludeFilter);

	actionAddExcludeFilter = new QAction(tr("Add exclude filter"), this);
	actionAddExcludeFilter->setToolTip(tr("Add exclude filter"));
	connect(actionAddExcludeFilter, SIGNAL(triggered()), this, SLOT(addExcludeFilter()));
	view->addAction(actionAddExcludeFilter);

	actionRemoveFilter = new QAction(tr("Remove filter"), this);
	actionRemoveFilter->setToolTip(tr("Remove filter"));
	actionRemoveFilter->setIcon(QIcon(":/share/images/removefilter.png"));
	connect(actionRemoveFilter, SIGNAL(triggered()), this, SLOT(removeFilter()));
	view->addAction(actionRemoveFilter);

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
	exportMenu->addAction(acionExportToXml);
	exportMenu->addAction(acionExportToCsv);

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
		QList<int> columnSizes;
		for (int i = 0; i < view->model()->columnCount(); i++)
			columnSizes << view->columnWidth(i);
		currentPage->setColumnSizes(columnSizes);
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
	view ->setItemDelegate(new DBFRedactorDelegate(page->redactor(), view));

	{
		int i = 0;
		foreach(int size, page->columnSizes())
			view->setColumnWidth(i++, size);
	}

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
	currentFile->setVisible(currentPage);
	informationLabel->setVisible(currentPage);
	acionExportToHtml->setEnabled(currentPage);
	acionExportToXml->setEnabled(currentPage);
	acionExportToCsv->setEnabled(currentPage);
	functionComboBox->setVisible(currentPage);
}

void DBFRedactorMainWindow::refreshModel()
{
	DBFRedactorPage *page = pages.value(tabBar->tabData(tabBar->currentIndex()).toString());
	if (page)
		page->refresh();
}

void DBFRedactorMainWindow::selectionChanged()
{
	switch (functionComboBox->currentIndex()) {
		case 0:
			informationLabel->setText(functionComboBox->currentText() + QString(" = %1").arg(sum(), 0, 'f', 2));
			break;
		case 1:
			informationLabel->setText(functionComboBox->currentText() + QString(" = %1").arg(count(), 0, 'f', 2));
			break;
		case 2:
			informationLabel->setText(functionComboBox->currentText() + QString(" = %1").arg(min(), 0, 'f', 2));
			break;
		case 3:
			informationLabel->setText(functionComboBox->currentText() + QString(" = %1").arg(max(), 0, 'f', 2));
			break;
		default:
			functionComboBox->setCurrentIndex(-1);
			informationLabel->clear();
	}
}

double DBFRedactorMainWindow::sum()
{
	const QModelIndexList& indexes = view->selectionModel()->selection().indexes();

	progressBar = new QProgressBar(this);
	progressBar->setFormat(tr("Calculating. %p% to finish."));
	progressBar->setRange(0, indexes.size());
	progressBar->setValue(0);
	statusBar()->addWidget(progressBar, 1);

	double sum = 0.0, value = 0.0;
	bool ok = false;
	int i = 0;

	foreach(const QModelIndex& index, indexes) {
		i++;
		if (i / ProcessEventsPeriod * ProcessEventsPeriod == i) {
			progressBar->setValue(i);
			QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
		}
		value = index.data(Qt::DisplayRole).toDouble(&ok);
		if (ok)
			sum += value;
	}
	delete progressBar;
	progressBar = 0;

	return sum;
}

double DBFRedactorMainWindow::count()
{
	const QModelIndexList& indexes = view->selectionModel()->selection().indexes();

	return indexes.size();
}

double DBFRedactorMainWindow::min()
{
	const QModelIndexList& indexes = view->selectionModel()->selection().indexes();

	progressBar = new QProgressBar(this);
	progressBar->setFormat(tr("Calculating. %p% to finish."));
	progressBar->setRange(0, indexes.size());
	progressBar->setValue(0);
	statusBar()->addWidget(progressBar, 1);

	double min = indexes.first().data(Qt::DisplayRole).toDouble(), value = 0.0;
	bool ok = false;
	int i = 0;

	foreach(const QModelIndex& index, indexes) {
		i++;
		if (i / ProcessEventsPeriod * ProcessEventsPeriod == i) {
			progressBar->setValue(i);
			QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
		}		value = index.data(Qt::DisplayRole).toDouble(&ok);
		if (ok)
			if (min > value)
				min = value;
	}
	delete progressBar;
	progressBar = 0;

	return min;
}

double DBFRedactorMainWindow::max()
{
	const QModelIndexList& indexes = view->selectionModel()->selection().indexes();

	progressBar = new QProgressBar(this);
	progressBar->setFormat(tr("Calculating. %p% to finish."));
	progressBar->setRange(0, indexes.size());
	progressBar->setValue(0);
	statusBar()->addWidget(progressBar, 1);

	double max = indexes.first().data(Qt::DisplayRole).toDouble(), value = 0.0;
	bool ok = false;
	int i = 0;

	foreach(const QModelIndex& index, indexes) {
		i++;
		if (i / ProcessEventsPeriod * ProcessEventsPeriod == i) {
			progressBar->setValue(i);
			QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
		}		value = index.data(Qt::DisplayRole).toDouble(&ok);
		if (ok)
			if (max < value)
				max = value;
	}
	delete progressBar;
	progressBar = 0;

	return max;
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
	html << "<HTML><HEAD><meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\" /></HEAD>\n<BODY>";
	html << "<TABLE BORDER=1 BORDERCOLOR=\"#000000\" CELLPADDING=4  CELLSPACING=0 WIDTH=\"100%\" ALIGN=LEFT>";
	html << "<COLGROUP>";
	for (int i = 0; i < view->model()->columnCount(); i++)
		html << QString("<COL WIDTH=%1%>").arg(100 / view->model()->columnCount());
	html << "<TBODY>";
	html << "<TR VALIGN=TOP>";
	for (int i = 0; i < view->model()->columnCount(); i++)
		html << "<TD ALIGN=CENTER> " + view->model()->headerData(i, Qt::Horizontal, Qt::DisplayRole).toString() + "</TD>";
	html << "</TR>";
	Q_ASSERT (progressBar == 0);
	progressBar = new QProgressBar(this);
	progressBar->setFormat(tr("Preparing. %p% to finish."));
	progressBar->setRange(0, view->model()->rowCount());
	progressBar->setValue(0);
	statusBar()->addWidget(progressBar, 1);

	for (int i = 0; i < view->model()->rowCount(); i++) {
		QString tempStrting;
		if (i / ProcessEventsPeriod * ProcessEventsPeriod == i) {
			progressBar->setValue(i);
			QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
		}
		tempStrting += "<TR ALIGN=LEFT>";
		for (int j = 0; j < view->model()->columnCount(); j++) {
			QString value = view->model()->index(i, j).data(Qt::DisplayRole).toString();
			if (value.isEmpty())
				value = "<BR>";
			tempStrting += "\n<TD><P>" + value + "</P></TD>";
		}
		tempStrting += "\n</TR>";
		html << tempStrting;
	}
	html << "</TBODY></TABLE></BODY></HTML>";
	delete progressBar;
	progressBar = 0;
	return html;
}

void DBFRedactorMainWindow::exportToHtml()
{
	QSettings settings;
	const QString& fileName = QFileDialog::getSaveFileName(this,
														   tr("Save"),
														   settings.value("Global/ExportPath", "'").toString() + "/" + currentPage->redactor()->tableName(),
														   tr("HTML files (*.html)"));
	if (fileName.isEmpty())
		return;

	settings.setValue("Global/ExportPath", QFileInfo(fileName).absolutePath());

	const QStringList& l(prepareHtml());

	Q_ASSERT (progressBar == 0);
	progressBar = new QProgressBar(this);
	progressBar->setFormat(tr("Saving. %p% to finish."));
	progressBar->setRange(0, l.size());
	progressBar->setValue(0);
	statusBar()->addWidget(progressBar, 1);

	QFile file(fileName);
	file.open(QIODevice::WriteOnly);


	QTextStream stream(&file);
	stream.setCodec("UTF-8");

	for (int i = 0; i < l.size(); i++) {
		if (i / ProcessEventsPeriod * ProcessEventsPeriod == i) {
			progressBar->setValue(i);
			QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
		}
		stream << l.at(i) << endl;
	}
	file.close();
	delete progressBar;
	progressBar = 0;

	QMessageBox::information(this, "", tr("Export finished"));
}

bool DBFRedactorMainWindow::event(QEvent *ev)
{
//	if (ev->type() == QEvent::Resize || ev->type() == QEvent::Move) {
//		if (progressBar) {
//			progressBar->resize(statusBar()->size());
//			progressBar->move(statusBar()->pos());
//		}
//	}
	return QMainWindow::event(ev);
}

void DBFRedactorMainWindow::exportToXml()
{
	QSettings settings;
	const QString& fileName = QFileDialog::getSaveFileName(this,
														   tr("Save"),
														   settings.value("Global/ExportPath", "'").toString() + "/" + currentPage->redactor()->tableName(),
														   tr("XML files (*.xml)"));
	if (fileName.isEmpty())
		return;

	settings.setValue("Global/ExportPath", QFileInfo(fileName).absolutePath());

	Q_ASSERT (progressBar == 0);
	progressBar = new QProgressBar(this);
	progressBar->setFormat(tr("Saving. %p% to finish."));
	progressBar->setRange(0, view->model()->rowCount());
	progressBar->setValue(0);
	statusBar()->addWidget(progressBar, 1);

	QFile file(fileName);
	file.open(QIODevice::WriteOnly);

	QXmlStreamWriter stream(&file);
	stream.setAutoFormatting(true);
	stream.writeStartDocument();
	stream.writeStartElement("ROWDATA");

	for (int i = 0; i < view->model()->rowCount(); i++) {
		stream.writeStartElement("ROW");

		if (i / ProcessEventsPeriod * ProcessEventsPeriod == i) {
			progressBar->setValue(i);
			QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
		}
		for (int j = 0; j < view->model()->columnCount(); j++) {
			QString value = view->model()->index(i, j).data(Qt::DisplayRole).toString();
			stream.writeTextElement(view->model()->headerData(j, Qt::Horizontal, Qt::DisplayRole).toString(),
									value);
		}
		stream.writeEndElement();
	}

	stream.writeEndElement();
	stream.writeEndDocument();
	file.close();
	delete progressBar;
	progressBar = 0;

	QMessageBox::information(this, "", tr("Export finished"));
}

void DBFRedactorMainWindow::exportToCsv()
{
	QSettings settings;
	const QString& fileName = QFileDialog::getSaveFileName(this,
														   tr("Save"),
														   settings.value("Global/ExportPath", "'").toString() + "/" + currentPage->redactor()->tableName(),
														   tr("CSV files (*.csv)"));
	if (fileName.isEmpty())
		return;

	settings.setValue("Global/ExportPath", QFileInfo(fileName).absolutePath());

	Q_ASSERT (progressBar == 0);
	progressBar = new QProgressBar(this);
	progressBar->setFormat(tr("Saving. %p% to finish."));
	progressBar->setRange(0, view->model()->rowCount());
	progressBar->setValue(0);
	statusBar()->addWidget(progressBar, 1);

	QFile file(fileName);
	file.open(QIODevice::WriteOnly);

	QTextStream stream(&file);
	stream.setCodec("UTF-8");


	QStringList tempStringList;
	for (int j = 0; j < view->model()->columnCount(); j++)
		tempStringList << "\"" + view->model()->headerData(j, Qt::Horizontal, Qt::DisplayRole).toString() + "\"";

	stream << tempStringList.join("|") << endl;
	tempStringList.clear();

	for (int i = 0; i < view->model()->rowCount(); i++) {
		if (i / ProcessEventsPeriod * ProcessEventsPeriod == i) {
			progressBar->setValue(i);
			QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
		}
		for (int j = 0; j < view->model()->columnCount(); j++) {
			switch (currentPage->redactor()->field(j).type) {
				case DBFRedactor::TYPE_NUMERIC: case DBFRedactor::TYPE_FLOAT:
					tempStringList << view->model()->index(i, j).data(Qt::DisplayRole).toString();
					break;
				case DBFRedactor::TYPE_DATE:
					tempStringList << view->model()->index(i, j).data(Qt::DisplayRole).toString();
					break;
				case DBFRedactor::TYPE_CHAR: default:
					tempStringList << "\"" + view->model()->index(i, j).data(Qt::DisplayRole).toString() + "\"";
			}
		}
		stream << tempStringList.join("|") << endl;
		tempStringList.clear();
	}

	file.close();
	delete progressBar;
	progressBar = 0;

	QMessageBox::information(this, "", tr("Export finished"));
}

void DBFRedactorMainWindow::sort(int section)
{
	Qt::SortOrder order = currentPage->model()->sortOrder();
	if (currentPage->model()->sortColumn() == section)
		order = currentPage->model()->sortOrder() == Qt::AscendingOrder
				? Qt::DescendingOrder
				: Qt::AscendingOrder;

	if (qApp->keyboardModifiers() & Qt::CTRL) {
		if (currentPage->model()->isColumnInSort(section)) {
			Qt::SortOrder order = currentPage->model()->sortOrder(section) == Qt::AscendingOrder
								  ? Qt::DescendingOrder
								  : Qt::AscendingOrder;
			currentPage->model()->changeSortedColumn(section, order);
		} else {
			currentPage->model()->addSortedColumn(section, currentPage->model()->sortOrder());
		}
	} else {
		Qt::SortOrder order = currentPage->model()->sortOrder();
		if (currentPage->model()->isColumnInSort(section)) {
			order = currentPage->model()->sortOrder(section) == Qt::AscendingOrder
					? Qt::DescendingOrder
					 : Qt::AscendingOrder;
		}
		currentPage->model()->clearSort();
		currentPage->model()->addSortedColumn(section, order);
	}
}

void DBFRedactorMainWindow::unsort()
{
	currentPage->model()->clearSort();
}

void DBFRedactorMainWindow::changeSort()
{
	QHash<int, QString> captions;

	for (int i = 0; i < currentPage->model()->columnCount(); i++)
		captions.insert(i, currentPage->model()->headerData(i, Qt::Horizontal, Qt::EditRole).toString());

	SortDialog dialog(captions, this);
	dialog.setSortedColumns(currentPage->model()->sortedColumns());

	if (dialog.exec())
		currentPage->model()->setSortedColumns(dialog.sortedColumns());
}

void DBFRedactorMainWindow::changeFilter()
{
	QHash<int, QString> captions;

	for (int i = 0; i < currentPage->model()->columnCount(); i++)
		captions.insert(i, currentPage->model()->headerData(i, Qt::Horizontal, Qt::EditRole).toString());

	FilterDialog dialog(captions, this);
	dialog.setFilter(currentPage->model()->filter());

	if (dialog.exec())
		currentPage->model()->setFilter(dialog.filter());
}

void DBFRedactorMainWindow::addIncludeFilter()
{
	const QModelIndex& index = view->currentIndex();
	if (!index.isValid())
		return;

	DBFRedactorSortFilterProxyModel::FilterItem item;
	item.filterOperator = DBFRedactorSortFilterProxyModel::AND;
	item.column = index.column();
	item.uslovie = DBFRedactorSortFilterProxyModel::Equal;
	item.regExp.setPattern(index.data(Qt::DisplayRole).toString());
	item.regExp.setPatternSyntax(QRegExp::FixedString);
	item.regExp.setCaseSensitivity(Qt::CaseInsensitive);

	currentPage->model()->addFilter(item);
}

void DBFRedactorMainWindow::addExcludeFilter()
{
	const QModelIndex& index = view->currentIndex();
	if (!index.isValid())
		return;

	DBFRedactorSortFilterProxyModel::FilterItem item;
	item.filterOperator = DBFRedactorSortFilterProxyModel::AND;
	item.column = index.column();
	item.uslovie = DBFRedactorSortFilterProxyModel::NotEqual;
	item.regExp.setPattern(index.data(Qt::DisplayRole).toString());
	item.regExp.setPatternSyntax(QRegExp::FixedString);
	item.regExp.setCaseSensitivity(Qt::CaseInsensitive);

	currentPage->model()->addFilter(item);
}

void DBFRedactorMainWindow::removeFilter()
{
	currentPage->model()->removeFilter();
}
