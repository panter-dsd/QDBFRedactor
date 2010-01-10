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

#include <QtCore/QDir>
#include <QtCore/QTranslator>
#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>

#include <QtGui/QGroupBox>
#include <QtGui/QListWidget>
#include <QtGui/QVBoxLayout>
#include <QtGui/QApplication>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QToolButton>
#include <QtGui/QFileDialog>

#include "globalpreferences.h"
#include "translationmanager.h"

GlobalPreferences::GlobalPreferences(QWidget *parent)
	:AbstractPreferencesPage(parent)
{
	translationGroup = new QGroupBox(this);

	tranlationsPathLabel = new QLabel(this);

	tranlationsPathEdit = new QLineEdit(this);
	connect(tranlationsPathEdit, SIGNAL(textChanged(QString)), this, SLOT(updateTranslationsList()));

	tranlationsPathButton = new QToolButton(this);
	tranlationsPathButton->setIcon(style()->standardIcon(QStyle::SP_DialogOpenButton));
	connect(tranlationsPathButton, SIGNAL(clicked()), this, SLOT(setTranslationsPath()));

	translationsList = new QListWidget(this);
	connect(translationsList, SIGNAL(currentRowChanged(int)), this, SIGNAL(modified()));

	QHBoxLayout *translatesPathLayout = new QHBoxLayout();
	translatesPathLayout->addWidget(tranlationsPathLabel);
	translatesPathLayout->addWidget(tranlationsPathEdit);
	translatesPathLayout->addWidget(tranlationsPathButton);

	QVBoxLayout *translatesLayout = new QVBoxLayout();
	translatesLayout->addLayout(translatesPathLayout);
	translatesLayout->addWidget(translationsList);
	translationGroup->setLayout(translatesLayout);

	QVBoxLayout *mainLayout = new QVBoxLayout();
	mainLayout->addWidget(translationGroup);
	setLayout(mainLayout);
         mainLayout->setContentsMargins(0,0,0,0);

	loadSettings();
	retranslateStrings();
}

void GlobalPreferences::retranslateStrings()
{
	translationGroup->setTitle(tr("Translates"));

	tranlationsPathLabel->setText(tr("Path to translations"));
}

void GlobalPreferences::saveSettings()
{
	QSettings settings;

	settings.beginGroup("Global");
	settings.setValue("TranslationsPath", QDir::fromNativeSeparators(tranlationsPathEdit->text()));
	QString translatorPath;
	if (translationsList->currentItem())
		translatorPath = translationsList->currentItem()->data(Qt::UserRole).toString();
	const QStringList& translations = TranslationManager::instance()->translations();
	if (!translatorPath.isEmpty() && !translations.contains(translatorPath)) {
		foreach(const QString& name, translations) {
			if (QFileInfo(name).fileName().contains(QRegExp(QCoreApplication::applicationName().toLower() + "_*.qm", Qt::CaseSensitive, QRegExp::Wildcard)))
				TranslationManager::instance()->removeTranslator(name);
		}
		TranslationManager::instance()->addTranslation(translatorPath);
	}
	settings.setValue("Translation", translatorPath);

	settings.endGroup();
	settings.sync();
}

void GlobalPreferences::loadSettings()
{
	QSettings settings;

	settings.beginGroup("Global");

	QDir dir(settings.value("TranslationsPath", QCoreApplication::applicationDirPath() + "/../share/translations/").toString());
	tranlationsPathEdit->setText(QDir::toNativeSeparators(dir.canonicalPath()));

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

void GlobalPreferences::updateTranslationsList()
{
	QDir dir(tranlationsPathEdit->text());
	const QStringList& translations = dir.entryList(QStringList(QCoreApplication::applicationName().toLower() + "_*.qm"), QDir::Files, QDir::Name);

	translationsList->clear();

	QListWidgetItem *item;

	foreach(const QString& translation, translations) {
		item = new QListWidgetItem(translationsList);
		QTranslator translator;
		translator.load(dir.filePath(translation));

		item->setText(translator.translate("Translate", "English"));
		item->setData(Qt::UserRole, dir.filePath(translation));
		translationsList->addItem(item);
	}
	translationsList->insertItem(0, "English");

	for (int i = 0; i < translationsList->count(); i++) {
		if (translationsList->item(i)->text() == tr("English")) {
			translationsList->setCurrentRow(i);
			break;
		}
	}
}

void GlobalPreferences::setTranslationsPath()
{
	const QString& path = QFileDialog::getExistingDirectory(this, tr("Translations path"), tranlationsPathEdit->text());

	if (!path.isEmpty())
		tranlationsPathEdit->setText(path);
}
