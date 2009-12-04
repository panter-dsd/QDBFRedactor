#ifndef DBFREDACTORPAGE_H
#define DBFREDACTORPAGE_H

class DBFRedactorModel;
class QItemSelectionModel;

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QPoint>

#include <QtGui/QItemSelectionModel>

#include "dbfredactormodel.h"

class DBFRedactorPage : QObject
{

private:
	DBFRedactorModel *m_model;
	QItemSelectionModel* m_selectionModel;
	QString m_fileName;
	QPoint m_pos;

public:
	DBFRedactorPage(const QString& fileName, QWidget *parent = 0)
	 :QObject(parent), m_fileName(fileName), m_pos(0, 0)
	{
		m_model = new DBFRedactorModel(m_fileName, parent);

		m_selectionModel = new QItemSelectionModel(m_model);
	}

	~DBFRedactorPage()
	{
		delete m_selectionModel;
		delete m_model;
	}

	QString fileName() const
	{return m_fileName;}

	QItemSelectionModel* selectionModel() const
	{return m_selectionModel;}
	void setSelectionModel(QItemSelectionModel *selectionModel)
	{
		if (m_selectionModel)
			delete m_selectionModel;
		m_selectionModel = selectionModel;
	}

	DBFRedactorModel* model() const
	{return m_model;}
	void setModel(DBFRedactorModel *model)
	{
		if (m_model)
			delete m_model;
		m_model = model;
	}

	QPoint pos()
	{return m_pos;}
	void setPos(QPoint pos)
	{m_pos = pos;}
	void setPos(int x, int y)
	{m_pos.setX(x); m_pos.setY(y);}
};

#endif //DBFREDACTORPAGE_H
