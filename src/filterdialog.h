/********************************************************************
* Copyright (C) PanteR
*-------------------------------------------------------------------
*
* QDBFRedactor is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License as
* published by the Free Software Foundation; either version 2
* of the License, or (at your option) any later version.
*
* QDBFRedactor is distributed in the hope that it will be
* useful, but WITHOUT ANY WARRANTY; without even the implied
* warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
* See the GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with Panther Commander; if not, write to the Free Software
* Foundation, Inc., 51 Franklin St, Fifth Floor,
* Boston, MA 02110-1301 USA
*-------------------------------------------------------------------
* Project:		QDBFRedactor
* Author:		PanteR
* Contact:		panter.dsd@gmail.com
*******************************************************************/

#ifndef FILTERDIALOG_H
#define FILTERDIALOG_H

class QGroupBox;
class QTableView;
class QAction;
class QToolButton;
class QStandardItemModel;

#include <QtCore/QHash>
#include <QtGui/QDialog>

#include "dbfredactorsortfilterproxymodel.h"

class FilterDialog : public QDialog
{
	Q_OBJECT

private:
	QHash<int, QString> m_captions;
	QList<DBFRedactorSortFilterProxyModel::FilterItem> m_filter;

private:
	QTableView *filterView;
	QStandardItemModel *model;

	QAction *actionAdd;
	QAction *actionRemove;

	QToolButton *addButton;
	QToolButton *removeButton;

public:
	FilterDialog(QHash<int, QString> captions, QWidget *parent = 0, Qt::WindowFlags f = Qt::WindowSystemMenuHint);
	~FilterDialog()
	{saveSettings();}

	QList<DBFRedactorSortFilterProxyModel::FilterItem> filter() const;
	void setFilter(QList<DBFRedactorSortFilterProxyModel::FilterItem> filter);

protected:
	bool event(QEvent *ev);

private:
	void loadSettings();
	void saveSettings();
	void retranslateStrings();

private Q_SLOTS:
	void add();
	void remove();
};

#endif //SORTDIALOG_H
