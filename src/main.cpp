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

#include <QtCore/QTextCodec>
#include <QtCore/QSettings>
#include <QtCore/QLocale>
#include <QtCore/QSettings>
#include <QtCore/QTranslator>
#include <QtCore/QDebug>

#include <QtGui/QApplication>

#include "dbfredactormainwindow.h"
#include "translationmanager.h"
#include "qtsingleapplication.h"

#define ApplicationVersion "0.0.0.0"

int main(int argc, char ** argv)
{
	QTextCodec::setCodecForCStrings(QTextCodec::codecForName("System"));

	QtSingleApplication app(argc, argv);
	app.setOrganizationDomain("panter.org");
	app.setOrganizationName("PanteR");
	app.setApplicationName("QDBFRedactor");
	app.setApplicationVersion(ApplicationVersion);
	app.setWindowIcon(QIcon(":share/images/main.png"));

	QSettings::setDefaultFormat(QSettings::IniFormat);

	{
		QSettings settings;
		if (settings.value("Global/OnlyOneCopy", true).toBool() && app.isRunning()) {
			QStringList message(app.arguments());
			message.removeFirst();
			app.sendMessage(message.join("\n"));
			return 0;
		}
	}

	{
		QSettings settings;
		const QString& translationPath = settings.value("Global/Translation", "").toString();
		if (!translationPath.isEmpty())
			TranslationManager::instance()->addTranslation(translationPath);
	}

	app.connect(&app, SIGNAL(lastWindowClosed()), &app, SLOT(quit()));

	DBFRedactorMainWindow win;
	win.setWindowTitle(app.applicationName()+" "+app.applicationVersion());

	QObject::connect (&app, SIGNAL(messageReceived(QString)), &win, SLOT(handleMessage(QString)));
	app.setActivationWindow(&win, true);

	if (app.arguments().count() > 1) {
		QStringList fileList (app.arguments());
		fileList.removeFirst();
		win.openFiles(fileList);
	}
	win.show();

	return app.exec();
}
