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
				return (naturalCompare(leftData.toString(), rightData.toString()) < 0) ^ order;
				break;
		}
	}
	return false;
}

static inline QChar getNextChar(const QString &s, int location)
{
	return (location < s.length()) ? s.at(location) : QChar();
}

inline int DBFRedactorSortFilterProxyModel::naturalCompare(const QString& left, const QString& right, Qt::CaseSensitivity cs) const
{
	for(int l1 = 0, l2 = 0; l1 <= left.count() && l2 <= right.count(); ++l1, ++l2)
	{
		// skip spaces, tabs and 0's
		QChar c1 = getNextChar(left, l1);
		while(c1.isSpace())
			c1 = getNextChar(left, ++l1);
		QChar c2 = getNextChar(right, l2);
		while(c2.isSpace())
			c2 = getNextChar(right, ++l2);

		if(c1.isDigit() && c2.isDigit())
		{
			while(c1.digitValue() == 0)
				c1 = getNextChar(left, ++l1);
			while(c2.digitValue() == 0)
				c2 = getNextChar(right, ++l2);

			int lookAheadLocation1 = l1;
			int lookAheadLocation2 = l2;
			int currentReturnValue = 0;
			// find the last digit, setting currentReturnValue as we go if it isn't equal
			for(QChar lookAhead1 = c1, lookAhead2 = c2;
				(lookAheadLocation1 <= left.length() && lookAheadLocation2 <= right.length());
				lookAhead1 = getNextChar(left, ++lookAheadLocation1),
				lookAhead2 = getNextChar(right, ++lookAheadLocation2))
			{
				bool ileftADigit = !lookAhead1.isNull() && lookAhead1.isDigit();
				bool irightADigit = !lookAhead2.isNull() && lookAhead2.isDigit();
				if(!ileftADigit && !irightADigit)
					break;
				if(!ileftADigit)
					return -1;
				if(!irightADigit)
					return 1;
				if(currentReturnValue == 0)
				{
					if(lookAhead1 < lookAhead2)
						currentReturnValue = -1;
					else if(lookAhead1 > lookAhead2)
						currentReturnValue = 1;
				}
			}
			if(currentReturnValue != 0)
				return currentReturnValue;
		}

		if(cs == Qt::CaseInsensitive)
		{
			if(!c1.isLower())
				c1 = c1.toLower();
			if(!c2.isLower())
				c2 = c2.toLower();
		}
		int r = QString::localeAwareCompare(c1, c2);
		if(r < 0)
			return -1;
		if(r > 0)
			return 1;
	}
	// The two strings are the same (02 == 2) so fall back to the normal sort
	return QString::compare(left, right, cs);
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
