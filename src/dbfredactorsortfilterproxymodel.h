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

#ifndef DBFREDACTORSORTFILTERPROXYMODEL_H
#define DBFREDACTORSORTFILTERPROXYMODEL_H

#include <QtGui/QSortFilterProxyModel>

class DBFRedactorSortFilterProxyModel : public QSortFilterProxyModel
{
	Q_OBJECT

public:

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
		FilterOperator filterOperator;
		int column;
		FilterUslovie uslovie;
		QRegExp regExp;
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

	QList<QPair<int, Qt::SortOrder> > sortedColumns() const
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

	QList<FilterItem> filter() const
	{return m_filter;}
	void addFilter(const FilterItem& filterItem)
	{
		m_filter.append(filterItem);
		invalidate();
	}
	void setFilter(QList<FilterItem> filter)
	{
		m_filter = filter;
		invalidate();
	}

protected:
	void sort();
	bool lessThan ( const QModelIndex & left, const QModelIndex & right ) const;
	bool filterAcceptsRow ( int source_row, const QModelIndex & source_parent ) const;

private:
	inline int naturalCompare(const QString& left, const QString& right, Qt::CaseSensitivity cs = Qt::CaseSensitive) const;

public Q_SLOTS:
	void clearSort();
	void removeFilter()
	{
		m_filter.clear();
		invalidate();
	}
};

#endif //DBFREDACTORSORTFILTERPROXYMODEL_H
