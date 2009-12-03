#include <QtCore/QDebug>

#include "dbfredactormodel.h"
#include "dbfredactor.h"

DBFRedactorModel::DBFRedactorModel(const QString& fileName, QObject *parent)
	:QAbstractItemModel(parent), m_fileName(fileName), m_showDeleted(true)
{
	redactor = new DBFRedactor(fileName);
	redactor->open(DBFRedactor::Read);
}

DBFRedactorModel::~DBFRedactorModel()
{
	redactor->close();
	delete redactor;
}

QVariant DBFRedactorModel::data(const QModelIndex &index, int role) const
{
	if (!index.isValid())
		return QVariant();

	switch (role) {
		case Qt::DisplayRole:
			return redactor->record(index.row()).value.at(index.column());
			break;
		case Qt::TextAlignmentRole:
			if ((redactor->field(index.column()).type == TYPE_NUMERIC)
				|| (redactor->field(index.column()).type == TYPE_FLOAT))
				return Qt::AlignRight;
			if (redactor->field(index.column()).type == TYPE_DATE)
				return Qt::AlignHCenter;
			return Qt::AlignLeft;
			break;
		case Qt::ForegroundRole:
			if ((redactor->field(index.column()).type == TYPE_NUMERIC)
				|| (redactor->field(index.column()).type == TYPE_FLOAT))
				return Qt::darkBlue;
			if (redactor->field(index.column()).type == TYPE_DATE)
				return Qt::darkGreen;
			return Qt::black;
			break;
		case Qt::BackgroundRole:
			if (redactor->record(index.row()).isDeleted)
				return Qt::lightGray;
			break;
		}

	return QVariant();
}

Qt::ItemFlags DBFRedactorModel::flags(const QModelIndex &index) const
{
	return index.isValid() ? Qt::ItemIsSelectable | Qt::ItemIsEnabled : Qt::NoItemFlags;
}

QVariant DBFRedactorModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if(role == Qt::DisplayRole) {
		if(orientation == Qt::Horizontal) {
			return redactor->field(section).name;
		} else {
			return section + 1;
		}
	}
	return QVariant();
}

QModelIndex DBFRedactorModel::index(int row, int column, const QModelIndex &parent) const
{
	if (!parent.isValid())
		return createIndex(row, column);
}

QModelIndex DBFRedactorModel::parent(const QModelIndex &index) const
{
	return QModelIndex();
}

int DBFRedactorModel::rowCount(const QModelIndex &parent) const
{
	return redactor->rowsCount();
}

int DBFRedactorModel::columnCount(const QModelIndex &parent) const
{
	return redactor->columnsCount();
}

void DBFRedactorModel::refresh()
{
	redactor->refresh();
	reset();
}
