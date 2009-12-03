#include "dbfredactor.h"
#define MAX_BUFFER_SIZE 10000

DBFRedactor::DBFRedactor()
	:m_fileName(0), m_openMode(No)
{
	header.recordsCount = -1;
}

DBFRedactor::DBFRedactor(const QString& fileName)
	: m_fileName(fileName)
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
	header.recordsCount = revert(m_buf.mid(4,4)).toHex().toLong(&ok,16);
	header.firstRecordPos = revert(m_buf.mid(8,2)).toHex().toInt(&ok,16);
	header.recordLenght = revert(m_buf.mid(10,2)).toHex().toInt(&ok,16);
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
		field.firstLenght = m_buf.mid(16, 1).toHex().toInt(&ok, 16);
		field.secondLenght = m_buf.mid(17, 1).toHex().toInt(&ok, 16);
		if ((field.type == TYPE_NUMERIC) && (field.firstLenght > 18))
			field.type=TYPE_FLOAT;

		header.fieldsList << field;
		m_buf = m_file.read(32);
	} while (m_buf.at(0) != 13);
	m_file.seek(header.firstRecordPos);
	m_buf.clear();
	return true;
}

QByteArray DBFRedactor::revert(const QByteArray& array)
{
	QByteArray newArray;
	for (int i = array.length() - 1; i >= 0; i--)
		newArray.append(array.at(i));
	return newArray;
}

Field DBFRedactor::field(int number)
{
	if (number < header.fieldsList.count())
		return header.fieldsList.at(number);
	else
		return Field();
}

QByteArray DBFRedactor::strRecord(int number)
{
	if ((number >= header.recordsCount) || (!m_file.isOpen()))
		return QByteArray();
	if (m_hash.contains(number)) {
		m_buf = m_hash.value(number);
	} else {
		if (lastRecord != number - 1)
			m_file.seek(header.firstRecordPos + header.recordLenght * number);
		m_buf = m_file.read(header.recordLenght);
		lastRecord = number;
		m_hash.insert(number, m_buf);
		if (m_hash.size() > MAX_BUFFER_SIZE)
			m_hash.erase(m_hash.begin());
	}
	return m_buf;
}

Record DBFRedactor::record(int number)
{
	if (number < 0 || number >= header.recordsCount || !m_file.isOpen())
		return Record();

	if (m_hash.contains(number)) {
		m_buf = m_hash.value(number);
	} else {
		if (lastRecord != number - 1)
			m_file.seek(header.firstRecordPos + header.recordLenght * number);
		m_buf = m_file.read(header.recordLenght);
		lastRecord = number;
		m_hash.insert(number, m_buf);
		if (m_hash.size() > MAX_BUFFER_SIZE)
			m_hash.erase(m_hash.begin());
	}

	Record record;
	QString recordString = m_codec->toUnicode(m_buf);
	QString tempString;
	int pos = 1;
	for (int i = 0; i < header.fieldsList.count(); i++) {
		tempString = recordString.mid(pos, header.fieldsList.at(i).firstLenght);
		pos += header.fieldsList.at(i).firstLenght;
		//Delete last spaces
		if (!tempString.isEmpty()) {
			int i = tempString.length();
			while ((--i >= 0) && tempString[i].isSpace()) {;}
			tempString.remove(i + 1,tempString.length());
		}

		switch (header.fieldsList.at(i).type) {
			case TYPE_CHAR:
				record.value.append(QVariant(tempString));
				break;
			case TYPE_NUMERIC:
				record.value.append(QVariant(tempString.toDouble()));
				break;
			case TYPE_LOGICAL:
				if (tempString.toInt() == 0)
					record.value.append("FALSE");
				else
					record.value.append("TRUE");
				break;
			case TYPE_DATE:
				record.value.append(QVariant(QDate::fromString(tempString,"yyyyMMdd")));
				break;
			case TYPE_FLOAT:
				record.value.append(QVariant(tempString.toDouble()));
				break;
			case TYPE_MEMO:
				record.value.append(QVariant());
				break;
		}
	}
	record.isDeleted = m_buf.at(0) == 0x2A;
	lastRecord = number;
	m_buf.clear();
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
