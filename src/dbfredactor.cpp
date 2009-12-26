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

#include "dbfredactor.h"
#define READING_RECORDS_COUNT 40960

DBFRedactor::DBFRedactor()
	:m_fileName(0), m_openMode(No), m_buffering(true)
{
	header.recordsCount = -1;
}

DBFRedactor::DBFRedactor(const QString& fileName)
	: m_fileName(fileName), m_openMode(No), m_buffering(true)
{
	header.recordsCount = -1;
}

DBFRedactor::~DBFRedactor()
{
	close();
}

void DBFRedactor::close()
{
	header.fieldsList.clear();
	m_file.close();
	m_cache.clear();
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
	m_file.setTextModeEnabled(false);

	QIODevice::OpenMode openMode;
	if (OpenMode == DBFRedactor::Read)
		openMode = QIODevice::ReadOnly;
	if (OpenMode == DBFRedactor::Write)
		openMode = QIODevice::ReadWrite;

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

		field.pos = header.fieldsList.isEmpty() ? 1 : header.fieldsList.last().pos + header.fieldsList.last().firstLenght;

		field.firstLenght = m_buf.mid(16, 1).toHex().toInt(&ok, 16);
		field.secondLenght = m_buf.mid(17, 1).toHex().toInt(&ok, 16);
		if ((field.type == TYPE_NUMERIC) && (field.firstLenght > 18))
			field.type=TYPE_FLOAT;

		header.fieldsList << field;
		m_buf = m_file.read(32);
	} while (m_buf.at(0) != 13);
	m_file.seek(header.firstRecordPos);

	m_buf.clear();
	lastRecord = -1;
	return true;
}

QByteArray DBFRedactor::revert(const QByteArray& array) const
{
	QByteArray newArray;
	for (int i = array.length() - 1; i >= 0; i--)
		newArray.append(array.at(i));
	return newArray;
}

DBFRedactor::Field DBFRedactor::field(int number) const
{
	return number < header.fieldsList.count() ? header.fieldsList.at(number) : Field();
}

QByteArray DBFRedactor::strRecord(int row)
{
	if (!m_file.isOpen() || row < 0 || row >= header.recordsCount)
		return false;

	if (!m_cache.contains(row)) {
		if (!m_buffering)
			m_cache.clear();
		qint64 filePos = 0;
		if (row >= lastRecord) {
			filePos = header.firstRecordPos + header.recordLenght * row;
			lastRecord = row;
		} else {
			filePos = header.firstRecordPos + header.recordLenght * row - (READING_RECORDS_COUNT - 1) * header.recordLenght;
			lastRecord = row - READING_RECORDS_COUNT + 1;
			if (filePos < header.firstRecordPos) {
				filePos = header.firstRecordPos;
				lastRecord = 0;
			}
		}
		m_file.seek(filePos);
		m_buf = m_file.read(READING_RECORDS_COUNT * header.recordLenght);
		int pos = 0;
		while (m_buf.size() - pos >= header.recordLenght) {
			m_cache.insert(lastRecord++, m_buf.mid(pos, header.recordLenght));
			pos += header.recordLenght;
		}
	}

	m_buf = m_cache.value(row);

	Q_ASSERT(m_buf.size() == header.recordLenght);

	return m_buf;
}

QVariant DBFRedactor::data(int row, int column)
{
	if (!m_file.isOpen() || row < 0 || row >= header.recordsCount || column < 0 || column >= header.fieldsList.size())
		return QVariant();

	QString tempString = m_codec->toUnicode(strRecord(row).mid(header.fieldsList.at(column).pos, header.fieldsList.at(column).firstLenght));

	switch (header.fieldsList.at(column).type) {
		case TYPE_CHAR:
			//Delete last spaces
			if (!tempString.isEmpty()) {
				int i = tempString.length();
				while ((--i >= 0) && tempString[i].isSpace()) {;}
				tempString.remove(i + 1, tempString.length());
			}
			return tempString;
			break;
		case TYPE_NUMERIC:
			return tempString.trimmed();
			break;
		case TYPE_LOGICAL:
			return tempString.trimmed() == "T";
			break;
		case TYPE_DATE:
			return QDate::fromString(tempString.trimmed(), "yyyyMMdd");
			break;
		case TYPE_FLOAT:
			return tempString.trimmed().toDouble();
			break;
		case TYPE_MEMO:
			return QVariant();
			break;
	}
	return QVariant();
}

bool DBFRedactor::setData(int row, int column, const QVariant& data)
{
	if (m_openMode != Write || row < 0 || row >= header.recordsCount || column < 0 || column >= header.fieldsList.size())
		return false;

	m_cache.remove(row);
	m_file.seek(header.firstRecordPos + header.recordLenght * row + header.fieldsList.at(column).pos);

	QByteArray buf;

	switch (header.fieldsList.at(column).type) {
		case TYPE_CHAR:
			buf = m_codec->fromUnicode(data.toString());
			break;
		case TYPE_NUMERIC:
			buf = m_codec->fromUnicode(QString::number(data.toString().left(header.fieldsList.at(column).firstLenght).toDouble()));
			/*
			  if data = 999.99 and firstLenght = 4 write not 999., write 999
			  */
			break;
		case TYPE_LOGICAL:
			buf = data.toBool() ? "T" : "F";
			break;
		case TYPE_DATE:
			buf = m_codec->fromUnicode(data.toDate().toString("yyyyMMdd"));
			break;
		case TYPE_FLOAT:
			buf.setNum(data.toDouble(), 'f', header.fieldsList.at(column).secondLenght);
			break;
		case TYPE_MEMO:
			buf = m_codec->fromUnicode(data.toString());
			break;
	}
	return m_file.write(buf.leftJustified(header.fieldsList.at(column).firstLenght, 0x20, true)) > 0;
}

bool DBFRedactor::isDeleted(int row)
{
	if (!m_file.isOpen() || row < 0 || row >= header.recordsCount)
		return false;

	if (m_cache.contains(row))
		return m_cache.value(row).at(0) == 0x2A;

	m_file.seek(header.firstRecordPos + header.recordLenght * row);
	char ch;
	m_file.read(&ch, 1);
	return ch == 0x2A;
}

DBFRedactor::Record DBFRedactor::record(int number)
{
	if (!m_file.isOpen() || number < 0 || number >= header.recordsCount)
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
				record.value.append(tempString == "T");
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

bool DBFRedactor::compareRecord(const Record& first, const Record& second)
{
	if (first.isDeleted != second.isDeleted)
		return false;

	if (first.value.size() != second.value.size())
		return false;

	for (int i = 0; i < first.value.size(); i++)
		if (first.value.at(i) != second.value.at(i))
			return false;
	return true;
}

bool DBFRedactor::isOpen() const
{
	return m_file.isOpen();
}

int DBFRedactor::deletedCount()
{
	qint64 pos = m_file.pos();
	m_file.seek(header.firstRecordPos);

	long deletedCount = 0;
	for (int i = 0; i < header.recordsCount; i++) {
		char ch;
		m_file.read(&ch, 1);
		if (ch == 0x2A)
			deletedCount++;
		m_file.seek(header.firstRecordPos + header.recordLenght * (i + 1));
	}
	m_file.seek(pos);
	return deletedCount;
}

int DBFRedactor::rowsCount() const
{
	return m_file.isOpen() ? header.recordsCount : 0;
}

void DBFRedactor::refresh()
{
	close();
	open(m_openMode);
}

void DBFRedactor::setOpenMode(DBFOpenMode openMode)
{
	m_openMode = openMode;
	QIODevice::OpenMode mode;
	if (m_openMode & DBFRedactor::Read)
		mode = QIODevice::ReadOnly;
	if (m_openMode & DBFRedactor::Write)
		mode = QIODevice::ReadWrite;

	m_file.close();
	m_file.open(mode);
	m_cache.clear();
}
