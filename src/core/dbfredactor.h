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
#include <QtCore/QVector>
#include <QtCore/QFile>
#include <QtCore/QTextCodec>
#include <QtCore/QFileInfo>
#include <QtCore/QVariant>
#include <QtCore/QQueue>

class DBFField
{

public:
	enum Type {
		TYPE_CHAR = 0,
		TYPE_NUMERIC,
		TYPE_LOGICAL,
		TYPE_MEMO,
		TYPE_DATE,
		TYPE_FLOAT
	};

public:
	DBFField () : m_data (0), m_codec (0), m_type (-1), m_firstLenght (-1), m_secondLenght (-1)
		{
			m_codec = QTextCodec::codecForName("IBM866");
		}

	DBFField (const char* data) : m_data (0), m_codec (0), m_type (-1), m_firstLenght (-1), m_secondLenght (-1)
		{
			m_codec = QTextCodec::codecForName("IBM866");
			setData (data);
		}

	~DBFField ()
		{
			if (m_data) {
				delete [] m_data;
			}
		}

	void setTextCodec (QTextCodec *codec)
		{
			m_codec = codec;

			m_name.clear ();
		}


	void setData (const char* data)
		{
			clear ();
			if (data) {
				m_data = new char [strlen (data)];
				strcpy (m_data, data);
			}
		}

	const char* data ()
		{ return m_data;}

	QString name () 
		{ 
			if (m_name.isEmpty ()) {
				int i = 0;
				for (; i < 10, m_data [i] != 0x0; i++) {}
				m_name = m_codec->toUnicode(QByteArray (m_data, i));
			}
			return m_name;
		}

	qint8 type ()
		{
			if (m_type < 0) {
				switch (m_data [11]) {
				case 'N':
					m_type = TYPE_NUMERIC;
					break;
				case 'L':
					m_type = TYPE_LOGICAL;
					break;
				case 'M':
					m_type = TYPE_MEMO;
					break;
				case 'D':
					m_type = TYPE_DATE;
					break;
				case 'F':
					m_type = TYPE_FLOAT;
					break;
				case 'C':
					m_type = TYPE_CHAR;
					break;
				}
			}
			return m_type;
		}

	qint8 firstLenght ()
		{
			if (m_firstLenght < 0) {
				m_firstLenght = m_data [16];
			}
			return m_firstLenght;
		}

	qint8 secondLenght ()
		{
			if (m_secondLenght < 0) {
				m_secondLenght = m_data [17];
			}
			return m_secondLenght;
		}

	void clear ()
		{
			if (m_data) {
				delete [] m_data;
				m_data = 0;
			}

			m_name.clear ();
			m_type = m_firstLenght = m_secondLenght = -1;
		}

	DBFField (const DBFField& f) : m_data (0), m_codec (0), m_type (-1), m_firstLenght (-1), m_secondLenght (-1)
		{
			*this = f;
		}

	DBFField& operator= (const DBFField& f)
		{
			if (this != &f) {
				setData (f.m_data);
				m_codec = f.m_codec;
			}
			return *this;
		}
	
private:
	char *m_data;
	QTextCodec *m_codec;
	QString m_name;
	qint8 m_type;
	qint8 m_firstLenght;
	qint8 m_secondLenght;
};

class DBFHeader
{

public:
	enum FileType{
		Fox3 = 0,
		Fox3M,
		Fox4,
		Fox,
		DBase3,
		DBase4,
		DBase5
	};

public:
	DBFHeader () : m_data (0), m_codec (0), m_fileType (-1), m_recordsCount (-1), 
				   m_firstRecordPos (-1), m_recordLenght (-1)
		{
			m_codec = QTextCodec::codecForName("IBM866");
		}
	DBFHeader (const char* data) : m_data (0), m_codec (0), m_fileType (-1), m_recordsCount (-1), 
								   m_firstRecordPos (-1), m_recordLenght (-1)
		{
			m_codec = QTextCodec::codecForName("IBM866");
			setData (data);
		}

	~DBFHeader ()
		{
			if (m_data) {
				delete [] m_data;
			}
		}

	void setTextCodec (QTextCodec *codec)
		{
			m_codec = codec;

			if (!m_fieldsList.isEmpty ()) {
				for (QVector <DBFField>::iterator begin = m_fieldsList.begin (), end = m_fieldsList.end (); begin != end; ++begin) {
					begin->setTextCodec (m_codec);
				}
			}
		}

	void setData (const char* data)
		{
			clear ();
			if (data) {
				m_data = new char [strlen (data)];
				strcpy (m_data, data);
			}
		}

	const char* data () const
		{ return m_data;}

	qint8 fileType () //This function is very bad
		{
			if (m_fileType < 0) {
				if (m_data [0] & 0x3)
					m_fileType = DBase3;
			}
			return m_fileType;
		}

	QDate lastUpdated ()
		{
			if (m_lastUpdated.isNull ()) {
				m_lastUpdated.setDate(2000 + m_data [1], m_data [2], m_data [3]);
			}
			return m_lastUpdated;
		}

	void setLastUpdated (const QDate& date = QDate::currentDate ()) 
		{
			m_lastUpdated = date;
			m_data [1] = m_lastUpdated.year () - 2000;
			m_data [2] = m_lastUpdated.month ();
			m_data [3] = m_lastUpdated.day ();
		}

	qint32 recordsCount ()
		{
			if (m_recordsCount < 0) {
				char *c = m_data + 8;
				m_recordsCount = *(qint32*) c;
			}
			return m_recordsCount;
		}
	
	void setRecordsCount (qint32 count)
		{
			m_recordsCount = count;
			char *c = m_data + 8;
			*(qint32*) c = m_recordsCount;
		}

	void addRecordsCount (qint32 count = 1)
		{
			setRecordsCount (m_recordsCount + count);
		}

	qint16 firstRecordPos ()
		{
			if (m_firstRecordPos < 0) {
				char *c = m_data + 12;
				m_firstRecordPos = *(qint32*) c;
			}
			return m_firstRecordPos; 
		}

	qint16 recordLenght ()
		{
			if (m_recordLenght < 0) {
				char *c = m_data + 14;
				m_recordLenght = *(qint32*) c;
			}
			return m_recordLenght; 
		}

	bool hasIndex ()
		{
			return m_data [28] == 1;
		}

	void addField (const char* field)
		{
			m_fieldsList.push_back (DBFField (field));
		}

	DBFField field (int index)
		{
			return index < m_fieldsList.size () ? m_fieldsList [index] : DBFField ();
		}

	qint16 fieldPos (int index)
		{
			if (index > m_fieldsList.size ()) {
				return -1;
			}

			qint16 pos = 1;
			
			for (int i = 0; i <= index; i++) {
				pos += m_fieldsList [i].firstLenght ();
			}

			return pos;
		}

	int fieldsCount () const
		{
			return m_fieldsList.size ();
		}

	void clear ()
		{
			if (m_data) {
				delete [] m_data;
				m_data = 0;
			}
			
			m_fileType = m_recordsCount = m_firstRecordPos = m_recordLenght = -1;
			m_lastUpdated = QDate ();
			m_fieldsList.clear ();
		}
private:
	DBFHeader (const DBFHeader&);
	const DBFHeader& operator= (const DBFHeader&);

private:
	char *m_data;
	QTextCodec *m_codec;
	qint8 m_fileType;
	QDate m_lastUpdated;
	qint32 m_recordsCount;
	qint16 m_firstRecordPos;
	qint16 m_recordLenght;
	QVector <DBFField> m_fieldsList;
};

class DBFRecord
{

public:
	DBFRecord (DBFHeader *header, const char* data) : m_header (header), m_data (0), m_codec (0)
		{
			m_codec = QTextCodec::codecForName("IBM866");			
			setData (data);
		}

	DBFRecord (const DBFRecord& record)
		{
			*this = record;
		}

	~DBFRecord ()
		{
			clear ();
		}

	DBFRecord& operator= (const DBFRecord& r)
		{
			if (this != &r) {
				m_header = r.m_header;
				setData (r.m_data);
				m_codec = r.m_codec;
			}
		}

	bool operator== (const DBFRecord& r)
		{
			return strcmp (m_data, r.m_data) == 0;
		}

	const char* data () const
		{
			return m_data;
		}

	void setData (const char* data)
		{
			clear ();
			if (data) {
				m_data = new char [strlen (data)];
				strcpy (m_data, data);
			}
		}

	void setTextCodec (QTextCodec *codec)
		{
			m_codec = codec;
		}

	void clear ()
		{
			if (m_data) {
				delete [] m_data;
				m_data = 0;
			}
		}

	QString value (int fieldIndex)
		{
			if (fieldIndex > m_header->fieldsCount ()) {
				return 0;
			}

			DBFField field = m_header->field (fieldIndex);

			char *c = m_data + m_header->fieldPos (fieldIndex);

			char *tmp = new char [field.firstLenght ()];
			strncpy (tmp, c, field.firstLenght ());

			QString value (tmp);
			delete [] tmp;
			
			return value;
		}

private:
	DBFHeader *m_header;
	char *m_data;
	QTextCodec *m_codec;
};


class DBFRedactor
{
public:

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
		QVector <Field> fieldsList;
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

	enum ErrorCode {
		NoError = 0,
		ErrorOpen,
		ErrorReading,
		ErrorWriting,
		FileNotCorrect
	};

private:
	Header header;
	QString m_fileName;
	QFile m_file;
	QByteArray m_buf;
	QHash<int, QByteArray> m_cache;
	QList<QPair<int, QByteArray> > m_changedData;
	QTextCodec *m_codec;
	QString m_tableName;
	int lastRecord;
	DBFOpenMode m_openMode;
	bool m_buffering;
	bool m_modified;
	ErrorCode m_lastError;

public:
	DBFRedactor();
	DBFRedactor(const QString& fileName);
	~DBFRedactor();

	bool open(DBFOpenMode OpenMode, const QString& fileName);
	bool open(DBFOpenMode OpenMode);
	void close();

	QVector<DBFRedactor::Field> fields () const
	{ return header.fieldsList;}

	DBFRedactor::Field field(int number) const;

	QByteArray strRecord(int row);
	DBFRedactor::Record record(int number);

	int columnsCount() const
	{return header.fieldsList.count();}

	int rowsCount() const;

	QString tableName() const
	{return m_tableName;}

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

	DBFOpenMode openMode() const
	{return m_openMode;}
	void setOpenMode(DBFOpenMode openMode);

	QTextCodec* textCodec() const
	{return m_codec;}
	void setTextCodec(QTextCodec *textCodec)
	{
		if (m_codec->name() != textCodec->name())
			m_codec = textCodec;
	}

	void addRecord();

	void removeRecord(int row);

	void recoverRecord(int row);

	bool isChanged(int row) const;

	bool save();

	bool modified() const
	{return m_modified;}

	static QStringList typeCaptions ()
	{return QStringList() << "C" << "N" << "L" << "M" << "D" << "F";}

	ErrorCode lastError () const
	{ return m_lastError;}

	QString errorString (ErrorCode errorCode) const;
	QString errorString () const
	{ return errorString (m_lastError);}

private:
	void writeHeader();
};
#endif
