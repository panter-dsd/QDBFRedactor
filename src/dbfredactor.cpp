#include "dbfredactor.h"
#define MAX_BUFFER_SIZE 10000
//
DBFRedactor::DBFRedactor()
{
	header.count=-1;
	qsFileName="";
}
//
DBFRedactor::DBFRedactor(const QString& fileName)
{
	qsFileName=fileName;
	qsTableName=QFileInfo(qsFileName).baseName();
}
//
DBFRedactor::~DBFRedactor()
{
	close();
}
//
void DBFRedactor::close()
{
	header.fieldsList.clear();
	qfFile.close();
	qhBuffer.clear();
}
//
bool DBFRedactor::open(DBFOpenMode OpenMode)
{
	if (qsFileName.isEmpty())
		return false;
	return open(OpenMode, qsFileName);
}
//
bool DBFRedactor::open(DBFOpenMode OpenMode, const QString& fileName)
{
	if (!fileName.isEmpty())
		qsFileName=fileName;
	if (qsFileName.isEmpty())
		return false;
	qsTableName=QFileInfo(qsFileName).baseName();
	qfFile.setFileName(qsFileName);
	QIODevice::OpenMode openMode;
	if (OpenMode == DBFRedactor::Read)
		openMode = QIODevice::ReadOnly;
	if (OpenMode == DBFRedactor::Write)
		openMode = QIODevice::Append;

	if (!qfFile.open(openMode))
	{
		qDebug("Error open file");
		return false;
	}
	buf=qfFile.read(32);
	bool ok;
#warning "Type is not true."
	header.type=buf.at(0);
	header.count=revert(buf.mid(4,4)).toHex().toLong(&ok,16);
	header.pos=revert(buf.mid(8,2)).toHex().toInt(&ok,16);
	header.lenghtRecord=revert(buf.mid(10,2)).toHex().toInt(&ok,16);
	header.isIndex=(buf.at(28)==1);

	if (buf.at(29)==-55)
		codec=QTextCodec::codecForName("CP1251");
	else
		codec=QTextCodec::codecForName("IBM866");
	buf=qfFile.read(32);
	do
	{
		SField field;
		field.qsName=codec->toUnicode(buf.left(10));
		field.qsName.remove(field.qsName.indexOf(QChar(0)),10);
		switch (buf.at(11))
		{
			case 'C':{field.type=TYPE_CHAR; break;}
			case 'N':{field.type=TYPE_NUMERIC; break;}
			case 'L':{field.type=TYPE_LOGICAL; break;}
			case 'M':{field.type=TYPE_MEMO; break;}
			case 'D':{field.type=TYPE_DATE; break;}
			case 'F':{field.type=TYPE_FLOAT; break;}
			case 'P':{field.type=TYPE_P; break;}
		}
		field.pos=revert(buf.mid(12,4)).toHex().toLong(&ok,16);
		field.firstLenght=buf.mid(16,1).toHex().toInt(&ok,16);
		field.secondLenght=buf.mid(17,1).toHex().toInt(&ok,16);
		if ((field.type==TYPE_NUMERIC) && (field.firstLenght>18))
		{
			field.type=TYPE_FLOAT;
		}
		header.fieldsList << field;
		buf=qfFile.read(32);
	}
	while (buf.at(0)!=13);
	qfFile.seek(header.pos);
	buf.clear();
	return true;
}
//
QByteArray DBFRedactor::revert(const QByteArray& array)
{
	QByteArray qbaArray;
	for (int i=array.length()-1; i>=0; i--)
	{
		qbaArray.append(array.at(i));
	}
	return qbaArray;
}
//
SField DBFRedactor::field(int number)
{
	if (number<header.fieldsList.count())
		return header.fieldsList.at(number);
	else
		return SField();
}
//
QByteArray DBFRedactor::strRecord(int number)
{
	if ((number>=header.count) || (!qfFile.isOpen()))
		return QByteArray();
	if (qhBuffer.contains(number))
		buf=qhBuffer.value(number);
	else
	{
		if (lastRecord!=number-1)
		{
			qfFile.seek(header.pos+header.lenghtRecord*number);
		}
		buf=qfFile.read(header.lenghtRecord);
		lastRecord=number;
		qhBuffer.insert(number,buf);
		if (qhBuffer.size()>MAX_BUFFER_SIZE)
			qhBuffer.erase(qhBuffer.begin());
	}
	return buf;
}
//
SRecord* DBFRedactor::record(int number)
{
	if ((number>=header.count) || (!qfFile.isOpen()))
		return 0;
	if (qhBuffer.contains(number))
	{
		buf=qhBuffer.value(number);
	}
	else
	{
		if (lastRecord!=number-1)
		{
			qfFile.seek(header.pos+header.lenghtRecord*number);
		}
		buf=qfFile.read(header.lenghtRecord);
		lastRecord=number;
		qhBuffer.insert(number,buf);
		if (qhBuffer.size()>MAX_BUFFER_SIZE)
			qhBuffer.erase(qhBuffer.begin());
	}

	SRecord* szRecord=new SRecord();
	QString recordString=codec->toUnicode(buf);
	QString qstemp;
	int pos=1;
	for (int i=0; i<header.fieldsList.count();i++)
	{
		qstemp=recordString.mid(pos,header.fieldsList.at(i).firstLenght);
		pos+=header.fieldsList.at(i).firstLenght;
		//Delete last spaces
		if (!qstemp.isEmpty())
		{
			int i=qstemp.length();
			while ((--i>=0) && qstemp[i].isSpace())	 {;}
			qstemp.remove(i+1,qstemp.length());
		}
		//
		switch (header.fieldsList.at(i).type)
		{
			case TYPE_CHAR: {szRecord->value.append(QVariant(qstemp));break;}
			case TYPE_NUMERIC: {szRecord->value.append(QVariant(qstemp.toDouble()));break;}
			case TYPE_LOGICAL:
			{
				if (qstemp.toInt()==0)
					szRecord->value.append("FALSE");
				else
					szRecord->value.append("TRUE");
				break;
			}
			case TYPE_DATE: {szRecord->value.append(QVariant(QDate::fromString(qstemp,"yyyyMMdd")));break;}
			case TYPE_FLOAT: {szRecord->value.append(QVariant(qstemp.toDouble()));break;}
			case TYPE_MEMO: {szRecord->value.append(QVariant());break;}
		}
	}
	if (buf.at(0)==0x2A)
		szRecord->isDeleted=true;
	else
		szRecord->isDeleted=false;
	lastRecord=number;
	buf.clear();
	return szRecord;
}
//
bool DBFRedactor::compareRecord(SRecord* qzFirst,SRecord* qzSecond)
{
	if (qzFirst->isDeleted!=qzSecond->isDeleted)
		return false;
	if (qzFirst->value.count()!=qzSecond->value.count())
		return false;
	for (int i=0; i<qzFirst->value.count(); i++)
	{
		if (qzFirst->value.at(i)!=qzSecond->value.at(i))
			return false;
	}
	return true;
}
//
bool DBFRedactor::isOpen()
{
	return qfFile.isOpen();
}
//
int DBFRedactor::deletedCount()
{
	qint64 pos=qfFile.pos();
	qfFile.seek(header.pos);
	int delCount=0;
	for (int i=0; i<header.count; i++)
	{
		buf=qfFile.read(header.lenghtRecord);
		if (buf.at(0)==0x2A)
			delCount++;
	}
	qfFile.seek(pos);
	return delCount;
}
//
int DBFRedactor::recordsCount()
{
	if (qfFile.isOpen())
		return header.count;
	else
		return 0;
}
//

