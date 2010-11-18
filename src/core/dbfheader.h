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
	DBFHeader (const DBFHeader &f);

	~DBFHeader ();

	DBFHeader& operator= (const DBFHeader &f);

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

	std::vector <DBFField> fields () const;

	int16 fieldPos (const int index) const;

	int8 fieldsCount () const;

private:
	char m_data [33];
	FileType m_fileType;
	time_t m_lastUpdated;
	int32 m_recordsCount;
	int16 m_firstRecordPos;
	int16 m_recordLenght;
	bool m_hasIndex;
	std::vector <DBFField> m_fieldsList;
};
}

#endif //DBFHEADER_H
