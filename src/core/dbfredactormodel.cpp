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

#include "dbfredactormodel.h"

DBFRedactorModel::DBFRedactorModel(const QString& fileName, QObject *parent)
	:QAbstractItemModel(parent), m_fileName(fileName), m_showDeleted(true)
{
	redactor = new DBFRedactor(fileName);
	redactor->open(DBFRedactor::Read);
	redactor->setBuffering(true);
}

DBFRedactorModel::~DBFRedactorModel()
{
	redactor->close();
	delete redactor;
}

QVariant DBFRedactorModel::data(const QModelIndex &index, int role) const
{
	if (!index.isValid())
		return QVariant();

	const QVariant& value = redactor->data(index.row(), index.column());

	switch (role) {
		case Qt::DisplayRole: case Qt::EditRole:
			return value;
			break;
		case Qt::CheckStateRole:
			if (redactor->field(index.column()).type == DBFRedactor::TYPE_LOGICAL)
				return value.toBool() ? Qt::Checked : Qt::Unchecked;
			break;
		case Qt::UserRole:
			return redactor->isDeleted(index.row());
		}

	return QVariant();
}

bool DBFRedactorModel::setData ( const QModelIndex & index, const QVariant & value, int role)
{
	if (!index.isValid())
		return false;

	return redactor->setData(index.row(), index.column(), value);
}

Qt::ItemFlags DBFRedactorModel::flags(const QModelIndex &index) const
{
	Qt::ItemFlags flags = index.isValid() ? Qt::ItemIsSelectable | Qt::ItemIsEnabled : Qt::NoItemFlags;
	if (redactor->field(index.column()).type == DBFRedactor::TYPE_LOGICAL)
		flags |= Qt::ItemIsUserCheckable;

	if (!isReadOnly())
		flags |= Qt::ItemIsEditable;
	return flags;
}

QVariant DBFRedactorModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if(role == Qt::DisplayRole || role == Qt::EditRole) {
		if(orientation == Qt::Horizontal) {
			return redactor->field(section).name;
		} else {
			return section + 1;
		}
	}
	if(role == Qt::TextAlignmentRole) {
		return Qt::AlignCenter;
	}

	if(role == Qt::TextColorRole) {
		if(orientation == Qt::Vertical) {
			if (redactor->isChanged(section))
				return Qt::red;
		}
	}

	return QVariant();
}

QModelIndex DBFRedactorModel::index(int row, int column, const QModelIndex &parent) const
{
	if (!parent.isValid())
		return createIndex(row, column);
}

QModelIndex DBFRedactorModel::parent(const QModelIndex &/*index*/) const
{
	return QModelIndex();
}

int DBFRedactorModel::rowCount(const QModelIndex &/*parent*/) const
{
	return redactor->rowsCount();
}

int DBFRedactorModel::columnCount(const QModelIndex &/*parent*/) const
{
	return redactor->columnsCount();
}

void DBFRedactorModel::refresh()
{
	redactor->refresh();
	reset();
}

void DBFRedactorModel::setReadOnly(bool b)
{
	DBFRedactor::DBFOpenMode mode = b ? DBFRedactor::Read : DBFRedactor::Write;
	redactor->setOpenMode(mode);
	reset();
}
