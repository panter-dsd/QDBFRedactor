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

#ifndef DISPLAYPREFERENCES_H
#define DISPLAYPREFERENCES_H

class QGroupBox;
class QLabel;
class QComboBox;
class QToolButton;
//namespace Utils {
	class QtColorButton;
//}
//using namespace Utils;
#include "abstractpreferencespage.h"

class DisplayPreferences : public AbstractPreferencesPage
{
	Q_OBJECT

private:
	enum Alignment {
		Left = 0,
		Center,
		Right
	};

private:
	QGroupBox *delegatePreferencesGroup;

	QLabel *cellType;
	QLabel *cellAlignment;
	QLabel *cellColor;
	QLabel *cellFont;

	QLabel *stringColorLabel;
	QComboBox *stringAligmentEdit;
	QtColorButton *stringColorButton;
	QToolButton *stringFontButton;

	QLabel *numericColorLabel;
	QComboBox *numericAligmentEdit;
	QtColorButton *numericColorButton;
	QToolButton *numericFontButton;

	QLabel *memoColorLabel;
	QComboBox *memoAligmentEdit;
	QtColorButton *memoColorButton;
	QToolButton *memoFontButton;

	QLabel *dateColorLabel;
	QComboBox *dateAligmentEdit;
	QtColorButton *dateColorButton;
	QToolButton *dateFontButton;

	QLabel *floatColorLabel;
	QComboBox *floatAligmentEdit;
	QtColorButton *floatColorButton;
	QToolButton *floatFontButton;

public:
	DisplayPreferences(QWidget *parent = 0);
	virtual ~DisplayPreferences()
	{}

	void saveSettings();
	void loadSettings();
	void setDefaults();

	QString preferenceGroup()
	{ return tr("Display/Table"); }

protected:
	bool event(QEvent *ev);

private:
	void retranslateStrings();

private Q_SLOTS:
//	void setButtonColor();
	void setButtonFont();

};

#endif // DISPLAYPREFERENCES_H
