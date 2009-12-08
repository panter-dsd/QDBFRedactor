#ifndef DBFREDACTORPAGE_H
#define DBFREDACTORPAGE_H

class DBFRedactorModel;
class QItemSelectionModel;

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QPoint>

#include <QtGui/QItemSelectionModel>
#include <QtGui/QSortFilterProxyModel>

#include "dbfredactormodel.h"

class DBFRedactorPage : QObject
{

private:
	DBFRedactorModel *m_model;
	QItemSelectionModel* m_selectionModel;
	QSortFilterProxyModel *m_sorModel;
	QString m_fileName;
	QPoint m_pos;

public:
	DBFRedactorPage(const QString& fileName, QWidget *parent = 0)
	 :QObject(parent), m_fileName(fileName), m_pos(0, 0)
	{
		m_model = new DBFRedactorModel(m_fileName, parent);

		m_sorModel = new QSortFilterProxyModel(parent);
		m_sorModel->setSourceModel(m_model);

		m_selectionModel = new QItemSelectionModel(m_sorModel);
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

	QSortFilterProxyModel* model() const
	{return m_sorModel;}
	void setModel(DBFRedactorModel *model)
	{
		if (m_model)
			delete m_model;
		m_model = model;
	}

	DBFRedactor *redactor()
	{return m_model->dbfRedactor();}

	QPoint pos()
	{return m_pos;}
	void setPos(QPoint pos)
	{m_pos = pos;}
	void setPos(int x, int y)
	{m_pos.setX(x); m_pos.setY(y);}

	void refresh()
	{m_model->refresh();}
};

#endif //DBFREDACTORPAGE_H
