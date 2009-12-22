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

void DBFRedactorSortFilterProxyModel::removeSortedColumn(int column)
{
	for (int i = 0; i < m_sortedColumns.size(); i++) {
		if (m_sortedColumns.at(i).first == column) {
			m_sortedColumns.removeAt(i);
			sort();
			break;
		}
	}
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
	sort();
}

void DBFRedactorSortFilterProxyModel::sort()
{
	int column = m_sortedColumns.isEmpty() ? -1 : 0;
	QSortFilterProxyModel::sort(column, Qt::AscendingOrder);
}

bool DBFRedactorSortFilterProxyModel::lessThan ( const QModelIndex & left, const QModelIndex & right ) const
{
	for (int i = 0, size = m_sortedColumns.size(); i < size; i++) {
		const QVariant& leftData = sourceModel()->index(left.row(), m_sortedColumns.at(i).first).data(Qt::DisplayRole);
		const QVariant& rightData = sourceModel()->index(right.row(), m_sortedColumns.at(i).first).data(Qt::DisplayRole);
		const Qt::SortOrder order = m_sortedColumns.at(i).second;

		if (leftData == rightData)
			continue;

		switch (leftData.type()) {
			case QVariant::Bool:
				return (leftData.toBool() < rightData.toBool()) ^ order;
				break;
			case QVariant::Int:
				return (leftData.toInt() < rightData.toInt()) ^ order;
				break;
			case QVariant::UInt:
				return (leftData.toUInt() < rightData.toUInt()) ^ order;
				break;
			case QVariant::LongLong:
				return (leftData.toLongLong() < rightData.toLongLong()) ^ order;
				break;
			case QVariant::ULongLong:
				return (leftData.toULongLong() < rightData.toULongLong()) ^ order;
				break;
			case QVariant::Double:
				return (leftData.toDouble() < rightData.toDouble()) ^ order;
				break;
			case QVariant::Char:
				return (leftData.toChar() < rightData.toChar()) ^ order;
				break;
			case QVariant::Date:
				return (leftData.toDate() < rightData.toDate()) ^ order;
				break;
			case QVariant::Time:
				return (leftData.toTime() < rightData.toTime()) ^ order;
				break;
			case QVariant::DateTime:
				return (leftData.toDateTime() < rightData.toDateTime()) ^ order;
				break;
			case QVariant::String:
				return (QString::localeAwareCompare(leftData.toString(), rightData.toString()) < 0) ^ order;
				break;
		}
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
	for (int i = 0; i < m_sortedColumns.size(); i++)
		if (m_sortedColumns.at(i).first == column)
			return true;

	return false;
}

bool DBFRedactorSortFilterProxyModel::filterAcceptsRow ( int source_row, const QModelIndex & source_parent ) const
{
	bool res = true;
	int i = 0;

	foreach(FilterItem item, m_filter) {
		bool tempRes = true;
		if (i == 0)
			item.filterOperator = AND;
		i++;
		const QVariant& data = sourceModel()->index(source_row, item.column, source_parent).data(Qt::DisplayRole);

		switch (item.uslovie) {
			case Equal:
				tempRes = data.toString().contains(item.regExp) && data.toString().remove(item.regExp).isEmpty();
				break;
			case NotEqual:
				tempRes = !(data.toString().contains(item.regExp) && data.toString().remove(item.regExp).isEmpty());
				break;
			case Smaller:
				tempRes =QString::compare(data.toString(), item.regExp.pattern(), item.regExp.caseSensitivity()) < 0;
				break;
			case SmallerOrEqual:
				tempRes = QString::compare(data.toString(), item.regExp.pattern(), item.regExp.caseSensitivity()) <= 0;
				break;
			case Lager:
				tempRes = QString::compare(data.toString(), item.regExp.pattern(), item.regExp.caseSensitivity()) > 0;
				break;
			case LagerOrEqual:
				tempRes = QString::compare(data.toString(), item.regExp.pattern(), item.regExp.caseSensitivity()) >= 0;
				break;
		}
		switch (item.filterOperator) {
			case AND: res &= tempRes; break;
			case OR: res |= tempRes; break;
		}
	}
	return res;
}
