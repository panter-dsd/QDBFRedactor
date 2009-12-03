#ifndef DBFREDACTOR_H
#define DBFREDACTOR_H

#include <QtCore/QStringList>
#include <QtCore/QDate>
#include <QtCore/QList>
#include <QtCore/QFile>
#include <QtCore/QTextCodec>
#include <QtCore/QFileInfo>
#include <QtCore/QVariant>
#include <QtCore/QHash>

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
	Dbase4};

struct Field
{
	QString name;
	int type;
	long pos;
	int firstLenght;
	int secondLenght;
};

struct Header
{
	int fileType;
	QDate lastUpdated;
	long recordsCount;
	int firstRecordPos;
	int recordLenght;
	bool isIndex;
	QList<Field> fieldsList;
};

struct Record
{
	bool isDeleted;
	QList<QVariant> value;
};

class DBFRedactor
{
public:
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
	QHash<int, QByteArray> m_hash;
	QTextCodec	 *m_codec;
	QString m_tableName;
	int lastRecord;
	DBFOpenMode m_openMode;
	bool m_buffering;

public:
	DBFRedactor();
	DBFRedactor(const QString& fileName);
	~DBFRedactor();

	bool open(DBFOpenMode OpenMode, const QString& fileName);
	bool open(DBFOpenMode OpenMode);
	void close();

	Field field(int number);
	QByteArray strRecord(int number);
	Record record(int number);
	int columnsCount()
	{return header.fieldsList.count();}

	int rowsCount();
	QString tableName()
	{return m_tableName;}

	static bool compareRecord(Record *first, Record *second);
	bool isOpen();
	int deletedCount();

	void refresh();

	bool buffering()
	{return m_buffering;}
	void setBuffering(bool b)
	{
		m_buffering = b;
		if (!b)
			m_hash.clear();
	}

private:
	QByteArray revert(const QByteArray& array);
};
#endif
