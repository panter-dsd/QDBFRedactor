#include <QtCore/QDebug>

#include "dbfredactormodel.h"

DBFRedactorModel::DBFRedactorModel(const QString& fileName, QObject *parent)
	:QAbstractItemModel(parent), m_fileName(fileName), m_showDeleted(true)
{
	redactor = new DBFRedactor(fileName);
	redactor->open(DBFRedactor::Read);
	redactor->setBuffering(true);
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

	const QVariant& value = redactor->data(index.row(), index.column());

	switch (role) {
		case Qt::DisplayRole:
			return value;
			break;
		case Qt::TextAlignmentRole:
			if ((redactor->field(index.column()).type == DBFRedactor::TYPE_NUMERIC)
				|| (redactor->field(index.column()).type == DBFRedactor::TYPE_FLOAT))
				return Qt::AlignRight;
			if (redactor->field(index.column()).type == DBFRedactor::TYPE_DATE)
				return Qt::AlignHCenter;
			if (redactor->field(index.column()).type == DBFRedactor::TYPE_LOGICAL)
				return Qt::AlignHCenter;
			return Qt::AlignLeft;
			break;
		case Qt::ForegroundRole:
			if ((redactor->field(index.column()).type == DBFRedactor::TYPE_NUMERIC)
				|| (redactor->field(index.column()).type == DBFRedactor::TYPE_FLOAT))
				return Qt::darkBlue;
			if (redactor->field(index.column()).type == DBFRedactor::TYPE_DATE)
				return Qt::darkGreen;
			return Qt::black;
			break;
		case Qt::BackgroundRole:
			if (redactor->isDeleted(index.row()))
				return Qt::lightGray;
			break;
		case Qt::CheckStateRole:
			if (redactor->field(index.column()).type == DBFRedactor::TYPE_LOGICAL)
				return value.toBool();
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
	if(role == Qt::DisplayRole || role == Qt::EditRole) {
		if(orientation == Qt::Horizontal) {
			return redactor->field(section).name;
		} else {
			return section + 1;
		}
	}
	if(role == Qt::TextAlignmentRole) {
		return Qt::AlignCenter;
	}
	return QVariant();
}

QModelIndex DBFRedactorModel::index(int row, int column, const QModelIndex &parent) const
{
	if (!parent.isValid())
		return createIndex(row, column);
}

QModelIndex DBFRedactorModel::parent(const QModelIndex &/*index*/) const
{
	return QModelIndex();
}

int DBFRedactorModel::rowCount(const QModelIndex &/*parent*/) const
{
	return redactor->rowsCount();
}

int DBFRedactorModel::columnCount(const QModelIndex &/*parent*/) const
{
	return redactor->columnsCount();
}

void DBFRedactorModel::refresh()
{
	redactor->refresh();
	reset();
}
