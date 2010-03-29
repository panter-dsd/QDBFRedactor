/********************************************************************
* Copyright (C) PanteR
*-------------------------------------------------------------------
*
* QDBFRedactor is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License as
* published by the Free Software Foundation; either version 2
* of the License, or (at your option) any later version.
*
* QDBFRedactor is distributed in the hope that it will be
* useful, but WITHOUT ANY WARRANTY; without even the implied
* warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
* See the GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with Panther Commander; if not, write to the Free Software
* Foundation, Inc., 51 Franklin St, Fifth Floor,
* Boston, MA 02110-1301 USA
*-------------------------------------------------------------------
* Project:		QDBFRedactor
* Author:		PanteR
* Contact:		panter.dsd@gmail.com
*******************************************************************/

#include <QtCore/QSettings>
#include <QtCore/QTextStream>
#include <QtCore/QCoreApplication>
#include <QtCore/QUrl>
#include <QtCore/QDebug>

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
#include <QtGui/QDragEnterEvent>
#include <QtGui/QDropEvent>
#include <QtGui/QPrintPreviewDialog>
#include <QtGui/QTextEdit>
#include <QtGui/QPrinter>
#include <QtGui/QPainter>

#include <QtXml/QXmlStreamWriter>

#include "dbfredactormainwindow.h"
#include "dbfredactorpage.h"
#include "sortdialog.h"
#include "filterdialog.h"
#include "preferencesdialog.h"
#include "aboutdialog.h"

#define ProcessEventsPeriod 500

DBFRedactorMainWindow::DBFRedactorMainWindow(QWidget* parent, Qt::WFlags f)
		: QMainWindow(parent, f), currentPage(0), progressBar(0)
{
	setAcceptDrops(true);
	resize (800, 600);

	trayIcon = new QSystemTrayIcon(this);
	trayIcon->setIcon(QApplication::windowIcon());
	connect(trayIcon, SIGNAL(activated (QSystemTrayIcon::ActivationReason)),
					this, SLOT(trayClicked(QSystemTrayIcon::ActivationReason)));

	tabBar = new QTabBar(this);
	tabBar->setTabsClosable(true);
	tabBar->setContextMenuPolicy(Qt::ActionsContextMenu);
	tabBar->setFocusPolicy(Qt::NoFocus);
	connect(tabBar, SIGNAL(currentChanged(int)), this, SLOT(tabChanged(int)));
	connect(tabBar, SIGNAL(tabCloseRequested(int)), this, SLOT(closeTab(int)));

	view = new QTableView(this);
	view->setContextMenuPolicy(Qt::ActionsContextMenu);
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
	for (int i = 0; i < 5; i++)
		functionComboBox->addItem("");
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
	actionOpen = new QAction(this);
	actionOpen->setShortcut(QKeySequence::Open);
	actionOpen->setIcon(QIcon(":/share/images/open.png"));
	connect(actionOpen, SIGNAL(triggered()), this, SLOT(open()));

	actionSave = new QAction(this);
	actionSave->setShortcut(QKeySequence::Save);
	actionSave->setIcon(QIcon(":/share/images/save.png"));
	connect(actionSave, SIGNAL(triggered()), this, SLOT(save()));

	actionExit = new QAction(this);
	actionExit->setIcon(QIcon(":/share/images/exit.png"));
	actionExit->setShortcut(Qt::ALT + Qt::Key_X);
	connect(actionExit, SIGNAL(triggered()), qApp, SLOT( quit ()));

	actionClose = new QAction(this);
	actionClose->setIcon(QIcon(":/share/images/close.png"));
	actionClose->setShortcut(Qt::CTRL + Qt::Key_W);
	connect(actionClose, SIGNAL(triggered()), this, SLOT(closeCurrentTab()));
	tabBar->addAction(actionClose);

	actionRefresh = new QAction(this);
	actionRefresh->setIcon(QIcon(":/share/images/refresh.png"));
	actionRefresh->setShortcut(Qt::CTRL + Qt::Key_R);
	connect(actionRefresh, SIGNAL(triggered()), this, SLOT(refreshModel()));
	tabBar->addAction(actionRefresh);

	actionCopy = new QAction(this);
	actionCopy->setIcon(QIcon(":/share/images/copy.png"));
	actionCopy->setShortcut(Qt::CTRL + Qt::Key_C);
	connect(actionCopy, SIGNAL(triggered()), this, SLOT(copyToClipboard()));
	view->addAction(actionCopy);

	acionExportToHtml = new QAction(this);
	acionExportToHtml->setIcon(QIcon(":/share/images/exportToHtml.png"));
	connect(acionExportToHtml, SIGNAL(triggered()), this, SLOT(exportToHtml()));

	acionExportToXml = new QAction(this);
	acionExportToXml->setIcon(QIcon(":/share/images/exportToXml.png"));
	connect(acionExportToXml, SIGNAL(triggered()), this, SLOT(exportToXml()));

	acionExportToCsv = new QAction(this);
	acionExportToCsv->setIcon(QIcon(":/share/images/exportToCsv.png"));
	connect(acionExportToCsv, SIGNAL(triggered()), this, SLOT(exportToCsv()));

	actionResizeColumnsToContents = new QAction(this);
	connect(actionResizeColumnsToContents, SIGNAL(triggered()), view, SLOT(resizeColumnsToContents()));
	view->horizontalHeader()->addAction(actionResizeColumnsToContents);

	actionUnsort = new QAction(this);
	connect(actionUnsort, SIGNAL(triggered()), this, SLOT(unsort()));
	view->horizontalHeader()->addAction(actionUnsort);

	actionChangeSort = new QAction(this);
	connect(actionChangeSort, SIGNAL(triggered()), this, SLOT(changeSort()));
	view->horizontalHeader()->addAction(actionChangeSort);

	actionChangeFilter = new QAction(this);
	actionChangeFilter->setIcon(QIcon(":/share/images/filter.png"));
	connect(actionChangeFilter, SIGNAL(triggered()), this, SLOT(changeFilter()));
	view->horizontalHeader()->addAction(actionChangeFilter);

	actionAddIncludeFilter = new QAction(this);
	connect(actionAddIncludeFilter, SIGNAL(triggered()), this, SLOT(addIncludeFilter()));
	view->addAction(actionAddIncludeFilter);

	actionAddExcludeFilter = new QAction(this);
	connect(actionAddExcludeFilter, SIGNAL(triggered()), this, SLOT(addExcludeFilter()));
	view->addAction(actionAddExcludeFilter);

	actionRemoveFilter = new QAction(this);
	actionRemoveFilter->setIcon(QIcon(":/share/images/removefilter.png"));
	connect(actionRemoveFilter, SIGNAL(triggered()), this, SLOT(removeFilter()));
	view->horizontalHeader()->addAction(actionRemoveFilter);

	actionPreferences = new QAction(this);
	actionPreferences->setIcon(QIcon(":/share/images/preferences.png"));
	connect(actionPreferences, SIGNAL(triggered()), this, SLOT(preferences()));

	actionSetEditMode = new QAction(this);
	actionSetEditMode->setIcon(QIcon(":/share/images/edit.png"));
	actionSetEditMode->setCheckable(true);
	actionSetEditMode->setChecked(false);
	connect(actionSetEditMode, SIGNAL(triggered(bool)), this, SLOT(setEditMode(bool)));

	actionAddRecord = new QAction(this);
	actionAddRecord->setIcon(QIcon(":/share/images/add.png"));
	actionAddRecord->setShortcut(Qt::Key_F2);
	connect(actionAddRecord, SIGNAL(triggered()), this, SLOT(addRecord()));
	view->addAction(actionAddRecord);

	actionRemoveRecord = new QAction(this);
	actionRemoveRecord->setIcon(QIcon(":/share/images/remove.png"));
	actionRemoveRecord->setShortcut(Qt::Key_F8);
	connect(actionRemoveRecord, SIGNAL(triggered()), this, SLOT(removeRecord()));
	view->addAction(actionRemoveRecord);

	actionRecoverRecord = new QAction(this);
	actionRecoverRecord->setIcon(QIcon(":/share/images/recover.png"));
	actionRecoverRecord->setShortcut(Qt::Key_F9);
	connect(actionRecoverRecord, SIGNAL(triggered()), this, SLOT(recoverRecord()));
	view->addAction(actionRecoverRecord);

	actionAbout = new QAction(this);
	actionAbout->setIcon(QIcon(":/share/images/about.png"));
	connect(actionAbout, SIGNAL(triggered()), this, SLOT(about()));

	actionAboutQt = new QAction(this);
	actionAboutQt->setIcon(QIcon(":/share/images/aboutQt.png"));
	connect(actionAboutQt, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

	actionHide = new QAction (this);
	connect(actionHide, SIGNAL(triggered()), this, SLOT(hide()));

	actionShow = new QAction (this);
	connect(actionShow, SIGNAL(triggered()), this, SLOT(show()));

	connect(actionHide, SIGNAL(triggered()), this, SLOT(updateHideShowActions ()));
	connect(actionShow, SIGNAL(triggered()), this, SLOT(updateHideShowActions ()));

	actionPrintPreview = new QAction (this);
	actionPrintPreview->setIcon(QIcon(":/share/images/preview.png"));
	connect(actionPrintPreview, SIGNAL(triggered()), this, SLOT(preview()));
//Menus
	QMenuBar *menuBar = new QMenuBar(this);
	setMenuBar(menuBar);

	historyMenu = new QMenu (this);

	fileMenu = new QMenu(menuBar);
	fileMenu->addAction(actionOpen);
	fileMenu->addAction(actionSave);
	fileMenu->addAction(actionClose);
	fileMenu->addAction(actionSetEditMode);
	fileMenu->addSeparator();
	fileMenu->addMenu(historyMenu);
	fileMenu->addSeparator();
	fileMenu->addAction(actionPreferences);
	fileMenu->addSeparator();
	fileMenu->addAction(actionPrintPreview);
	fileMenu->addSeparator();
	fileMenu->addAction(actionExit);

	menuBar->addMenu(fileMenu);

	exportMenu = new QMenu(menuBar);
	exportMenu->addAction(acionExportToHtml);
	exportMenu->addAction(acionExportToXml);
	exportMenu->addAction(acionExportToCsv);

	menuBar->addMenu(exportMenu);

	codecsMenu = new QMenu(menuBar);
	menuBar->addMenu(codecsMenu);

	helpMenu = new QMenu(menuBar);
	helpMenu->addAction(actionAbout);
	helpMenu->addAction(actionAboutQt);

	menuBar->addMenu(helpMenu);

//Tray menu
	QMenu *trayIconMenu = new QMenu(this);
	trayIconMenu->addAction(actionHide);
	trayIconMenu->addAction(actionShow);
	trayIconMenu->addSeparator();
	trayIconMenu->addAction(actionExit);
	trayIcon->setContextMenu(trayIconMenu);

//ToolBars
	fileToolBar = new QToolBar(this);
	fileToolBar->setObjectName("file_toolbar");
	fileToolBar->addAction(actionOpen);
	fileToolBar->addAction(actionSave);
	fileToolBar->addAction(actionSetEditMode);
	fileToolBar->addAction(actionClose);
	fileToolBar->addSeparator();
	fileToolBar->addAction(actionExit);
	addToolBar(fileToolBar);

	loadSettings();
	updateActions();
	retranslateStrings();
	createCodecsMenu();
}

DBFRedactorMainWindow::~DBFRedactorMainWindow()
{
	saveSettings();
	while(tabBar->count() > 0)
		closeTab(0);
}

void DBFRedactorMainWindow::retranslateStrings()
{
	functionComboBox->setItemText(0, tr("Sum"));
	functionComboBox->setItemText(1, tr("Count"));
	functionComboBox->setItemText(2, tr("Minimum"));
	functionComboBox->setItemText(3, tr("Maximum"));
	functionComboBox->setItemText(4, tr("Clear"));

	actionOpen->setText(tr("&Open"));
	actionOpen->setToolTip(tr("Open file"));

	actionSave->setText(tr("&Save"));
	actionSave->setToolTip(tr("Save file"));

	actionExit->setText(tr("E&xit"));
	actionExit->setToolTip(tr("Exit from program"));

	actionClose->setText(tr("C&lose"));
	actionClose->setToolTip(tr("Close current file"));

	actionRefresh->setText(tr("&Refresh"));
	actionRefresh->setToolTip(tr("Refresh current file"));

	actionCopy->setText(tr("&Copy"));
	actionCopy->setToolTip(tr("Copy selected cells to clipboard"));

	acionExportToHtml->setText(tr("Export to &html"));
	acionExportToHtml->setToolTip(tr("Export to html"));

	acionExportToXml->setText(tr("Export to &xml"));
	acionExportToXml->setToolTip(tr("Export to xml"));

	acionExportToCsv->setText(tr("Export to cs&v"));
	acionExportToCsv->setToolTip(tr("Export to csv"));

	actionResizeColumnsToContents->setText(tr("Resize columns to contents"));
	actionResizeColumnsToContents->setToolTip(tr("Resize columns to contents"));

	actionUnsort->setText(tr("Unsort"));
	actionUnsort->setToolTip(tr("Remove sort"));

	actionChangeSort->setText(tr("Change sort"));
	actionChangeSort->setToolTip(tr("Change sort"));

	actionChangeFilter->setText(tr("Change filter"));
	actionChangeFilter->setToolTip(tr("Change filter"));

	actionAddIncludeFilter->setText(tr("Add include filter"));
	actionAddIncludeFilter->setToolTip(tr("Add include filter"));

	actionAddExcludeFilter->setText(tr("Add exclude filter"));
	actionAddExcludeFilter->setToolTip(tr("Add exclude filter"));

	actionRemoveFilter->setText(tr("Remove filter"));
	actionRemoveFilter->setToolTip(tr("Remove filter"));

	actionPreferences->setText(tr("Preferences"));
	actionPreferences->setToolTip(tr("Application preferences"));

	actionSetEditMode->setText(tr("Edit"));
	actionSetEditMode->setText(tr("Edit this file"));

	actionAddRecord->setText(tr("Add record"));

	actionRemoveRecord->setText(tr("Remove record"));

	actionRecoverRecord->setText(tr("Recover record"));

	actionAbout->setText(tr("About..."));

	actionAboutQt->setText(tr("About Qt"));

	actionHide->setText(tr ("Hide"));
	actionShow->setText(tr ("Show"));

	actionPrintPreview->setText(tr ("Preview"));
	historyMenu->setTitle(tr ("Recently opened files"));
	fileMenu->setTitle(tr("&File"));
	exportMenu->setTitle(tr("&Export"));
	codecsMenu->setTitle(tr("&Codecs"));
	helpMenu->setTitle(tr("&Help"));

	fileToolBar->setWindowTitle(tr("&File"));
}

void DBFRedactorMainWindow::loadSettings()
{
	QSettings settings;

	if (settings.value("Global/RestoreWindowParams", true).toBool()) {
		settings.beginGroup("MainWindow");
		move(settings.value("pos", QPoint(0, 0)).toPoint());
		resize(settings.value("size", QSize(640, 480)).toSize());
		bool isMaximized = settings.value("IsMaximized", false).toBool();
		if (isMaximized)
			setWindowState(Qt::WindowMaximized);
		restoreState(settings.value("State", "").toByteArray());
		settings.endGroup();
	}

	m_history = settings.value("Global/History", QStringList()).toStringList();
	updateHistoryMenu ();

	reloadSettings ();
}

void DBFRedactorMainWindow::reloadSettings ()
{
	QSettings settings;

	if (settings.value("Global/TrayAlwaysShow", true).toBool())
		trayIcon->show();
	else
		trayIcon->hide ();

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
	settings.endGroup();

	settings.setValue("Global/History", m_history);

	settings.sync();
}

void DBFRedactorMainWindow::open()
{
	QSettings settings;
	settings.beginGroup("Global");
	const QStringList& fileNames = QFileDialog::getOpenFileNames(this,
															tr("Open files"),
															settings.value("OpenPath", QDir::homePath()).toString(),
															tr("DBase files (*.dbf);;All files(*)"));
	if (fileNames.isEmpty())
		return;

	settings.setValue("OpenPath", QFileInfo(fileNames.first()).absolutePath());

	openFiles(fileNames);
}

void DBFRedactorMainWindow::save()
{
	if (!currentPage->redactor()->save())
		QMessageBox::critical(this, "", tr("Saving error"));
	updateActions();
}

void DBFRedactorMainWindow::openFiles(const QStringList& fileList)
{
	int index = 0;
	foreach(const QString& fileName, fileList) {
		QFileInfo fi (fileName);
		if (!fi.exists())
			continue;

		if (pages.contains(fi.absoluteFilePath())) {
			for (int i = 0; i < tabBar->count(); i++) {
				if (tabBar->tabData(i).toString() == fi.absoluteFilePath()) {
					index = i;
					break;
				}
			}
		} else {
			DBFRedactorPage *page = new DBFRedactorPage(fi.absoluteFilePath(), this);

			pages.insert(fi.absoluteFilePath(), page);
			index = tabBar->addTab(fi.fileName());
			tabBar->setTabData(index, fi.absoluteFilePath());
			tabBar->setTabToolTip(index, QDir::toNativeSeparators(fi.absoluteFilePath()));
			addToHistory(fi.absoluteFilePath());
		}
	}
	tabBar->setCurrentIndex(index);
	tabChanged(index);
	updateActions();
	view->setFocus();
}

void DBFRedactorMainWindow::tabChanged(int index)
{
	if (index < 0)
		return;

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
	if (!page)
		return;

	view->setModel(page->model());
	view->setSelectionModel(page->selectionModel());
	view->horizontalScrollBar()->setValue(page->pos().x());
	view->verticalScrollBar()->setValue(page->pos().y());
	view->setItemDelegate(page->delegate());

	for (int i = 0; i < page->columnSizes().size(); i++)
		view->setColumnWidth(i, page->columnSizes().at(i));

	connect(page->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
			this, SLOT(selectionChanged()));

	currentPage = page;
	currentFile->setText(QDir::toNativeSeparators(currentPage->fileName()));
	selectionChanged();
	setCurentCodec();
	updateActions();
}

void DBFRedactorMainWindow::closeTab(int index)
{
	DBFRedactorPage *page = pages.value(tabBar->tabData(index).toString());
	if (!page)
		return;
	delete page;
	currentPage = 0;
	pages.remove(tabBar->tabData(index).toString());
	tabBar->removeTab(index);
	updateActions();
}

void DBFRedactorMainWindow::closeCurrentTab()
{
	closeTab(tabBar->currentIndex());
}

void DBFRedactorMainWindow::updateActions()
{
	actionClose->setEnabled(currentPage);
	currentFile->setVisible(currentPage);
	informationLabel->setVisible(currentPage && functionComboBox->currentIndex() >= 0);
	acionExportToHtml->setEnabled(currentPage);
	acionExportToXml->setEnabled(currentPage);
	acionExportToCsv->setEnabled(currentPage);
	functionComboBox->setVisible(currentPage);
	actionSetEditMode->setEnabled(currentPage);
	actionSave->setEnabled(currentPage);
	actionAddRecord->setEnabled(actionSetEditMode->isChecked());
	actionRemoveRecord->setEnabled(actionSetEditMode->isChecked());
	actionRecoverRecord->setEnabled(actionSetEditMode->isChecked());

	view->setVisible(currentPage);

	codecsMenu->setEnabled(currentPage);

	actionCopy->setEnabled(currentPage);

	if (currentPage) {
		actionSetEditMode->setChecked(!currentPage->dbfModel()->isReadOnly());
		actionSave->setEnabled(currentPage->redactor()->modified());
		actionCopy->setEnabled(currentPage->model()->rowCount() > 0);
	}
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
	updateActions();
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
	return view->selectionModel()->selection().indexes().size();
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
		}
		value = index.data(Qt::DisplayRole).toDouble(&ok);
		if (ok && min > value)
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
		}
		value = index.data(Qt::DisplayRole).toDouble(&ok);
		if (ok && max < value)
			max = value;
	}
	delete progressBar;
	progressBar = 0;

	return max;
}

void DBFRedactorMainWindow::copyToClipboard()
{
	const QModelIndexList& indexes = view->selectionModel()->selection().indexes();

	progressBar = new QProgressBar(this);
	progressBar->setFormat(tr("Coping to clipboard. %p% to finish."));
	progressBar->setRange(0, indexes.size());
	progressBar->setValue(0);
	statusBar()->addWidget(progressBar, 1);

	QMap<int, QString> text;
	int i = 0;
	foreach(const QModelIndex& index, indexes) {
		i++;
		if (i / ProcessEventsPeriod * ProcessEventsPeriod == i) {
			progressBar->setValue(i);
			QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
		}

		if (!text.contains(index.row()))
			text.insert(index.row(), QString());
		if (!text.value(index.row()).isEmpty())
			text[index.row()] += "\t";
		switch(currentPage->redactor()->field(index.column()).type) {
			case DBFRedactor::TYPE_DATE:
				text[index.row()] += index.data(Qt::DisplayRole).toDate().toString(Qt::SystemLocaleShortDate);
				break;
			case DBFRedactor::TYPE_CHAR:
				text[index.row()] += "\"" + index.data(Qt::DisplayRole).toString() + "\"";
				break;
			default:
				text[index.row()] += index.data(Qt::DisplayRole).toString();
		}
	}
	QApplication::clipboard()->setText(QStringList(text.values()).join("\n"));
	delete progressBar;
	progressBar = 0;
}

QStringList DBFRedactorMainWindow::prepareHtml()
{
	QStringList html;
	{
		QStringList tempList;
		tempList << "<HTML>\n<HEAD><meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\" /></HEAD>\n<BODY>";
		tempList << "<TABLE BORDER=1 BORDERCOLOR=\"#000000\" CELLPADDING=4  CELLSPACING=0 WIDTH=\"100%\" ALIGN=LEFT>";

		tempList << "<COLGROUP>";
		for (int i = 0; i < view->model()->columnCount(); i++)
			tempList << "<COL>";

		tempList << "<TBODY>";
		tempList << "<TR VALIGN=TOP>";
		for (int i = 0; i < view->model()->columnCount(); i++)
			tempList << "<TD ALIGN=CENTER>" + view->model()->headerData(i, Qt::Horizontal, Qt::EditRole).toString() + "</TD>";
		tempList << "</TR>";

		html << tempList.join("\n");
	}

	progressBar = new QProgressBar(this);
	progressBar->setFormat(tr("Preparing. %p% to finish."));
	progressBar->setRange(0, view->model()->rowCount());
	progressBar->setValue(0);
	statusBar()->addWidget(progressBar, 1);

	for (int i = 0; i < view->model()->rowCount(); i++) {
		if (view->isRowHidden(i))
			continue;

		if (i / ProcessEventsPeriod * ProcessEventsPeriod == i) {
			progressBar->setValue(i);
			QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
		}

		QString tempStrting = "<TR ALIGN=LEFT>";
		for (int j = 0; j < view->model()->columnCount(); j++) {
			const QVariant& value = view->model()->index(i, j).data(Qt::DisplayRole);
			QString stringValue;

			switch(currentPage->redactor()->field(view->model()->index(i, j).column()).type) {
				case DBFRedactor::TYPE_DATE:
					stringValue = value.toDate().toString(Qt::SystemLocaleShortDate);
					break;
				case DBFRedactor::TYPE_LOGICAL:
					stringValue = value.toBool() ? tr("Yes") : tr("No");
					break;
				default:
					stringValue = value.toString();
			}
			if (stringValue.isEmpty())
				stringValue = "<BR>";
			tempStrting += "\n<TD><P>" + stringValue + "</P></TD>";
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
	settings.beginGroup("Global");
	const QString& fileName = QFileDialog::getSaveFileName(this,
														   tr("Save"),
														   settings.value("ExportPath", "'").toString() + "/" + currentPage->redactor()->tableName(),
														   tr("HTML files (*.html)"));
	if (fileName.isEmpty())
		return;

	settings.setValue("ExportPath", QFileInfo(fileName).absolutePath());

	const QStringList& l(prepareHtml());

	progressBar = new QProgressBar(this);
	progressBar->setFormat(tr("Saving. %p% to finish."));
	progressBar->setRange(0, l.size());
	progressBar->setValue(0);
	statusBar()->addWidget(progressBar, 1);

	QFile file(fileName);
	if (!file.open(QIODevice::WriteOnly)) {
		QMessageBox::critical(this, "", tr("Error save to file.\n%1").arg(file.errorString()));
		return;
	}

	QTextStream stream(&file);
	stream.setCodec("UTF-8");

	for (int i = 0, size = l.size(); i < size; i++) {
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
	if (ev->type() == QEvent::LanguageChange) {
		retranslateStrings();
	}

	if (ev->type() == QEvent::DragEnter) {
		QDragEnterEvent *dragEvent = static_cast<QDragEnterEvent*> (ev);

		if (dragEvent->mimeData()->hasFormat("text/uri-list")) {
			dragEvent->acceptProposedAction();
		}
	}

	if (ev->type() == QEvent::Drop) {
		QDropEvent *dropEvent = static_cast<QDragEnterEvent*> (ev);

		QStringList fileNames;
		foreach (const QUrl& url, dropEvent->mimeData()->urls()) {
			fileNames << url.toLocalFile();
		}
		openFiles(fileNames);
	}

	if (ev->type() == QEvent::Close) {
		QSettings settings;
		if (settings.value("Global/MoveToTrayWhenClose", true).toBool()) {
			hide ();
			trayIcon->show();
			actionShow->setEnabled(true);
			actionHide->setEnabled(false);
			ev->ignore();
			return true;
		}
	}

	if (ev->type() == QEvent::Show) {
		QSettings settings;
		if (!settings.value("Global/TrayAlwaysShow", true).toBool())
			trayIcon->hide();
		actionShow->setEnabled(false);
		actionHide->setEnabled(true);
	}

	return QMainWindow::event(ev);
}

void DBFRedactorMainWindow::exportToXml()
{
	QSettings settings;
	settings.beginGroup("Global");
	const QString& fileName = QFileDialog::getSaveFileName(this,
														   tr("Save"),
														   settings.value("ExportPath", "'").toString() + "/" + currentPage->redactor()->tableName(),
														   tr("XML files (*.xml)"));
	if (fileName.isEmpty())
		return;

	settings.setValue("ExportPath", QFileInfo(fileName).absolutePath());

	progressBar = new QProgressBar(this);
	progressBar->setFormat(tr("Saving. %p% to finish."));
	progressBar->setRange(0, view->model()->rowCount());
	progressBar->setValue(0);
	statusBar()->addWidget(progressBar, 1);

	QFile file(fileName);
	if (!file.open(QIODevice::WriteOnly)) {
		QMessageBox::critical(this, "", tr("Error save to file.\n%1").arg(file.errorString()));
		return;
	}

	QXmlStreamWriter stream(&file);
	stream.setAutoFormatting(true);
	stream.writeStartDocument();
	stream.writeStartElement("ROWDATA");

	for (int i = 0, rowCount = view->model()->rowCount(); i < rowCount; i++) {
		if (view->isRowHidden(i))
			continue;

		stream.writeStartElement("ROW");

		if (i / ProcessEventsPeriod * ProcessEventsPeriod == i) {
			progressBar->setValue(i);
			QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
		}
		for (int j = 0, columnCount = view->model()->columnCount(); j < columnCount; j++) {
			const QVariant& value = view->model()->index(i, j).data(Qt::DisplayRole);
			stream.writeTextElement(view->model()->headerData(j, Qt::Horizontal, Qt::EditRole).toString(),
									value.toString());
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
	settings.beginGroup("Global");
	const QString& fileName = QFileDialog::getSaveFileName(this,
														   tr("Save"),
														   settings.value("ExportPath", "'").toString() + "/" + currentPage->redactor()->tableName(),
														   tr("CSV files (*.csv)"));
	if (fileName.isEmpty())
		return;

	settings.setValue("ExportPath", QFileInfo(fileName).absolutePath());

	progressBar = new QProgressBar(this);
	progressBar->setFormat(tr("Saving. %p% to finish."));
	progressBar->setRange(0, view->model()->rowCount());
	progressBar->setValue(0);
	statusBar()->addWidget(progressBar, 1);

	QFile file(fileName);
	if (!file.open(QIODevice::WriteOnly)) {
		QMessageBox::critical(this, "", tr("Error save to file.\n%1").arg(file.errorString()));
		return;
	}

	QTextStream stream(&file);
	stream.setCodec("UTF-8");

	QStringList tempStringList;
	for (int j = 0, columnCount = view->model()->columnCount(); j < columnCount; j++)
		tempStringList << "\"" + view->model()->headerData(j, Qt::Horizontal, Qt::EditRole).toString() + "\"";

	stream << tempStringList.join("|") << endl;

	for (int i = 0, rowCount = view->model()->rowCount(); i < rowCount; i++) {
		if (view->isRowHidden(i))
			continue;

		if (i / ProcessEventsPeriod * ProcessEventsPeriod == i) {
			progressBar->setValue(i);
			QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
		}
		tempStringList.clear();
		for (int j = 0, columnCount = view->model()->columnCount(); j < columnCount; j++) {
			const QVariant& value = view->model()->index(i, j).data(Qt::DisplayRole);

			switch (currentPage->redactor()->field(j).type) {
				case DBFRedactor::TYPE_CHAR:
					tempStringList << "\"" + value.toString() + "\"";
					break;
				case DBFRedactor::TYPE_LOGICAL:
					tempStringList << value.toString();
					break;
				case DBFRedactor::TYPE_DATE:
					tempStringList << value.toDate().toString(Qt::SystemLocaleShortDate);
					break;
				default :
					tempStringList << value.toString();
			}
		}
		stream << tempStringList.join("|") << endl;
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

	for (int i = 0, columnCount = currentPage->model()->columnCount(); i < columnCount; i++)
		captions.insert(i, currentPage->model()->headerData(i, Qt::Horizontal, Qt::EditRole).toString());

	SortDialog dialog(captions, this);
	dialog.setSortedColumns(currentPage->model()->sortedColumns());

	if (dialog.exec())
		currentPage->model()->setSortedColumns(dialog.sortedColumns());
}

void DBFRedactorMainWindow::changeFilter()
{
	QHash<int, QString> captions;

	for (int i = 0, columnCount = currentPage->model()->columnCount(); i < columnCount; i++)
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

void DBFRedactorMainWindow::preferences()
{
	PreferencesDialog d(this);

	connect (&d, SIGNAL(applied()), this, SLOT(reloadSettings()));
	if (d.exec())
		reloadSettings ();
}

void DBFRedactorMainWindow::setEditMode(bool b)
{
	currentPage->dbfModel()->setReadOnly(!b);
	updateActions();
}

QStringList DBFRedactorMainWindow::codecsList()
{
	QStringList codecs;
	foreach(int mib, QTextCodec::availableMibs())
		codecs << QTextCodec::codecForMib(mib)->name();
	codecs.sort();
	return codecs;
}

void DBFRedactorMainWindow::createCodecsMenu()
{
	QActionGroup *codecsGroup = new QActionGroup(this);

	QAction *codecAction;

	foreach(const QString& codecName, codecsList()) {
		codecAction = new QAction(codecName, this);
		codecAction->setCheckable(true);
		connect(codecAction, SIGNAL(triggered()), this, SLOT(setPageCodec()));
		codecsGroup->addAction(codecAction);
		codecsMenu->addAction(codecAction);
	}
}

void DBFRedactorMainWindow::setCurentCodec()
{
	const QString& codecName = currentPage->redactor()->textCodec()->name();

	foreach(QAction *action, codecsMenu->actions()) {
		if (action->text() == codecName) {
			action->setChecked(true);
			break;
		}
	}
}

void DBFRedactorMainWindow::setPageCodec()
{
	QAction *action = qobject_cast<QAction*> (sender());
	if (!action)
		return;

	currentPage->redactor()->setTextCodec(QTextCodec::codecForName(action->text().toAscii()));
}

void DBFRedactorMainWindow::addRecord()
{
	currentPage->dbfModel()->addRecord();
}

void DBFRedactorMainWindow::removeRecord()
{
	QList<int> rows;
	foreach(const QModelIndex& index, view->selectionModel()->selectedIndexes()) {
		const int row = currentPage->model()->mapToSource(index).row();
		if (!rows.contains(row))
			rows.append(row);
	}

	int res = QMessageBox::information(this, "", tr("Are you shure remove %1 records?").arg(rows.size()), QMessageBox::Ok | QMessageBox::Cancel);

	if (res = QMessageBox::Ok)
		currentPage->dbfModel()->removeRecords(rows);
}

void DBFRedactorMainWindow::recoverRecord()
{
	QList<int> rows;
	foreach(const QModelIndex& index, view->selectionModel()->selectedIndexes()) {
		const int row = currentPage->model()->mapToSource(index).row();
		if (!rows.contains(row))
			rows.append(row);
	}

	int res = QMessageBox::information(this, "", tr("Are you shure recover %1 records?").arg(rows.size()), QMessageBox::Ok | QMessageBox::Cancel);

	if (res = QMessageBox::Ok)
		currentPage->dbfModel()->recoverRecords(rows);
}

void DBFRedactorMainWindow::about()
{
	AboutDialog d (this);

	d.setAuthor(tr("PanteR"));
	d.setMail("panter.dsd@gmail.com");
	d.setPhone("89094119675");
	d.setLicense("GNU GPL v3");

	{
		QFile file(":/LICENSE.GPL3");
		if (file.open(QFile::ReadOnly)) {
			d.setLicenseText(QString(file.readAll()));
			file.close();
		}
	}
	d.addThanks("Abbapoh", "archangel_rus@mail.ru", tr ("Color and Font buttons"));
	d.exec();
}

void DBFRedactorMainWindow::trayClicked(QSystemTrayIcon::ActivationReason reason)
{
	switch (reason) {
		case QSystemTrayIcon::Trigger:
			if (isHidden()) {
				show();
				activateWindow();
				actionShow->setEnabled(false);
				actionHide->setEnabled(true);
			} else {
				hide();
				actionShow->setEnabled(true);
				actionHide->setEnabled(false);
			}
			break;
		default:
			break;
	}
}

void DBFRedactorMainWindow::updateHideShowActions ()
{
	actionHide->setEnabled(!isHidden());
	actionShow->setEnabled(isHidden());
}

void DBFRedactorMainWindow::addToHistory (const QString& fileName)
{
	const int index = m_history.indexOf(fileName);
	if (index >= 0) {
		m_history.removeAt(index);
	}

	m_history.insert(0, fileName);

	while (m_history.size() > 30)
		m_history.removeLast();

	updateHistoryMenu ();
}

void DBFRedactorMainWindow::updateHistoryMenu ()
{
	qDeleteAll (historyMenu->actions());
	historyMenu->clear();

	QAction *action;
	foreach (const QString& fileName, m_history) {
		action = new QAction (this);
		action->setText(QDir::toNativeSeparators(fileName));
		action->setObjectName(fileName);
		connect (action, SIGNAL(triggered()), this, SLOT(openHistory()));
		historyMenu->addAction(action);
	}
}

void DBFRedactorMainWindow::openHistory ()
{
	QAction *action = qobject_cast <QAction*> (sender ());

	if (!action)
		return;

	openFiles(QStringList(action->objectName()));
}
void DBFRedactorMainWindow::preview()
{
#ifndef QT_NO_PRINTER
	QPrinter printer(QPrinter::HighResolution);
	printer.setPageMargins(20, 20, 20, 20, QPrinter::Millimeter);
	QPrintPreviewDialog preview(&printer, this);
	connect(&preview, SIGNAL(paintRequested(QPrinter*)), SLOT(printPreview(QPrinter*)));
	preview.exec();
#endif
}

void DBFRedactorMainWindow::printPreview(QPrinter *printer)
{
#ifdef QT_NO_PRINTER
	Q_UNUSED(printer);
#else
	QTextEdit edit (this);
	edit.setHtml(prepareHtml ().join("\n"));
	edit.print(printer);
#endif
}

