#ifndef DBFFIELD_H
#define DBFFIELD_H

#include <string>

#ifdef unix
#include <stdint.h>
#endif //unix

namespace DBFRedactorCore {

#ifdef unix
typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
#endif //unix

#ifdef WIN32
typedef __int8 int8;
typedef __int16 int16;
typedef __int32 int32;
#endif //WIN32


enum DBFType {
	Unknow = -1,
	Char = 0,
	Numeric,
	Logical,
	Date,
	Float,
	Memo
};

class DBFField {
public:
	DBFField ();
	DBFField (const char *data);
	DBFField (const DBFField &f);

	DBFField& operator= (const DBFField &f);


	~DBFField ();

	void clear ();
	bool isEmpty () const;
	bool isValid () const;


	std::string data () const
	{return m_data;}

	void setData (const char *data);

	std::string name () const;

	DBFType type () const;

	int8 firstLenght () const;
	int8 secondLenght () const;


private:
	char *m_data;
	mutable std::string m_name;
	mutable DBFType m_type;
	mutable int8 m_firstLenght;
	mutable int8 m_secondLenght;
};
}

#endif //DBFFIELD_H
