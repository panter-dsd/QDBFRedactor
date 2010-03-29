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
	m_codec = QTextCodec::codecForName("IBM866");
}

DBFRedactor::DBFRedactor(const QString& fileName)
	: m_fileName(fileName), m_openMode(No), m_buffering(true)
{
	header.recordsCount = -1;
	m_codec = QTextCodec::codecForName("IBM866");
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
	m_changedData.clear();
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

	char *c = new char[33];
	char *cTmp = c;
	m_file.read(c, 32);

#warning "Type is not true."
	if (c[0] & 0x3)
		header.fileType = DBase3;
	header.lastUpdated.setDate(2000 + c[1], c[2], c[3]);
	cTmp += 4;
	header.recordsCount = *(qint32*)cTmp;
	cTmp += 4;
	header.firstRecordPos = *(qint16*)cTmp;
	cTmp += 2;
	header.recordLenght = *(qint16*)cTmp;
	header.isIndex = (c[28] == 1);

	m_file.read(c, 32);
	do {
		Field field;
		m_buf.clear();
		for (int i = 0; i < 10, c[i] != 0x0; i++)
			m_buf.append(c[i]);
		field.name = m_codec->toUnicode(m_buf);
		switch (c[11]) {
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
			case 'C': default:
				field.type = TYPE_CHAR;
				break;
		}

		field.pos = header.fieldsList.isEmpty() ? 1 : header.fieldsList.last().pos + header.fieldsList.last().firstLenght;

		field.firstLenght = c[16];
		field.secondLenght = c[17];
		if ((field.type == TYPE_NUMERIC) && (field.firstLenght > 18))
			field.type=TYPE_FLOAT;

		header.fieldsList << field;
		m_file.read(c, 32);
	} while (c[0] != 0xD);
	delete [] c;
	m_file.seek(header.firstRecordPos);

	m_buf.clear();
	lastRecord = -1;
	m_modified = false;
	return true;
}

DBFRedactor::Field DBFRedactor::field(int number) const
{
	return number < header.fieldsList.count() ? header.fieldsList.at(number) : Field();
}

QByteArray DBFRedactor::strRecord(int row)
{
	if (!m_file.isOpen() || row < 0 || row >= header.recordsCount)
		return false;

//Search in changedData
	for (int i = m_changedData.size() - 1; i >= 0; i--) {
		if (m_changedData.at(i).first == row)
			return m_changedData.at(i).second;
	}

	if (!m_cache.contains(row)) {//If not in cache
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

	QByteArray tmpBuf;
	switch (header.fieldsList.at(column).type) {
		case TYPE_CHAR:
			tmpBuf = m_codec->fromUnicode(data.toString());
			break;
		case TYPE_NUMERIC:
			tmpBuf = m_codec->fromUnicode(QString::number(data.toString().left(header.fieldsList.at(column).firstLenght).toDouble()));
			/*
			  if data = 999.99 and firstLenght = 4 write not 999., write 999
			  */
			break;
		case TYPE_LOGICAL:
			tmpBuf = data.toBool() ? "T" : "F";
			break;
		case TYPE_DATE:
			tmpBuf = m_codec->fromUnicode(data.toDate().toString("yyyyMMdd"));
			break;
		case TYPE_FLOAT:
			tmpBuf.setNum(data.toDouble(), 'f', header.fieldsList.at(column).secondLenght);
			break;
		case TYPE_MEMO:
			tmpBuf = m_codec->fromUnicode(data.toString());
			break;
	}
	m_buf = strRecord(row);
	m_buf.replace(header.fieldsList.at(column).pos,
				  header.fieldsList.at(column).firstLenght,
				  tmpBuf.leftJustified(header.fieldsList.at(column).firstLenght, 0x20, true));

	QPair<int, QByteArray> pair;
	pair.first = row;
	pair.second = m_buf;
	m_changedData.append(pair);
	m_cache.remove(row);

	m_modified = true;
	return true;
}

bool DBFRedactor::isDeleted(int row)
{
	if (!m_file.isOpen() || row < 0 || row >= header.recordsCount)
		return false;

	return strRecord(row).at(0) == 0x2A;
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

bool DBFRedactor::isOpen() const
{
	return m_file.isOpen();
}

int DBFRedactor::deletedCount()
{
	long deletedCount = 0;
	for (int i = 0; i < header.recordsCount; i++)
		if (strRecord(i).at(i) == 0x2A)
			deletedCount++;
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

	m_cache.clear();
	m_changedData.clear();
	m_file.close();
	m_file.open(mode);
	m_modified = false;
}

void DBFRedactor::addRecord()
{
	if (m_openMode != Write)
		return;

	QPair<int, QByteArray> pair;
	pair.first = header.recordsCount++;
	m_buf.clear();
	m_buf.resize(header.recordLenght);
	m_buf.fill(0x20, header.recordLenght);
	pair.second = m_buf;
	m_changedData.append(pair);

	m_modified = true;
}

void DBFRedactor::removeRecord(int row)
{
	if (m_openMode != Write)
		return;

	m_buf = strRecord(row);
	m_buf[0] = 0x2A;
	QPair<int, QByteArray> pair;
	pair.first = row;
	pair.second = m_buf;
	m_changedData.append(pair);
	m_cache.remove(row);

	m_modified = true;
}

void DBFRedactor::recoverRecord(int row)
{
	if (m_openMode != Write)
		return;

	m_buf = strRecord(row);
	m_buf[0] = 0x20;
	QPair<int, QByteArray> pair;
	pair.first = row;
	pair.second = m_buf;
	m_changedData.append(pair);
	m_cache.remove(row);

	m_modified = true;
}

void DBFRedactor::writeHeader()
{
	if (m_openMode != Write)
		return;

	char *c = new char[33];
	for (int i = 0; i < 32; i++)
		c[i] = 0;
	char *tmp = c;

	c[0] = 0x3;
	c[1] = header.lastUpdated.toString("yy").toShort();
	c[2] = header.lastUpdated.month();
	c[3] = header.lastUpdated.day();
	tmp += 4;
	*(qint32*)tmp = header.recordsCount;
	tmp += 4;
	*(qint16*)tmp = header.firstRecordPos;
	tmp += 2;
	*(qint16*)tmp = header.recordLenght;
	m_file.seek(0);
	m_file.write(c, 32);
	delete [] c;
}

bool DBFRedactor::isChanged(int row) const
{
	for (int i = 0; i < m_changedData.size(); i++) {
		if (m_changedData.at(i).first == row)
			return true;
	}
	return false;
}

bool DBFRedactor::save()
{
	if (m_openMode != Write)
		return false;

	QMap<int, QByteArray> writeData;

	for (int i = m_changedData.size() - 1; i >=0; i-- ) {
		if (writeData.contains(m_changedData.at(i).first))
			continue;
		writeData.insert(m_changedData.at(i).first, m_changedData.at(i).second);
	}

	QMapIterator<int, QByteArray> it(writeData);

	while(it.hasNext()) {
		it.next();

		m_file.seek(header.firstRecordPos + header.recordLenght * it.key());
		m_file.write(it.value());
	}

	writeHeader();
	m_changedData.clear();
	m_modified = false;

	return true;
}
