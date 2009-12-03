#ifndef DBFREDACTORMAINWINDOW_H
#define DBFREDACTORMAINWINDOW_H

class QTableView;
class QTabBar;
class QAction;
class DBFRedactorModel;
class QLabel;

#include <QtCore/QMap>

#include <QtGui/QMainWindow>

#include "dbfredactormodel.h"

class DBFRedactorMainWindow : public QMainWindow
{
	Q_OBJECT

private:
	QTableView *view;
	QTabBar *tabBar;
	QMap<QString, DBFRedactorModel*> models;
	QLabel *currentFile;

	QAction *actionOpen;
	QAction *actionExit;
	QAction *actionClose;
	QAction *actionRefresh;

public:
	DBFRedactorMainWindow(QWidget* parent = 0, Qt::WFlags f = 0);
	~DBFRedactorMainWindow();
	void openFiles(const QStringList& fileList);

private:
	void loadSettings();
	void saveSettings();
	void updateActions();

protected:

private Q_SLOTS:
	void open();
	void tabChanged(int index);
	void closeTab(int index);
	void closeCurrentTab();
	void refreshModel();
};

#endif // DBFREDACTORMAINWINDOW_H
