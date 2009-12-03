#ifndef DBFREDACTORMAINWINDOW_H
#define DBFREDACTORMAINWINDOW_H

class QTableView;
class QTabBar;
class QAction;

#include <QtGui/QMainWindow>

#include "dbfredactormodel.h"

class DBFRedactorMainWindow : public QMainWindow
{
	Q_OBJECT

private:
	QTableView *view;
	QTabBar *tabBar;

	QAction *actionOpen;

public:
	DBFRedactorMainWindow(QWidget* parent = 0, Qt::WFlags f = 0);
	~DBFRedactorMainWindow();

private:

protected:

private Q_SLOTS:
	void open();
};

#endif // DBFREDACTORMAINWINDOW_H
