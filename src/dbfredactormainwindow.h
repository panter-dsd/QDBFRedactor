#ifndef DBFREDACTORMAINWINDOW_H
#define DBFREDACTORMAINWINDOW_H

class QTableView;
class QTabBar;
class QAction;
class DBFRedactorPage;
class QLabel;
class QItemSelection;

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
	QLabel *sumLabel;

	QAction *actionOpen;
	QAction *actionExit;
	QAction *actionClose;
	QAction *actionRefresh;
	QAction *actionCopy;

public:
	DBFRedactorMainWindow(QWidget* parent = 0, Qt::WFlags f = 0);
	~DBFRedactorMainWindow();
	void openFiles(const QStringList& fileList);

private:
	void loadSettings();
	void saveSettings();
	void updateActions();
	QStringList prepareHtml();

protected:

private Q_SLOTS:
	void open();
	void tabChanged(int index);
	void closeTab(int index);
	void closeCurrentTab();
	void refreshModel();
	void selectionChanged();
	void copyToClipboard();
};

#endif // DBFREDACTORMAINWINDOW_H
