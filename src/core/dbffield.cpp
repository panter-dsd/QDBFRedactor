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

#include <iostream>
#include <algorithm>

#include "dbffield.h"

const int nameLength = 10;

namespace DBFRedactorCore {

DBFField::DBFField ()
	: data_ (0)
{

}

DBFField::DBFField (const DBFFieldDataType& data)
	: data_ (0)
{
	setData (data);
}

DBFField::DBFField (const DBFField &f)
	: data_ (0)
{
	*this = f;
}

DBFField& DBFField::operator= (const DBFField &f)
{
	if (this != &f) {
		setData (f.data_);
	}

	return *this;
}

void DBFField::clear ()
{
	data_.clear();
}

bool DBFField::isEmpty () const
{
	return data_.empty ();
}

bool DBFField::isValid () const
{
	return !data_.empty ();
}

void DBFField::setData (const DBFFieldDataType& data)
{
	clear ();
	data_.assign (data.begin (), data.end ());
}

std::string DBFField::name () const
{
	if (!isValid ()) {
		return std::string ();
	}
	
	const DBFFieldDataType::const_iterator it = std::find (data_.begin (),
													   data_.begin () + nameLength,
													   '\0');

	return std::string (data_.begin (), it);
}

DBFType DBFField::type () const
{
	if (!isValid ()) {
		return Unknow;
	}
	
	switch (data_ [11]) {
		case 'C': return Char; break;
		case 'N': return Numeric; break;
		case 'L': return Logical; break;
		case 'D': return Date; break;
		case 'F': return Float; break;
		case 'M': return Memo; break;
	}
	
	return Unknow;
}

int8 DBFField::firstLenght () const
{
	return isValid () ? data_ [16] : 0;
}

int8 DBFField::secondLenght () const
{
	return isValid () ? data_ [17] : 0;
}
}
