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

#ifndef DBFFIELD_H
#define DBFFIELD_H

#include <string>
#include <vector>

#include "dbfredactorcore.h"

namespace DBFRedactorCore {

enum DBFType {
	Unknow = -1,
	Char = 0,
	Numeric,
	Logical,
	Date,
	Float,
	Memo
};

typedef std::vector <char> DBFFieldDataType;

class DBFField {
public:
	DBFField ();
	explicit DBFField (const DBFFieldDataType &data);
	DBFField (const DBFField &f);

	DBFField& operator= (const DBFField &f);

	~DBFField ()
	{}

	void clear ();
	bool isEmpty () const;
	bool isValid () const;

	DBFFieldDataType data () const
	{return data_;}

	void setData (const DBFFieldDataType &data);

	std::string name () const;

	DBFType type () const;

	int8 firstLenght () const;
	int8 secondLenght () const;

private:
	DBFFieldDataType data_;
};

typedef std::vector <DBFField> DBFFieldsList;
}

#endif //DBFFIELD_H
