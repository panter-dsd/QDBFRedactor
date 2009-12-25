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

#ifndef PREFERENCESDIALOG_H
#define PREFERENCESDIALOG_H

class QTreeWidget;
class QTreeWidgetItem;
class QStackedWidget;
class QDialogButtonBox;
class QPushButton;
class QSplitter;

#include <QtCore/QMap>
#include <QtGui/QDialog>

class PreferencesDialog : public QDialog
{
	Q_OBJECT

private:
	QTreeWidget *preferencesList;
	QStackedWidget *preferencesWidgets;
	QSplitter *splitter;
	QDialogButtonBox *buttons;
	QPushButton *defaultsButton;
	QMap<int, QWidget*> pages;

private:
	void loadSettings();
	void saveSattings();
	void retranslateStrings();
	void updatePreferencesList();

public:
	PreferencesDialog(QWidget * parent = 0, Qt::WindowFlags f = Qt::WindowSystemMenuHint | Qt::WindowMaximizeButtonHint);
	~PreferencesDialog();

private Q_SLOTS:
	void savePreferences();
	void savePreferencesAndExit()
	{
		savePreferences();
		close();
	}
	void setApplyEnabled();
	void setDefaults();

	void itemChanged(QTreeWidgetItem * item, int column);
};
#endif //PREFERENCESDIALOG_H
