#include <cstring>
#include <iostream>

#include "dbffield.h"

namespace DBFRedactorCore {

DBFField::DBFField ()
	: m_data (0), m_type (Unknow),
	  m_firstLenght (-1), m_secondLenght (-1)
{

}

DBFField::DBFField (const char *data)
	: m_data (0), m_type (Unknow),
	  m_firstLenght (-1), m_secondLenght (-1)
{
	setData (data);
}

DBFField::DBFField (const DBFField &f)
	: m_data (0), m_type (Unknow),
	  m_firstLenght (-1), m_secondLenght (-1)
{
	*this = f;
}

DBFField& DBFField::operator= (const DBFField &f)
{
	if (this != &f) {
		if (m_data) {
			delete [] m_data;
			m_data = 0;
		}
		m_data = new char [33];
		memcpy (m_data, f.m_data, 32);
		m_name = f.m_name;
		m_type = f.m_type;
		m_firstLenght = f.m_firstLenght;
		m_secondLenght = f.m_secondLenght;
	}

	return *this;
}

DBFField::~DBFField ()
{
	if (m_data) {
		delete [] m_data;
		m_data = 0;
	}
}

void DBFField::clear ()
{
	if (m_data) {
		delete [] m_data;
		m_data = 0;
	}
	m_name.clear ();
	m_type = Unknow;
	m_firstLenght = -1;
	m_secondLenght = -1;
}

bool DBFField::isEmpty () const
{
	return !m_data;
}

bool DBFField::isValid () const
{
	return !isEmpty ();
}

void DBFField::setData (const char *data)
{
	clear ();
	m_data = new char [33];
	memcpy (m_data, data, 32);
}

std::string DBFField::name () const
{
	if (m_name.empty ()) {
		int i = 10;
		for (;i > 0 && m_data [i] == '\0'; --i) {}
		m_name.assign (m_data, i + 1);
	}

	return m_name;
}

DBFType DBFField::type () const
{
	if (m_type == Unknow) {
		if (isValid ()) {
			switch (m_data [11]) {
			case 'C': m_type = Char; break;
			case 'N': m_type = Numeric; break;
			case 'L': m_type = Logical; break;
			case 'D': m_type = Date; break;
			case 'F': m_type = Float; break;
			case 'M': m_type = Memo; break;
			}
		}
	}

	return m_type;
}

int8 DBFField::firstLenght () const
{
	if (m_firstLenght < 0) {
		if (isValid ()) {
			m_firstLenght = m_data [16];
		}
	}

	return m_firstLenght;
}

int8 DBFField::secondLenght () const
{
	if (m_secondLenght < 0) {
		if (isValid ()) {
			m_secondLenght = m_data [17];
		}
	}

	return m_secondLenght;
}
}
