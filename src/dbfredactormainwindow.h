#ifndef DBFREDACTORMAINWINDOW_H
#define DBFREDACTORMAINWINDOW_H

class QTableView;
class QTabBar;
class QAction;
class DBFRedactorModel;

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

	QAction *actionOpen;

public:
	DBFRedactorMainWindow(QWidget* parent = 0, Qt::WFlags f = 0);
	~DBFRedactorMainWindow();

private:
	void openFiles(const QStringList& fileList);

protected:

private Q_SLOTS:
	void open();
	void tabChanged(int index);
};

#endif // DBFREDACTORMAINWINDOW_H
