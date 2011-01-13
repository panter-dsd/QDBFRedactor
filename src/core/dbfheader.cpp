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
	: fileType_ (UnknowFile), lastUpdated_ (0), recordsCount_ (-1),
	  firstRecordPos_ (-1), recordLenght_ (-1)
{

}

DBFHeader::~DBFHeader ()
{

}

bool DBFHeader::load (std::iostream &stream)
{
	clear ();

	stream.seekg (0);
	stream.read (data_, 32);

	{//File type
		if (data_ [0] & 0x3) {
			fileType_ = FoxBase;
		}
	}

	{//Records count
		char* c  = data_ + 4;
		recordsCount_ = *(int32*) c;
	}

	{//First record pos
		char* c  = data_ + 8;
		firstRecordPos_ = *(int16*) c;
	}

	{//Record lenght
		char* c  = data_ + 10;
		recordLenght_ = *(int16*) c;
	}

	{//Record lenght
		hasIndex_ = data_ [28] == 1;
	}

	DBFFieldDataType fieldData (32, 0);

	stream.read (&*fieldData.begin (), 32);

	do {
		fieldsList_.push_back (DBFField (fieldData));
		stream.read (&*fieldData.begin (), 32);
	} while (fieldData [0] != 0xd);

	for (int i = 0; i < fieldsList_.size (); ++i) {
		std::cout << fieldsList_ [i].name () << (int)fieldsList_ [i].firstLenght () << std::endl;
	}
}

bool DBFHeader::save (std::iostream &stream)
{

}

void DBFHeader::clear ()
{
	fileType_ = UnknowFile;
	lastUpdated_ = 0;
	recordsCount_ = firstRecordPos_ = recordLenght_ = -1;
	fieldsList_.clear ();
}

bool DBFHeader::isEmpty () const
{
	return recordsCount_ < 0 || fieldsList_.empty ();
}

bool DBFHeader::isValid () const
{
	return !isEmpty ();
}

FileType DBFHeader::fileType () const
{
	return fileType_;
}

time_t DBFHeader::lastUpdated () const
{
	return lastUpdated_;
}

void DBFHeader::setLastUpdated (const time_t time)
{

}

int32 DBFHeader::recordsCount () const
{
	return recordsCount_;
}

void DBFHeader::setRecordsCount (const long count)
{
	recordsCount_ = count;
}

void DBFHeader::addRecordsCount (const long count)
{
	setRecordsCount (recordsCount_ + count);
}

int16 DBFHeader::firstRecordPos () const
{
	return firstRecordPos_;
}

int16 DBFHeader::recordLenght () const
{
	return firstRecordPos_;
}

bool DBFHeader::hasIndex () const
{
	return hasIndex_;
}

DBFField DBFHeader::field (const int index) const
{
	return index > 0 && index < fieldsList_.size ()
			? fieldsList_ [index]
			: DBFField ();
}

DBFFieldsList DBFHeader::fields () const
{
	return fieldsList_;
}

int16 DBFHeader::fieldPos (const int index) const
{
	if (index < 0 || index > fieldsList_.size () ) {
		return -1;
	}

	int16 pos = 1;

	for (int i = 0; i <= index; ++i) {
		pos += fieldsList_ [i].firstLenght ();
	}

	return pos;
}

int8 DBFHeader::fieldsCount () const
{
	return fieldsList_.size ();
}
}
