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
class QListWidget;
class QLabel;
class QLineEdit;
class QToolButton;

#include "abstractpreferencespage.h"

class GlobalPreferences : public AbstractPreferencesPage
{
	Q_OBJECT

private:
	QGroupBox *translationGroup;
	QLabel *tranlationsPathLabel;
	QLineEdit *tranlationsPathEdit;
	QToolButton *tranlationsPathButton;
	QListWidget *translationsList;

public:
	GlobalPreferences(QWidget *parent = 0);
	~GlobalPreferences()
	{}

	void saveSettings();
	void loadSettings();
	void setDefaults();

	QString preferenceGroup()
	{ return tr("Global/Language/Ru"); }

protected:
	bool event(QEvent *ev);

private:
	void retranslateStrings();

private Q_SLOTS:
	void updateTranslationsList();
	void setTranslationsPath();
};

#endif // GLOBALPREFERENCES_H
