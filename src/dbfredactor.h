#ifndef DBFREDACTOR_H
#define DBFREDACTOR_H
//
#include <QtCore/QStringList>
#include <QtCore/QDate>
#include <QtCore/QList>
#include <QtCore/QFile>
#include <QtCore/QTextCodec>
#include <QtCore/QFileInfo>
#include <QtCore/QVariant>
#include <QtCore/QHash>
//
enum FieldType{
	TYPE_CHAR,
	TYPE_NUMERIC,
	TYPE_LOGICAL,
	TYPE_MEMO,
	TYPE_DATE,
	TYPE_FLOAT,
	TYPE_P};
//
enum FileType{
	Fox3,
	Fox3M,
	Fox4,
	Fox,
	Dbase4};
//
struct SField
{
	QString				qsName;
	int						type;
	long						pos;
	int 						firstLenght;
	int 						secondLenght;
};
//
struct SHeader
{
	int							type;
	QDate						qdLastUpdate;
	long							count;
	int							pos;
	int							lenghtRecord;
	bool							isIndex;
	QList<SField>		fieldsList;
};
//
struct SRecord
{
	bool								isDeleted;
	QList<QVariant>		value;
};
//
class DBFRedactor
{
public:
	enum DBFOpenModeFlag {
		Read,
		Write
	};
	Q_DECLARE_FLAGS(DBFOpenMode, DBFOpenModeFlag);
private:
	SHeader header;
	QString qsFileName;
	QFile qfFile;
	QByteArray buf;
	QHash<int,QByteArray> qhBuffer;
	QTextCodec	 *codec;
	QString qsTableName;
	int lastRecord;
public:
	DBFRedactor();
	DBFRedactor(const QString& fileName);
	~DBFRedactor();

	bool open(DBFOpenMode OpenMode, const QString& fileName);
	bool open(DBFOpenMode OpenMode);
	void close();

	SField field(int number);
	QByteArray strRecord(int number);
	SRecord* record(int number);
	int fieldCount() {return header.fieldsList.count();}
	int recordsCount();
	QString tableName() {return qsTableName;}
	static bool compareRecord(SRecord* qzFirst, SRecord* qzSecond);
	bool isOpen();
	int deletedCount();
private:
	QByteArray revert(const QByteArray& array);
};
#endif
