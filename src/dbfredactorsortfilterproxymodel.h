#ifndef DBFREDACTORSORTFILTERPROXYMODEL_H
#define DBFREDACTORSORTFILTERPROXYMODEL_H

#include <QtGui/QSortFilterProxyModel>

class DBFRedactorSortFilterProxyModel : public QSortFilterProxyModel
{
	Q_OBJECT

private:
	QList<QPair<int, Qt::SortOrder> > m_sortedColumns;
	Qt::SortOrder m_sortOrder;

public:
	DBFRedactorSortFilterProxyModel(QObject *parent = 0);
	virtual ~DBFRedactorSortFilterProxyModel()
	{}

	QVariant headerData ( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;

	QList<QPair<int, Qt::SortOrder> > sortedColumns()
	{return m_sortedColumns;}
	void setSortedColumns(QList<QPair<int, Qt::SortOrder> > sortedColumns)
	{
		m_sortedColumns = sortedColumns;
		sort();
	}

	void addSortedColumn(int column, Qt::SortOrder order);
	void removeSortedColumn(int column);
	void changeSortedColumn(int column, Qt::SortOrder order);

	Qt::SortOrder sortOrder() const
	{return m_sortOrder;}
	Qt::SortOrder sortOrder(int column) const;

	bool isColumnInSort(int column) const;

protected:
	void sort();
	bool lessThan ( const QModelIndex & left, const QModelIndex & right ) const;

public Q_SLOTS:
	void clearSort();
};

#endif //DBFREDACTORSORTFILTERPROXYMODEL_H
