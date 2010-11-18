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
	: m_data (0), m_length (-1)
{

}

DBFRecord::DBFRecord (const char *data, int16 length)
	: m_data (0), m_length (length)
{
	setData (data, length);
}

DBFRecord::DBFRecord (const DBFRecord &f)
	: m_data (0), m_length (-1)
{
	*this = f;
}

DBFRecord& DBFRecord::operator= (const DBFRecord &f)
{
	if (this != &f) {
		setData (f.m_data, f.m_length);
	}

	return *this;
}

DBFRecord::~DBFRecord ()
{
	if (m_data) {
		delete [] m_data;
		m_data = 0;
	}
}

void DBFRecord::clear ()
{
	if (m_data) {
		delete [] m_data;
		m_data = 0;
	}
	m_length = -1;
}

bool DBFRecord::isEmpty () const
{
	return !m_data;
}

bool DBFRecord::isValid () const
{
	return !isEmpty ();
}

void DBFRecord::setData (const char *data, int16 length)
{
	clear ();
	m_length = length;
	m_data = new char [m_length + 1];
	memcpy (m_data, data, m_length);
}

}
