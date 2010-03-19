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

#ifndef GLOBALPREFERENCES_H
#define GLOBALPREFERENCES_H

class QGroupBox;
class QPushButton;
class QLabel;

#include "abstractpreferencespage.h"

class GlobalPreferences : public AbstractPreferencesPage
{
	Q_OBJECT

private:
	QGroupBox *associationGroup;
	QLabel *currentAssociationLabel;
	QLabel *currentAssociation;
	QPushButton *associationButton;

	QString m_currentAssociation;

public:
	GlobalPreferences(QWidget *parent = 0);
	virtual ~GlobalPreferences()
	{}

	void saveSettings();
	void loadSettings();
	void setDefaults();

	QString preferenceGroup()
	{ return tr("Global/System"); }

protected:
	bool event(QEvent *ev);

private:
	void retranslateStrings();
	void updateCurrentAssociation ();

private Q_SLOTS:
	void makeAssociation ();
};

#endif // GLOBALPREFERENCES_H
