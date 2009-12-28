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

#ifndef DBFREDACTORDELEGATE_H
#define DBFREDACTORDELEGATE_H

#include <QtCore/QSettings>
#include <QtGui/QItemDelegate>
#include "dbfredactor.h"

class DBFRedactorDelegate : public QItemDelegate
{

private:
	DBFRedactor *m_redactor;
	QSettings m_settings;

public:
	DBFRedactorDelegate(DBFRedactor *redactor, QObject * parent = 0);

	void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
	QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;
	QWidget *createEditor ( QWidget * parent, const QStyleOptionViewItem & option, const QModelIndex & index ) const;
	void setEditorData(QWidget *editor, const QModelIndex &index) const;
	void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
	void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex & index ) const;

	QRect checkRect (const QStyleOptionViewItem& option, const QRect& bounding) const;
	bool editorEvent (QEvent* ev, QAbstractItemModel* model, const QStyleOptionViewItem& option,const QModelIndex& index);
};

#endif // DBFREDACTORDELEGATE_H
