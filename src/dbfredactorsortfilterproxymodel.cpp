#include <QtCore/QDebug>

#include "dbfredactorsortfilterproxymodel.h"

DBFRedactorSortFilterProxyModel::DBFRedactorSortFilterProxyModel(QObject *parent)
	:QSortFilterProxyModel(parent)
{

}

QVariant DBFRedactorSortFilterProxyModel::headerData ( int section, Qt::Orientation orientation, int role) const
{
	if(role == Qt::DisplayRole) {
		if(orientation == Qt::Horizontal) {
			for (int i = 0; i < m_sortedColumns.size(); i++) {
				if (m_sortedColumns.at(i).first == section) {
					QString sortIndicator = m_sortedColumns.at(i).second == Qt::AscendingOrder ? QString(0x2193) : QString(0x2191);
					if (i > 0)
						sortIndicator += QString::number(i + 1);
					return sortIndicator + " " + sourceModel()->headerData(section, orientation, role).toString();
				}
			}
		}
	}
	return sourceModel()->headerData(section, orientation, role);
}

void DBFRedactorSortFilterProxyModel::addSortedColumn(int column, Qt::SortOrder order)
{
	m_sortOrder = order;
	QPair<int, Qt::SortOrder> pair;
	pair.first = column;
	pair.second = order;
	m_sortedColumns << pair;
	sort();
}

void DBFRedactorSortFilterProxyModel::changeSortedColumn(int column, Qt::SortOrder order)
{
	for (int i = 0; i < m_sortedColumns.size(); i++) {
		if (m_sortedColumns.at(i).first == column) {
			m_sortOrder = order;
			m_sortedColumns[i].second = order;
			sort();
			break;
		}
	}
}

void DBFRedactorSortFilterProxyModel::clearSort()
{
	m_sortedColumns.clear();
	reset();
	sort();
}

void DBFRedactorSortFilterProxyModel::sort()
{
	qDebug() << m_sortedColumns;
}

Qt::SortOrder DBFRedactorSortFilterProxyModel::sortOrder(int column) const
{
	for (int i = 0; i < m_sortedColumns.size(); i++) {
		if (m_sortedColumns.at(i).first == column) {
			return m_sortedColumns.at(i).second;
		}
	}
}

bool DBFRedactorSortFilterProxyModel::isColumnInSort(int column) const
{
	for (int i = 0; i < m_sortedColumns.size(); i++) {
		if (m_sortedColumns.at(i).first == column) {
			return true;
		}
	}
	return false;
}
