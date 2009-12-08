#include <QtCore/QDebug>

#include "dbfredactor.h"
#define MAX_BUFFER_SIZE 10000
#define READING_RECORDS_COUNT 40960

DBFRedactor::DBFRedactor()
	:m_fileName(0), m_openMode(No), m_buffering(true)
{
	header.recordsCount = -1;
}

DBFRedactor::DBFRedactor(const QString& fileName)
	: m_fileName(fileName), m_openMode(No), m_buffering(true)
{
	m_tableName = QFileInfo(m_fileName).baseName();
}

DBFRedactor::~DBFRedactor()
{
	close();
}

void DBFRedactor::close()
{
	header.fieldsList.clear();
	m_file.close();
	m_hash.clear();
}

bool DBFRedactor::open(DBFOpenMode OpenMode)
{
	return open(OpenMode, m_fileName);
}

bool DBFRedactor::open(DBFOpenMode OpenMode, const QString& fileName)
{
	if (!fileName.isEmpty())
		m_fileName = fileName;

	if (m_fileName.isEmpty())
		return false;

	m_openMode = OpenMode;
	m_tableName = QFileInfo(m_fileName).baseName();
	m_file.setFileName(m_fileName);

	QIODevice::OpenMode openMode;
	if (OpenMode == DBFRedactor::Read)
		openMode = QIODevice::ReadOnly;
	if (OpenMode == DBFRedactor::Write)
		openMode = QIODevice::Append;

	if (!m_file.open(openMode)) {
		qDebug("Error open file");
		return false;
	}

	m_buf = m_file.read(32);
	bool ok = false;
#warning "Type is not true."
	header.fileType = m_buf.at(0);
	header.recordsCount = revert(m_buf.mid(4, 4)).toHex().toLong(&ok, 16);
	header.firstRecordPos = revert(m_buf.mid(8, 2)).toHex().toInt(&ok, 16);
	header.recordLenght = revert(m_buf.mid(10, 2)).toHex().toInt(&ok, 16);
	header.isIndex = (m_buf.at(28) == 1);

	if (m_buf.at(29) == -55)
		m_codec = QTextCodec::codecForName("CP1251");
	else
		m_codec = QTextCodec::codecForName("IBM866");

	m_buf = m_file.read(32);
	do {
		Field field;
		field.name = m_codec->toUnicode(m_buf.left(10));
		field.name.remove(field.name.indexOf(QChar(0)), 10);
		switch (m_buf.at(11)) {
			case 'C':
				field.type = TYPE_CHAR;
				break;
			case 'N':
				field.type = TYPE_NUMERIC;
				break;
			case 'L':
				field.type = TYPE_LOGICAL;
				break;
			case 'M':
				field.type = TYPE_MEMO;
				break;
			case 'D':
				field.type = TYPE_DATE;
				break;
			case 'F':
				field.type = TYPE_FLOAT;
				break;
			case 'P':
				field.type = TYPE_P;
				break;
		}
		field.pos = revert(m_buf.mid(12, 4)).toHex().toLong(&ok, 16);
		if (!field.pos) {
			field.pos = 1;
			foreach(Field f, header.fieldsList)
				field.pos += f.firstLenght;
		}

		field.firstLenght = m_buf.mid(16, 1).toHex().toInt(&ok, 16);
		field.secondLenght = m_buf.mid(17, 1).toHex().toInt(&ok, 16);
		if ((field.type == TYPE_NUMERIC) && (field.firstLenght > 18))
			field.type=TYPE_FLOAT;

		header.fieldsList << field;
		m_buf = m_file.read(32);
	} while (m_buf.at(0) != 13);
	m_file.seek(header.firstRecordPos);

	lastRecord = 0;
	while (!m_file.atEnd()) {
		m_buf = m_file.read(READING_RECORDS_COUNT * header.recordLenght);
		int pos = 0;
		while (m_buf.size() - pos >= header.recordLenght) {
			m_hash.insert(lastRecord++, m_buf.mid(pos, header.recordLenght));
			pos += header.recordLenght;
		}
	}

	m_buf.clear();
	m_file.seek(header.firstRecordPos);
	lastRecord = 0;
	return true;
}

QByteArray DBFRedactor::revert(const QByteArray& array)
{
	QByteArray newArray;
	for (int i = array.length() - 1; i >= 0; i--)
		newArray.append(array.at(i));
	return newArray;
}

DBFRedactor::Field DBFRedactor::field(int number)
{
	return number < header.fieldsList.count() ? header.fieldsList.at(number) : Field();
}

QByteArray DBFRedactor::strRecord(int row)
{
	if (row < 0 || row >= header.recordsCount || !m_file.isOpen())
		return false;

	m_buf = m_hash.value(row);
	Q_ASSERT(m_buf.size() == header.recordLenght);

	return m_buf;
}

QVariant DBFRedactor::data(int row, int column)
{
	if (row < 0 || row >= header.recordsCount || !m_file.isOpen())
		return QVariant();

	if (column < 0 || column >= header.fieldsList.size() || !m_file.isOpen())
		return QVariant();

	QString tempString = m_codec->toUnicode(strRecord(row)).mid(header.fieldsList.at(column).pos, header.fieldsList.at(column).firstLenght);
		//Delete last spaces
	if (!tempString.isEmpty()) {
		int i = tempString.length();
		while ((--i >= 0) && tempString[i].isSpace()) {;}
		tempString.remove(i + 1, tempString.length());
	}

	switch (header.fieldsList.at(column).type) {
		case TYPE_CHAR:
			return QVariant(tempString);
			break;
		case TYPE_NUMERIC:
			return QVariant(tempString.toDouble());
			break;
		case TYPE_LOGICAL:
			if (tempString.toInt() == 0)
				return QVariant("FALSE");
			else
				return QVariant("TRUE");
			break;
		case TYPE_DATE:
			return QVariant(QDate::fromString(tempString, "yyyyMMdd"));
			break;
		case TYPE_FLOAT:
			return QVariant(tempString.toDouble());
			break;
		case TYPE_MEMO:
			return QVariant();
			break;
	}
	return QVariant();
}

bool DBFRedactor::isDeleted(int row)
{
	if (row < 0 || row >= header.recordsCount || !m_file.isOpen())
		return false;

	return strRecord(row).at(0) == 0x2A;
}

DBFRedactor::Record DBFRedactor::record(int number)
{
	if (number < 0 || number >= header.recordsCount || !m_file.isOpen())
		return Record();

	Record record;
	QString recordString = m_codec->toUnicode(strRecord(number));
	QString tempString;
	for (int i = 0; i < header.fieldsList.count(); i++) {
		tempString = recordString.mid(header.fieldsList.at(i).pos, header.fieldsList.at(i).firstLenght);
		//Delete last spaces
		if (!tempString.isEmpty()) {
			int i = tempString.length();
			while ((--i >= 0) && tempString[i].isSpace()) {;}
			tempString.remove(i + 1, tempString.length());
		}

		switch (header.fieldsList.at(i).type) {
			case TYPE_CHAR:
				record.value.append(tempString);
				break;
			case TYPE_NUMERIC:
				record.value.append(tempString.toDouble());
				break;
			case TYPE_LOGICAL:
				if (tempString.toInt() == 0)
					record.value.append("FALSE");
				else
					record.value.append("TRUE");
				break;
			case TYPE_DATE:
				record.value.append(QDate::fromString(tempString, "yyyyMMdd"));
				break;
			case TYPE_FLOAT:
				record.value.append(tempString.toDouble());
				break;
			case TYPE_MEMO:
				record.value.append(QVariant());
				break;
		}
	}
	record.isDeleted = m_buf.at(0) == 0x2A;
	return record;
}

bool DBFRedactor::compareRecord(Record* first,Record* second)
{
	if (first->isDeleted != second->isDeleted)
		return false;

	if (first->value.count() != second->value.count())
		return false;

	for (int i = 0; i < first->value.count(); i++)
		if (first->value.at(i) != second->value.at(i))
			return false;
	return true;
}

bool DBFRedactor::isOpen()
{
	return m_file.isOpen();
}

int DBFRedactor::deletedCount()
{
	qint64 pos = m_file.pos();
	m_file.seek(header.firstRecordPos);

	long deletedCount = 0;
	for (int i = 0; i < header.recordsCount; i++)
	{
		m_buf = m_file.read(1);
		if (m_buf.at(0) == 0x2A)
			deletedCount++;
		m_file.seek(header.firstRecordPos + header.recordLenght * (i + 1));
	}
	m_file.seek(pos);
	return deletedCount;
}

int DBFRedactor::rowsCount()
{
	if (m_file.isOpen())
		return header.recordsCount;
	else
		return 0;
}

void DBFRedactor::refresh()
{
	close();
	open(m_openMode);
}
