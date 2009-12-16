#ifndef DBFREDACTORSORTFILTERPROXYMODEL_H
#define DBFREDACTORSORTFILTERPROXYMODEL_H

#include <QtGui/QSortFilterProxyModel>

class DBFRedactorSortFilterProxyModel : public QSortFilterProxyModel
{
	Q_OBJECT

public:

	enum FilterType {
		FixedString = 0,
		Wildcard,
		RegExp
	};

	enum FilterOperator {
		AND,
		OR
	};

	enum FilterUslovie {
		Equal = 0,
		NotEqual,
		Smaller,
		SmallerOrEqual,
		Lager,
		LagerOrEqual
	};

	struct FilterItem {
		FilterOperator m_operator;
		int column;
		FilterUslovie uslovie;
		QVariant value;
		FilterType type;
		Qt::CaseSensitivity caseSensitivity;
	};

private:
	QList<QPair<int, Qt::SortOrder> > m_sortedColumns;
	Qt::SortOrder m_sortOrder;
	QList<FilterItem> m_filter;

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

	QList<FilterItem> filter()
	{return m_filter;}
	void addFilter(const FilterItem& filterItem)
	{
		m_filter.append(filterItem);
		invalidate();
	}

protected:
	void sort();
	bool lessThan ( const QModelIndex & left, const QModelIndex & right ) const;
	bool filterAcceptsRow ( int source_row, const QModelIndex & source_parent ) const;

private:
	inline int fixedStringCompare(const QVariant& left, const QVariant& right, Qt::CaseSensitivity sensitivity) const;

public Q_SLOTS:
	void clearSort();
	void removeFilter()
	{
		m_filter.clear();
		invalidate();
	}
};

#endif //DBFREDACTORSORTFILTERPROXYMODEL_H
