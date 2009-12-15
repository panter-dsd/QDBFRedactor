#include <QtCore/QDebug>
#include <QtCore/QDateTime>

#include "dbfredactorsortfilterproxymodel.h"

DBFRedactorSortFilterProxyModel::DBFRedactorSortFilterProxyModel(QObject *parent)
	:QSortFilterProxyModel(parent), m_sortOrder(Qt::AscendingOrder)
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
	int column = m_sortedColumns.isEmpty() ? -1 : 0;
	QSortFilterProxyModel::sort(column, Qt::AscendingOrder);
}

bool DBFRedactorSortFilterProxyModel::lessThan ( const QModelIndex & left, const QModelIndex & right ) const
{
	for (int i = 0; i < m_sortedColumns.size(); i++) {
		QVariant leftData = sourceModel()->index(left.row(), m_sortedColumns.at(i).first).data(Qt::DisplayRole);
		QVariant rightData = sourceModel()->index(right.row(), m_sortedColumns.at(i).first).data(Qt::DisplayRole);
		Qt::SortOrder order = m_sortedColumns.at(i).second;

		if (leftData == rightData)
			continue;

		if (leftData.type() == QVariant::DateTime)
			return (leftData.toDateTime() < rightData.toDateTime()) ^ order;

		if (leftData.type() == QVariant::Double)
			return (leftData.toDouble() < rightData.toDouble()) ^ order;

		return (QString::localeAwareCompare(leftData.toString(), rightData.toString()) < 0) ^ order;
	}
	return false;
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
