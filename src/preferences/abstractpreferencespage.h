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

#ifndef ABSTRACTPREFERENCESPAGE_H
#define ABSTRACTPREFERENCESPAGE_H

#include <QtCore/QSettings>

#include <QtGui/QWidget>

class QString;

class AbstractPreferencesPage : public QWidget
{
	Q_OBJECT

public:
	AbstractPreferencesPage(QWidget *parent = 0)
	{}
	virtual ~AbstractPreferencesPage()
	{}

	virtual void saveSettings() = 0;
	virtual void loadSettings() = 0;
	virtual void setDefaults() = 0;

	virtual QString preferenceGroup()
	{return QString();}

Q_SIGNALS:
	void modified();
};

#endif // ABSTRACTPREFERENCESPAGE_H
