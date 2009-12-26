#ifndef DBFREDACTORPAGE_H
#define DBFREDACTORPAGE_H

class DBFRedactorModel;
class QItemSelectionModel;

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QPoint>

#include <QtGui/QItemSelectionModel>


#include "dbfredactormodel.h"
#include "dbfredactorsortfilterproxymodel.h"
#include "dbfredactordelegate.h"

class DBFRedactorPage : QObject
{

private:
	DBFRedactorModel *m_model;
	QItemSelectionModel* m_selectionModel;
	DBFRedactorSortFilterProxyModel *m_sortModel;
	DBFRedactorDelegate *m_delegate;
	QString m_fileName;
	QPoint m_pos;
	QList<int> m_columnSizes;

public:
	DBFRedactorPage(const QString& fileName, QWidget *parent = 0)
	 :QObject(parent), m_fileName(fileName), m_pos(0, 0)
	{
		m_model = new DBFRedactorModel(m_fileName, parent);

		m_sortModel = new DBFRedactorSortFilterProxyModel(parent);
		m_sortModel->setSourceModel(m_model);

		m_selectionModel = new QItemSelectionModel(m_sortModel);

		m_delegate = new DBFRedactorDelegate(m_model->dbfRedactor(), parent);
	}

	~DBFRedactorPage()
	{
		delete m_selectionModel;
		delete m_model;
		delete m_sortModel;
		delete m_delegate;
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

	DBFRedactorSortFilterProxyModel* model() const
	{return m_sortModel;}
	void setModel(DBFRedactorModel *model)
	{
		if (m_model)
			delete m_model;
		m_model = model;
	}

	DBFRedactor *redactor()
	{return m_model->dbfRedactor();}

	DBFRedactorDelegate *delegate()
	{return m_delegate;}

	QPoint pos()
	{return m_pos;}
	void setPos(QPoint pos)
	{m_pos = pos;}
	void setPos(int x, int y)
	{m_pos.setX(x); m_pos.setY(y);}

	void refresh()
	{m_model->refresh();}

	QList<int> columnSizes()
	{return m_columnSizes;}
	void setColumnSizes(QList<int> sizes)
	{m_columnSizes = sizes;}

	DBFRedactorModel *dbfModel()
	{return m_model;}
};

#endif //DBFREDACTORPAGE_H
