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

#include <cstring>
#include <iostream>

#include "dbfrecord.h"

namespace DBFRedactorCore {

DBFRecord::DBFRecord ()
	: data_ (0), length_ (-1)
{

}

DBFRecord::DBFRecord (const DBFRecordDataType &data, int16 length)
	: data_ (0), length_ (length)
{
	setData (data, length);
}

DBFRecord::DBFRecord (const DBFRecord &f)
	: data_ (0), length_ (-1)
{
	*this = f;
}

DBFRecord& DBFRecord::operator= (const DBFRecord &f)
{
	if (this != &f) {
		setData (f.data_, f.length_);
	}

	return *this;
}

void DBFRecord::clear ()
{
	data_.clear ();
	length_ = -1;
}

bool DBFRecord::isEmpty () const
{
	return data_.empty ();
}

bool DBFRecord::isValid () const
{
	return !data_.empty ();
}

void DBFRecord::setData (const DBFRecordDataType &data, int16 length)
{
	clear ();
	length_ = length;
	data_.assign (data.begin (), data.end ());
}

}
