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

#include "dbffield.h"

namespace DBFRedactorCore {

DBFField::DBFField ()
	: data_ (0), type_ (Unknow),
	  firstLenght_ (-1), secondLenght_ (-1)
{

}

DBFField::DBFField (const char *data)
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
		if (data_) {
			delete [] data_;
			data_ = 0;
		}
		data_ = new char [33];
		memcpy (data_, f.data_, 32);
		name_ = f.name_;
		type_ = f.type_;
		firstLenght_ = f.firstLenght_;
		secondLenght_ = f.secondLenght_;
	}

	return *this;
}

DBFField::~DBFField ()
{
	if (data_) {
		delete [] data_;
		data_ = 0;
	}
}

void DBFField::clear ()
{
	if (data_) {
		delete [] data_;
		data_ = 0;
	}
	name_.clear ();
	type_ = Unknow;
	firstLenght_ = 	secondLenght_ = -1;
}

bool DBFField::isEmpty () const
{
	return !data_;
}

bool DBFField::isValid () const
{
	return !isEmpty ();
}

void DBFField::setData (const char *data)
{
	clear ();
	data_ = new char [33];
	memcpy (data_, data, 32);
}

std::string DBFField::name () const
{
	if (name_.empty () && isValid ()) {
		int i = 10;
		for (;i > 0 && data_ [i] == '\0'; --i) {}
		name_.assign (data_, i + 1);
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
