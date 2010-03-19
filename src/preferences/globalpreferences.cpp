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
#include <QtGui/QLabel>
#include <QtGui/QCheckBox>

#include "globalpreferences.h"
#include "translationmanager.h"

GlobalPreferences::GlobalPreferences(QWidget *parent)
	:AbstractPreferencesPage(parent)
{
//Association
	associationGroup = new QGroupBox (this);

	currentAssociationLabel = new QLabel (this);

	currentAssociation = new QLabel (this);
	currentAssociation->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

	associationButton = new QPushButton (this);
	connect (associationButton, SIGNAL(clicked()), this, SLOT(makeAssociation()));

	QHBoxLayout *associationLabelsLayout = new QHBoxLayout ();
	associationLabelsLayout->addWidget(currentAssociationLabel);
	associationLabelsLayout->addWidget(currentAssociation);

	QHBoxLayout *associationButtonLayout = new QHBoxLayout();
	associationButtonLayout->addWidget(associationButton);
	associationButtonLayout->addSpacerItem(new QSpacerItem (0, 0, QSizePolicy::Expanding, QSizePolicy::Preferred));

	QVBoxLayout *associationLayout = new QVBoxLayout ();
	associationLayout->addLayout(associationLabelsLayout);
	associationLayout->addLayout(associationButtonLayout);
	associationGroup->setLayout(associationLayout);
//Run
	runGroup = new QGroupBox (this);

	onlyOneCopy = new QCheckBox (this);
	connect (onlyOneCopy, SIGNAL(stateChanged(int)), this, SIGNAL(modified()));

	restoreWindowParams = new QCheckBox (this);
	connect (restoreWindowParams, SIGNAL(stateChanged(int)), this, SIGNAL(modified()));

	QVBoxLayout *runLayout = new QVBoxLayout ();
	runLayout->addWidget(onlyOneCopy);
	runLayout->addWidget(restoreWindowParams);
	runGroup->setLayout(runLayout);
//Tray
	trayGroup = new QGroupBox (this);

	alwaysShowTray = new QCheckBox (this);
	connect (alwaysShowTray, SIGNAL(stateChanged(int)), this, SIGNAL(modified()));

	moveToTrayWhenClose = new QCheckBox (this);
	connect (moveToTrayWhenClose, SIGNAL(stateChanged(int)), this, SIGNAL(modified()));

	QVBoxLayout *trayLayout = new QVBoxLayout ();
	trayLayout->addWidget(alwaysShowTray);
	trayLayout->addWidget(moveToTrayWhenClose);
	trayGroup->setLayout(trayLayout);

	QVBoxLayout *mainLayout = new QVBoxLayout();
	mainLayout->addWidget(associationGroup);
	mainLayout->addWidget(runGroup);
	mainLayout->addWidget(trayGroup);
	mainLayout->addSpacerItem(new QSpacerItem (0, 0, QSizePolicy::Preferred, QSizePolicy::Expanding));
	mainLayout->setContentsMargins(0, 0, 0, 0);
	setLayout(mainLayout);

	loadSettings();
	retranslateStrings();
}

void GlobalPreferences::retranslateStrings()
{
	associationGroup->setTitle(tr ("Association"));
	currentAssociationLabel->setText(tr ("Current association") + ":");
	associationButton->setText(tr ("Make association"));

	runGroup->setTitle(tr ("Run"));
	onlyOneCopy->setText(tr ("Allow only one copy"));
	restoreWindowParams->setText (tr ("Restore window position and size"));

	trayGroup->setTitle(tr ("Tray icon"));
	alwaysShowTray->setText(tr ("Always show tray icon"));
	moveToTrayWhenClose->setText(tr ("Move to tray when close"));
}

void GlobalPreferences::saveSettings()
{
	QSettings settings;

	settings.beginGroup("Global");
	settings.setValue("OnlyOneCopy", onlyOneCopy->isChecked());
	settings.setValue("RestoreWindowParams", restoreWindowParams->isChecked());
	settings.setValue("TrayAlwaysShow", alwaysShowTray->isChecked());
	settings.setValue("MoveToTrayWhenClose", moveToTrayWhenClose->isChecked());
	settings.endGroup();
	settings.sync();
}

void GlobalPreferences::loadSettings()
{
	QSettings settings;

	settings.beginGroup("Global");
	onlyOneCopy->setChecked(settings.value("OnlyOneCopy", true).toBool());
	restoreWindowParams->setChecked(settings.value("RestoreWindowParams", true).toBool());
	alwaysShowTray->setChecked(settings.value("TrayAlwaysShow", true).toBool());
	moveToTrayWhenClose->setChecked(settings.value("MoveToTrayWhenClose", true).toBool());

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

	if (ev->type() == QEvent::Resize) {
		currentAssociation->setText(currentAssociation->fontMetrics().elidedText(m_currentAssociation, Qt::ElideMiddle, currentAssociation->width()));
	}

	if (ev->type() == QEvent::Show) {
		updateCurrentAssociation ();
	}

	return QWidget::event(ev);
}

void GlobalPreferences::makeAssociation ()
{
#ifdef Q_WS_WIN
	QSettings settings ("HKEY_CLASSES_ROOT", QSettings::NativeFormat);

	settings.setValue (".dbf/.", "QDBFRedactor.file");
	settings.setValue ("QDBFRedactor.file/.", tr("File of database"));
	settings.setValue ("QDBFRedactor.file/shell/open/command/.",
					   "\"" + QDir::toNativeSeparators (QCoreApplication::applicationFilePath()) + "\"" + " \"%1\"");
#endif
	updateCurrentAssociation ();
}

void GlobalPreferences::updateCurrentAssociation ()
{
#ifdef Q_WS_WIN
	QSettings settings ("HKEY_CLASSES_ROOT", QSettings::NativeFormat);

	const QString &cur = settings.value (".dbf/.", "").toString();

	m_currentAssociation = settings.value (cur + "/shell/open/command/.", "").toString();
#endif
	currentAssociation->setText(currentAssociation->fontMetrics().elidedText(m_currentAssociation, Qt::ElideMiddle, currentAssociation->width()));
	currentAssociation->setToolTip(m_currentAssociation);
}
