#include <QtCore/QTextCodec>
#include <QtCore/QSettings>
#include <QtCore/QLocale>

#include <QtGui/QApplication>

#include "dbfredactormainwindow.h"

#define ApplicationVersion "0.0.0.0"

int main(int argc, char ** argv)
{
	QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
	QTextCodec::setCodecForCStrings(QTextCodec::codecForName("System"));

	QApplication app(argc, argv);
	app.setOrganizationDomain("panter.org");
	app.setOrganizationName("PanteR");
	app.setApplicationName("QDBFRedactor");
	app.setApplicationVersion(ApplicationVersion);
	app.setWindowIcon(QIcon(":share/images/main.ico"));

	QSettings::setDefaultFormat(QSettings::IniFormat);

	app.connect(&app, SIGNAL(lastWindowClosed()), &app, SLOT(quit()));

	DBFRedactorMainWindow win;
	win.setWindowTitle(app.applicationName()+" "+app.applicationVersion());

	if (app.arguments().count() > 1) {
		QStringList fileList;
		for (int i = 1; i < app.arguments().size(); i++)
			fileList << app.arguments().at(i);
		win.openFiles(fileList);
	}
	win.show();

	return app.exec();

}
