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

#ifndef DBFHEADER_H
#define DBFHEADER_H

#include <ctime>
#include <iostream>
#include <vector>
#include "dbffield.h"

namespace DBFRedactorCore {

enum FileType {
	UnknowFile = -1,
	FoxBase,
	Fox3,
	Fox3M,
	Fox4,
	Fox,
	DBase3,
	DBase4,
	DBase5
};

class DBFHeader {

public:
	DBFHeader ();
	~DBFHeader ();

	bool load (std::iostream &stream);
	bool save (std::iostream &stream);

	void clear ();
	bool isEmpty () const;
	bool isValid () const;

	FileType fileType () const;

	time_t lastUpdated () const;
	void setLastUpdated (const time_t time);

	int32 recordsCount () const;
	void setRecordsCount (const long count);
	void addRecordsCount (const long count = 1);

	int16 firstRecordPos () const;

	int16 recordLenght () const;

	bool hasIndex () const;

	DBFField field (const int index) const;

	DBFFieldsList fields () const;

	int16 fieldPos (const int index) const;

	int8 fieldsCount () const;

private:
	DBFHeader (const DBFHeader &f);
	DBFHeader& operator= (const DBFHeader &f);

private:
	char m_data [33];
	FileType m_fileType;
	time_t m_lastUpdated;
	int32 m_recordsCount;
	int16 m_firstRecordPos;
	int16 m_recordLenght;
	bool m_hasIndex;
	DBFFieldsList m_fieldsList;
};
}

#endif //DBFHEADER_H
