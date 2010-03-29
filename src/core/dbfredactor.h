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

#ifndef DBFREDACTOR_H
#define DBFREDACTOR_H

#include <QtCore/QStringList>
#include <QtCore/QDate>
#include <QtCore/QList>
#include <QtCore/QFile>
#include <QtCore/QTextCodec>
#include <QtCore/QFileInfo>
#include <QtCore/QVariant>
#include <QtCore/QQueue>

class DBFRedactor
{
public:
	enum FieldType{
		TYPE_CHAR = 0,
		TYPE_NUMERIC,
		TYPE_LOGICAL,
		TYPE_MEMO,
		TYPE_DATE,
		TYPE_FLOAT,
		TYPE_P};

	enum FileType{
		Fox3,
		Fox3M,
		Fox4,
		Fox,
		DBase3,
		DBase4,
		DBase5};

	struct Field
	{
		QString name;
		quint8 type;
		quint16 pos;
		quint8 firstLenght;
		quint8 secondLenght;
	};

	struct Header
	{
		quint8 fileType;
		QDate lastUpdated;
		qint32 recordsCount;
		qint16 firstRecordPos;
		qint16 recordLenght;
		bool isIndex;
		QList<Field> fieldsList;
	};

	struct Record
	{
		bool isDeleted;
		QList<QVariant> value;
	};

	enum DBFOpenModeFlag {
		No,
		Read,
		Write
	};
	Q_DECLARE_FLAGS(DBFOpenMode, DBFOpenModeFlag);

private:
	Header header;
	QString m_fileName;
	QFile m_file;
	QByteArray m_buf;
	QHash<int, QByteArray> m_cache;
	QList<QPair<int, QByteArray> > m_changedData;
	QTextCodec	 *m_codec;
	QString m_tableName;
	int lastRecord;
	DBFOpenMode m_openMode;
	bool m_buffering;
	bool m_modified;

public:
	DBFRedactor();
	DBFRedactor(const QString& fileName);
	~DBFRedactor();

	bool open(DBFOpenMode OpenMode, const QString& fileName);
	bool open(DBFOpenMode OpenMode);
	void close();

	QList<DBFRedactor::Field> fields () const
	{ return header.fieldsList;}

	DBFRedactor::Field field(int number) const;

	QByteArray strRecord(int row);
	DBFRedactor::Record record(int number);

	int columnsCount() const
	{return header.fieldsList.count();}

	int rowsCount() const;

	QString tableName() const
	{return m_tableName;}

	static bool compareRecord(const Record& first, const Record& second);
	bool isOpen() const;
	int deletedCount();

	void refresh();

	bool buffering() const
	{return m_buffering;}
	void setBuffering(bool b)
	{
		m_buffering = b;
		if (!b)
			m_cache.clear();
	}

	QVariant data(int row, int column);
	bool setData(int row, int column, const QVariant& data);

	bool isDeleted(int row);

	DBFOpenMode openMode()
	{return m_openMode;}
	void setOpenMode(DBFOpenMode openMode);

	QTextCodec* textCodec()
	{return m_codec;}
	void setTextCodec(QTextCodec *textCodec)
	{
		if (m_codec->name() != textCodec->name())
			m_codec = textCodec;
	}

	void addRecord();

	void removeRecord(int row);

	void recoverRecord(int row);

	bool isChanged(int row);

	bool save();

	bool modified() const
	{return m_modified;}

private:
	void writeHeader();
};
#endif
