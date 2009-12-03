#ifndef DBFREDACTORMODEL_H
#define DBFREDACTORMODEL_H

class DBFRedactor;

#include <QtCore/QAbstractItemModel>

class DBFRedactorModel : public QAbstractItemModel
{
	Q_OBJECT

private:
	QString m_fileName;
	DBFRedactor *redactor;

public:
	DBFRedactorModel(const QString& fileName, QObject * parent = 0);
	~DBFRedactorModel();

	QVariant data(const QModelIndex &index, int role) const;
	Qt::ItemFlags flags(const QModelIndex &index) const;
	QVariant headerData(int section, Qt::Orientation orientation,
						int role = Qt::DisplayRole) const;
	QModelIndex index(int row, int column,
					  const QModelIndex &parent = QModelIndex()) const;
	QModelIndex parent(const QModelIndex &index) const;
	int rowCount(const QModelIndex &parent = QModelIndex()) const;
	int columnCount(const QModelIndex &parent = QModelIndex()) const;
};

#endif //DBFREDACTORMODEL_H
