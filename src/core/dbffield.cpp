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
	: data_ (0), type_ (Unknow),
	  firstLenght_ (-1), secondLenght_ (-1)
{

}

DBFField::DBFField (const DBFFieldDataType& data)
	: data_ (0), type_ (Unknow),
	  firstLenght_ (-1), secondLenght_ (-1)
{
	setData (data);
}

DBFField::DBFField (const DBFField &f)
	: data_ (0), type_ (Unknow),
	  firstLenght_ (-1), secondLenght_ (-1)
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
	name_.clear ();
	type_ = Unknow;
	firstLenght_ = secondLenght_ = -1;
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
	if (name_.empty () && isValid ()) {
		const DBFFieldDataType::const_iterator it = std::find (data_.begin (),
													   data_.begin () + nameLength,
													   '\0');

		name_.assign (data_.begin (), it);
	}

	return name_;
}

DBFType DBFField::type () const
{
	if (type_ == Unknow && isValid ()) {
		switch (data_ [11]) {
		case 'C': type_ = Char; break;
		case 'N': type_ = Numeric; break;
		case 'L': type_ = Logical; break;
		case 'D': type_ = Date; break;
		case 'F': type_ = Float; break;
		case 'M': type_ = Memo; break;
		}
	}

	return type_;
}

int8 DBFField::firstLenght () const
{
	if (firstLenght_ < 0 && isValid ()) {
		firstLenght_ = data_ [16];
	}

	return firstLenght_;
}

int8 DBFField::secondLenght () const
{
	if (secondLenght_ < 0 && isValid ()) {
		secondLenght_ = data_ [17];
	}

	return secondLenght_;
}
}
