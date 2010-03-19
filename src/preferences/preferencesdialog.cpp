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

#include <QtGui/QTreeWidget>
#include <QtGui/QStackedWidget>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QPushButton>
#include <QtGui/QScrollArea>
#include <QtGui/QVBoxLayout>
#include <QtGui/QSplitter>
#include <QtGui/QHeaderView>
#include <QtGui/QtEvents>

#include "abstractpreferencespage.h"
#include "preferencesdialog.h"
#include "languagepreferences.h"
#include "displaypreferences.h"

PreferencesDialog::PreferencesDialog(QWidget * parent, Qt::WindowFlags f)
	: QDialog(parent,f)
{
	preferencesList = new QTreeWidget(this);
	preferencesList->setColumnCount(1);
	preferencesList->header()->hide();
	connect(preferencesList, SIGNAL(itemClicked(QTreeWidgetItem*,int)), this, SLOT(itemChanged(QTreeWidgetItem*,int)));

	preferencesWidgets = new QStackedWidget(this);
	preferencesWidgets->setContentsMargins(0,0,0,0);

	splitter = new QSplitter(this);
	splitter->setOrientation(Qt::Horizontal);
	splitter->addWidget(preferencesList);
	splitter->addWidget(preferencesWidgets);

	LanguagePreferences *globalPreferences = new LanguagePreferences(this);
	connect(globalPreferences, SIGNAL(modified()), this, SLOT(setApplyEnabled()));
	pages.insert(preferencesWidgets->addWidget(globalPreferences), globalPreferences);

	DisplayPreferences *displayPreferences = new DisplayPreferences(this);
	connect(displayPreferences, SIGNAL(modified()), this, SLOT(setApplyEnabled()));
	pages.insert(preferencesWidgets->addWidget(displayPreferences), displayPreferences);

	buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel | QDialogButtonBox::Apply,
								   Qt::Horizontal,
								   this);
	buttons->button(QDialogButtonBox::Apply)->setEnabled(false);
	connect(buttons, 	SIGNAL(accepted()), this, SLOT(savePreferencesAndExit()));
	connect(buttons, SIGNAL(rejected()), this, SLOT(reject()));
	connect(buttons->button(QDialogButtonBox::Apply), SIGNAL(clicked()), this, SLOT(savePreferences()));

	defaultsButton =new QPushButton(this);
	connect(defaultsButton, SIGNAL(clicked()), this, SLOT(setDefaults()));
	buttons->addButton(defaultsButton, QDialogButtonBox::ApplyRole);

	QVBoxLayout *mainLayout = new QVBoxLayout();
	mainLayout->addWidget(splitter);
	mainLayout->addWidget(buttons);
	setLayout(mainLayout);

	loadSettings();
	updatePreferencesList();
	retranslateStrings();
}

void PreferencesDialog::retranslateStrings()
{
	setWindowTitle(tr("Preferences"));

	defaultsButton->setText(tr("Default"));
}

bool PreferencesDialog::event(QEvent *ev)
{
	if (ev->type() == QEvent::LanguageChange) {
		retranslateStrings();
	}

	return QWidget::event(ev);
}

PreferencesDialog::~PreferencesDialog()
{
	saveSattings();
}

void PreferencesDialog::savePreferences()
{
	QMapIterator<int, QWidget*> it(pages);

	while(it.hasNext()) {
		it.next();

		AbstractPreferencesPage *page = qobject_cast<AbstractPreferencesPage*> (it.value());
		if (!page)
			continue;
		page->saveSettings();
	}
	buttons->button(QDialogButtonBox::Apply)->setEnabled(false);
}

void PreferencesDialog::setApplyEnabled()
{
	buttons->button(QDialogButtonBox::Apply)->setEnabled(true);
}

void PreferencesDialog::setDefaults()
{
}

void PreferencesDialog::loadSettings()
{
	QSettings settings;
	settings.beginGroup("PreferencesDialog");
	move(settings.value("pos", QPoint(0, 0)).toPoint());
	resize(settings.value("size", QSize(640, 480)).toSize());
	if(settings.value("IsMaximized", false).toBool())
		showMaximized();

	splitter->restoreState(settings.value("SplitterState", QByteArray()).toByteArray());
	settings.endGroup();
}

void PreferencesDialog::saveSattings()
{
	QSettings settings;
	settings.beginGroup("PreferencesDialog");
	settings.setValue("IsMaximized", isMaximized());
	if(!isMaximized()) {
		settings.setValue("pos", pos());
		settings.setValue("size", size());
	}

	settings.setValue("SplitterState", splitter->saveState());
	settings.endGroup();
	settings.sync();
}

void PreferencesDialog::updatePreferencesList()
{
	preferencesList->clear();

	QMapIterator<int, QWidget*> it(pages);

	while(it.hasNext()) {
		it.next();

		AbstractPreferencesPage *page = qobject_cast<AbstractPreferencesPage*> (it.value());
		if (!page)
			continue;

		QStringList l = page->preferenceGroup().split("/", QString::KeepEmptyParts);

		QList<QTreeWidgetItem *> items;
		QTreeWidgetItem *parent = 0;
		foreach(const QString& name, l) {
			QTreeWidgetItem *item = new QTreeWidgetItem(parent);
			item->setText(0, name);
			items.append(item);
			parent = item;
		}
		items.last()->setData(0, Qt::UserRole, it.key());
		preferencesList->addTopLevelItems(items);
	}
}

void PreferencesDialog::itemChanged(QTreeWidgetItem * item, int column)
{
	if (!item)
		return;

	bool ok = false;

	while (!ok) {
		int index = item->data(0, Qt::UserRole).toInt(&ok);

		if (ok)
			preferencesWidgets->setCurrentIndex(index);
		else
			item = item->child(0);
		if (!item)
			break;
	}
}
