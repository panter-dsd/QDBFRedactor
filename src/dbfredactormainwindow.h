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

	QAction *actionOpen;
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
	QAction *actionChangeFilter;

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

protected:
	bool event(QEvent *ev);

private Q_SLOTS:
	void open();
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
};

#endif // DBFREDACTORMAINWINDOW_H
