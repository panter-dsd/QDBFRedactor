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

#ifndef DBFREDACTORMODEL_H
#define DBFREDACTORMODEL_H

#include <QtCore/QAbstractItemModel>
#include <QtCore/QMap>

#include "dbfredactor.h"

class DBFRedactorModel : public QAbstractItemModel
{
	Q_OBJECT

private:
	QString m_fileName;
	bool m_showDeleted;
	DBFRedactor *redactor;

public:
	DBFRedactorModel(const QString& fileName, QObject * parent = 0);
	~DBFRedactorModel();

	QVariant data(const QModelIndex &index, int role) const;
	bool setData ( const QModelIndex & index, const QVariant & value, int role = Qt::EditRole );
	Qt::ItemFlags flags(const QModelIndex &index) const;
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
	QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
	QModelIndex parent(const QModelIndex &index) const;

	bool addRecord ()
	{
		beginInsertRows(QModelIndex(), rowCount(), rowCount());
		redactor->addRecord();
		endInsertRows();
		return true;
	}

	bool removeRecord(int row)
	{
		return removeRecords(QList<int> () << row);
	}

	bool removeRecords (const QList<int> &rows)
	{
		foreach(int row, rows)
			redactor->removeRecord(row);
		return true;
	}

	bool recoverRecord(int row)
	{
		return recoverRecords(QList<int> () << row);
	}

	bool recoverRecords (const QList<int> &rows)
	{
		foreach(int row, rows)
			redactor->recoverRecord(row);
		return true;
	}

	int rowCount(const QModelIndex &parent = QModelIndex()) const;
	int columnCount(const QModelIndex &parent = QModelIndex()) const;

	bool showDeleted()
	{return m_showDeleted;}
	void setShowDeleted(bool isShow)
	{m_showDeleted = isShow;}

	void refresh();

	DBFRedactor* dbfRedactor()
	{return redactor;}

	bool isReadOnly() const
	{return redactor->openMode() & DBFRedactor::Read;}
	void setReadOnly(bool b);

};

#endif //DBFREDACTORMODEL_H
