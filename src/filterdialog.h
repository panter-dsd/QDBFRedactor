#ifndef FILTERDIALOG_H
#define FILTERDIALOG_H

class QGroupBox;
class QListView;
class QAction;
class QToolButton;

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
	QListView *filterView;

	QAction *actionAdd;
	QAction *actionRemove;

	QToolButton *addButton;
	QToolButton *removeButton;

public:
	FilterDialog(QHash<int, QString> captions, QWidget *parent = 0, Qt::WindowFlags f = Qt::WindowSystemMenuHint);
	virtual ~FilterDialog()
	{}

	QList<DBFRedactorSortFilterProxyModel::FilterItem> filter() const
	{return m_filter;}
	void setFilter(QList<DBFRedactorSortFilterProxyModel::FilterItem> filter);

private:

private Q_SLOTS:
	void add();
	void remove();
};

#endif //SORTDIALOG_H
