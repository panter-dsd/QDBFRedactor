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

#ifndef STRUCTDIALOG_H
#define STRUCTDIALOG_H

class DBFRedactor;
class QTableWidget;

#include <QtGui/QDialog>

class StructDialog : public QDialog
{
	Q_OBJECT

private:
	enum Columns {
		Name = 0,
		Type,
		FirstLenght,
		SecondLenght,
		Count
	};

private:
	DBFRedactor*m_redactor;
	QTableWidget *table;

public:
	StructDialog(DBFRedactor *reader = 0, QWidget *parent = 0, Qt::WindowFlags f = Qt::WindowSystemMenuHint);
	~StructDialog()
	{saveSettings();}

private:
	void loadSettings();
	void saveSettings();
	void retranslateStrings();
	void readStruct ();

protected:
	bool event(QEvent *ev);

private Q_SLOTS:
};

#endif //STRUCTDIALOG_H
