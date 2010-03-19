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

#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtCore/QCoreApplication>

#include <QtGui/QGroupBox>
#include <QtGui/QVBoxLayout>
#include <QtGui/QPushButton>
#include <QtGui/QtEvents>

#include "globalpreferences.h"
#include "translationmanager.h"

GlobalPreferences::GlobalPreferences(QWidget *parent)
	:AbstractPreferencesPage(parent)
{
	associationGroup = new QGroupBox (this);

	associationButton = new QPushButton (this);
	connect (associationButton, SIGNAL(clicked()), this, SLOT(doAssociation()));

	QHBoxLayout *associationLayout = new QHBoxLayout();
	associationLayout->addWidget(associationButton);
	associationLayout->addSpacerItem(new QSpacerItem (0, 0, QSizePolicy::Expanding, QSizePolicy::Preferred));
	associationGroup->setLayout(associationLayout);

	QVBoxLayout *mainLayout = new QVBoxLayout();
	mainLayout->addWidget(associationGroup);
	mainLayout->addSpacerItem(new QSpacerItem (0, 0, QSizePolicy::Preferred, QSizePolicy::Expanding));
	mainLayout->setContentsMargins(0, 0, 0, 0);
	setLayout(mainLayout);

	loadSettings();
	retranslateStrings();
}

void GlobalPreferences::retranslateStrings()
{
	associationGroup->setTitle(tr ("Association"));
	associationButton->setText(tr ("Do association"));

}

void GlobalPreferences::saveSettings()
{
	QSettings settings;

	settings.beginGroup("Global");

	settings.endGroup();
	settings.sync();
}

void GlobalPreferences::loadSettings()
{
	QSettings settings;

	settings.beginGroup("Global");


	settings.endGroup();
}

void GlobalPreferences::setDefaults()
{

}

bool GlobalPreferences::event(QEvent *ev)
{
	if (ev->type() == QEvent::LanguageChange) {
		retranslateStrings();
	}

	return QWidget::event(ev);
}

void GlobalPreferences::doAssociation ()
{
	QSettings settings ("HKEY_CLASSES_ROOT", QSettings::NativeFormat);

	settings.setValue (".dbf/.", "QDBFRedactor.file");
	settings.setValue ("QDBFRedactor.file/.", tr("File of database"));
	settings.setValue ("QDBFRedactor.file/shell/open/command/.",
					   QDir::toNativeSeparators (QCoreApplication::applicationFilePath()) + " %1");
}
