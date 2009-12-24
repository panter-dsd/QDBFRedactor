#ifndef DBFREDACTORMODEL_H
#define DBFREDACTORMODEL_H

#include <QtCore/QAbstractItemModel>
#include <QtCore/QMap>

#include "dbfredactor.h"

class DBFRedactorModel : public QAbstractItemModel
{
	Q_OBJECT

private:
	QString m_fileName;
	bool m_showDeleted;
	DBFRedactor *redactor;

public:
	DBFRedactorModel(const QString& fileName, QObject * parent = 0);
	~DBFRedactorModel();

	QVariant data(const QModelIndex &index, int role) const;
	bool setData ( const QModelIndex & index, const QVariant & value, int role = Qt::EditRole );
	Qt::ItemFlags flags(const QModelIndex &index) const;
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
	QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
	QModelIndex parent(const QModelIndex &index) const;

	int rowCount(const QModelIndex &parent = QModelIndex()) const;
	int columnCount(const QModelIndex &parent = QModelIndex()) const;

	bool showDeleted()
	{return m_showDeleted;}
	void setShowDeleted(bool isShow)
	{m_showDeleted = isShow;}

	void refresh();

	DBFRedactor* dbfRedactor()
	{return redactor;}
};

#endif //DBFREDACTORMODEL_H
