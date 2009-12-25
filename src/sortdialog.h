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

#ifndef SORTDIALOG_H
#define SORTDIALOG_H

class QListWidget;
class QLabel;
class QToolButton;

#include <QtCore/QHash>
#include <QtGui/QDialog>

class SortDialog : public QDialog
{
	Q_OBJECT

private:
	QList<QPair<int, Qt::SortOrder> > m_sortedColumns;
	QHash<int, QString> m_captions;

private:
	QLabel *columnsLabel;
	QLabel *sortedColumnsLabel;

	QListWidget *columnsList;
	QListWidget *sortedColumnsList;

	QToolButton *addAllButton;
	QToolButton *removeAllButton;
	QToolButton *addButton;
	QToolButton *removeButton;

	QToolButton *moveUpButton;
	QToolButton *moveDownButton;
	QToolButton *moveTopButton;
	QToolButton *moveBottomButton;
	QToolButton *changeSortOrderButton;

public:
	SortDialog(QHash<int, QString> captions, QWidget *parent = 0, Qt::WindowFlags f = Qt::WindowSystemMenuHint);
	~SortDialog()
	{saveSettings();}

	QList<QPair<int, Qt::SortOrder> > sortedColumns() const
	{return m_sortedColumns;}
	void setSortedColumns(QList<QPair<int, Qt::SortOrder> > sortedColumns);

private:
	void updateLists();
	void loadSettings();
	void saveSettings();
	void retranslateStrings();

protected:
	bool event(QEvent *ev);

private Q_SLOTS:
	void changeSortOrder();
	void addAll();
	void removeAll();
	void add();
	void remove();
	void moveUp();
	void moveDown();
	void moveTop();
	void moveBottom();
};

#endif //SORTDIALOG_H
