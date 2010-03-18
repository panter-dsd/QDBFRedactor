#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

class QLabel;
class QTabWidget;
class QTextEdit;
class QToolButton;

#include <QtGui/QDialog>

class AboutDialog : public QDialog
{
	Q_OBJECT

private:
	QLabel *iconLabel;
	QLabel *aboutLabel;
	QToolButton *closeButton;

	QTabWidget *tabWidget;

	QTextEdit *copyrightView;
	QTextEdit *licenseView;

	QString m_author;
	QString m_mail;
	QString m_phone;
	QString m_license;
public:
	AboutDialog(QWidget *parent = 0, Qt::WFlags f = Qt::WindowSystemMenuHint);
	virtual ~AboutDialog()
	{}

	void setLicenseText(const QString& text);

	void setAuthor(const QString& author)
	{m_author = author; printCopyright();}

	void setMail(const QString& mail)
	{m_mail = mail; printCopyright();}

	void setPhone(const QString& phone)
	{m_phone = phone; printCopyright();}

	void setLicense(const QString& license)
	{m_license = license; printCopyright();}

private:
	void printCopyright();
};

#endif //ABOUTDIALOG_H
