#ifndef DBFREDACTORMODEL_H
#define DBFREDACTORMODEL_H

#include <QtGui/QStandardItemModel>

class DBFRedactorModel : public QStandardItemModel
{
	Q_OBJECT

public:
	DBFRedactorModel(QObject * parent = 0);
};

#endif //DBFREDACTORMODEL_H
