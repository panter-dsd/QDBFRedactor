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

#ifndef DBFREDACTORMAINWINDOW_H
#define DBFREDACTORMAINWINDOW_H

class QTableView;
class QTabBar;
class QAction;
class DBFRedactorPage;
class QLabel;
class QItemSelection;
class QProgressBar;
class QComboBox;

#include <QtCore/QMap>

#include <QtGui/QMainWindow>

class DBFRedactorMainWindow : public QMainWindow
{
	Q_OBJECT

private:
	QTableView *view;
	QTabBar *tabBar;
	QMap<QString, DBFRedactorPage*> pages;
	DBFRedactorPage *currentPage;
	QLabel *currentFile;
	QLabel *informationLabel;
	QProgressBar *progressBar;
	QComboBox *functionComboBox;

	QMenu *fileMenu;
	QMenu *exportMenu;
	QMenu *codecsMenu;
	QMenu *helpMenu;

	QToolBar *fileToolBar;

	QAction *actionOpen;
	QAction *actionSave;
	QAction *actionExit;
	QAction *actionClose;
	QAction *actionRefresh;
	QAction *actionCopy;
	QAction *acionExportToHtml;
	QAction *acionExportToXml;
	QAction *acionExportToCsv;
	QAction *actionResizeColumnsToContents;
	QAction *actionUnsort;
	QAction *actionChangeSort;
	QAction *actionPreferences;
	QAction *actionSetEditMode;
	QAction *actionAddRecord;
	QAction *actionRemoveRecord;
	QAction *actionRecoverRecord;
	QAction *actionAbout;
	QAction *actionAboutQt;

	QAction *actionChangeFilter;
	QAction *actionAddIncludeFilter;
	QAction *actionAddExcludeFilter;
	QAction *actionRemoveFilter;

public:
	DBFRedactorMainWindow(QWidget* parent = 0, Qt::WFlags f = 0);
	~DBFRedactorMainWindow();
	void openFiles(const QStringList& fileList);

private:
	void loadSettings();
	void saveSettings();
	void updateActions();
	QStringList prepareHtml();
	double sum();
	double count();
	double min();
	double max();
	void retranslateStrings();
	QStringList codecsList();
	void createCodecsMenu();
	void setCurentCodec();

protected:
	bool event(QEvent *ev);

private Q_SLOTS:
	void open();
	void save();
	void tabChanged(int index);
	void closeTab(int index);
	void closeCurrentTab();
	void refreshModel();
	void selectionChanged();
	void copyToClipboard();
	void exportToHtml();
	void exportToXml();
	void exportToCsv();
	void unsort();
	void sort(int section);
	void changeSort();
	void changeFilter();
	void addIncludeFilter();
	void addExcludeFilter();
	void removeFilter();
	void preferences();
	void setEditMode(bool b);
	void setPageCodec();
	void addRecord();
	void removeRecord();
	void recoverRecord();
	void about();

public Q_SLOTS:
	void handleMessage(const QString& message)
	{ openFiles(message.split("\n"));}
};

#endif // DBFREDACTORMAINWINDOW_H
