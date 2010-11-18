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
//TEST
#include <iostream>
//END TEST

#include <sstream>

#include "dbfheader.h"

namespace DBFRedactorCore {

DBFHeader::DBFHeader ()
	: m_fileType (UnknowFile), m_lastUpdated (0), m_recordsCount (-1),
	  m_firstRecordPos (-1), m_recordLenght (-1)
{

}

DBFHeader::DBFHeader (const DBFHeader &f)
	: m_fileType (UnknowFile), m_lastUpdated (0), m_recordsCount (-1),
	  m_firstRecordPos (-1), m_recordLenght (-1)
{
	*this = f;
}

DBFHeader::~DBFHeader ()
{

}

DBFHeader& DBFHeader::operator= (const DBFHeader &f)
{
	if (this != &f) {
		m_fileType = f.m_fileType;
		m_lastUpdated = f.m_lastUpdated;
		m_recordsCount = f.m_recordsCount;
		m_firstRecordPos = f.m_firstRecordPos;
		m_recordLenght = f.m_recordLenght;
		m_fieldsList.clear ();
		m_fieldsList = f.m_fieldsList;
	}

	return *this;
}

bool DBFHeader::load (std::iostream &stream)
{
	clear ();

	stream.seekg (0);
	stream.read (m_data, 32);

	{//File type
		if (m_data [0] & 0x3) {
			m_fileType = FoxBase;
		}
	}

	{//Records count
		char* c  = m_data + 4;
		m_recordsCount = *(int32*) c;
	}

	{//First record pos
		char* c  = m_data + 8;
		m_firstRecordPos = *(int16*) c;
	}

	{//Record lenght
		char* c  = m_data + 10;
		m_recordLenght = *(int16*) c;
	}

	{//Record lenght
		m_hasIndex = m_data [28] == 1;
	}

	char field [33];

	stream.read (field, 32);

	do {
		m_fieldsList.push_back (DBFField (field));
		stream.read (field, 32);
	} while (field [0] != 0xd);

	for (int i = 0; i < m_fieldsList.size (); ++i) {
		std::cout << m_fieldsList [i].name () << (int)m_fieldsList [i].firstLenght () << std::endl;
	}
}

bool DBFHeader::save (std::iostream &stream)
{

}

void DBFHeader::clear ()
{
	m_fileType = UnknowFile;
	m_lastUpdated = 0;
	m_recordsCount = m_firstRecordPos = m_recordLenght = -1;
	m_fieldsList.clear ();
}

bool DBFHeader::isEmpty () const
{
	return m_recordsCount < 0 || m_fieldsList.empty ();
}

bool DBFHeader::isValid () const
{
	return !isEmpty ();
}

FileType DBFHeader::fileType () const
{
	return m_fileType;
}

time_t DBFHeader::lastUpdated () const
{
	return m_lastUpdated;
}

void DBFHeader::setLastUpdated (const time_t time)
{

}

int32 DBFHeader::recordsCount () const
{
	return m_recordsCount;
}

void DBFHeader::setRecordsCount (const long count)
{
	m_recordsCount = count;
}

void DBFHeader::addRecordsCount (const long count)
{
	setRecordsCount (m_recordsCount + count);
}

int16 DBFHeader::firstRecordPos () const
{
	return m_firstRecordPos;
}

int16 DBFHeader::recordLenght () const
{
	return m_firstRecordPos;
}

bool DBFHeader::hasIndex () const
{
	return m_hasIndex;
}

DBFField DBFHeader::field (const int index) const
{
	return index > 0 && index < m_fieldsList.size ()
			? m_fieldsList [index]
			: DBFField ();
}

DBFFieldsList DBFHeader::fields () const
{
	return m_fieldsList;
}

int16 DBFHeader::fieldPos (const int index) const
{
	if (index < 0 || index > m_fieldsList.size () ) {
		return -1;
	}

	int16 pos = 1;

	for (int i = 0; i <= index; ++i) {
		pos += m_fieldsList [i].firstLenght ();
	}

	return pos;
}

int8 DBFHeader::fieldsCount () const
{
	return m_fieldsList.size ();
}
}
