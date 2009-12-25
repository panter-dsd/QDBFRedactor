#ifndef FILTERDIALOG_H
#define FILTERDIALOG_H

class QGroupBox;
class QTableView;
class QAction;
class QToolButton;
class QStandardItemModel;

#include <QtCore/QHash>
#include <QtGui/QDialog>

#include "dbfredactorsortfilterproxymodel.h"

class FilterDialog : public QDialog
{
	Q_OBJECT

private:
	QHash<int, QString> m_captions;
	QList<DBFRedactorSortFilterProxyModel::FilterItem> m_filter;

private:
	QTableView *filterView;
	QStandardItemModel *model;

	QAction *actionAdd;
	QAction *actionRemove;

	QToolButton *addButton;
	QToolButton *removeButton;

public:
	FilterDialog(QHash<int, QString> captions, QWidget *parent = 0, Qt::WindowFlags f = Qt::WindowSystemMenuHint);
	~FilterDialog()
	{saveSettings();}

	QList<DBFRedactorSortFilterProxyModel::FilterItem> filter() const;
	void setFilter(QList<DBFRedactorSortFilterProxyModel::FilterItem> filter);

protected:
	bool event(QEvent *ev);

private:
	void loadSettings();
	void saveSettings();
	void retranslateStrings();

private Q_SLOTS:
	void add();
	void remove();
};

#endif //SORTDIALOG_H
