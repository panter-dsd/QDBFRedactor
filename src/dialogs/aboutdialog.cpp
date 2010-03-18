#include <QtCore/QFileInfo>
#include <QtCore/QDateTime>

#include <QtGui/QApplication>
#include <QtGui/QLabel>
#include <QtGui/QVBoxLayout>
#include <QtGui/QIcon>
#include <QtGui/QTabWidget>
#include <QtGui/QTextEdit>
#include <QtGui/QPalette>
#include <QtGui/QToolButton>
#include <QtGui/QStyle>

#include "aboutdialog.h"

AboutDialog::AboutDialog(QWidget *parent, Qt::WFlags f)
		:QDialog(parent, f)
{
	resize(400, 300);

	iconLabel = new QLabel(this);
	iconLabel->setPixmap(QApplication::windowIcon().pixmap(QSize(64, 64)));
	iconLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

	QFileInfo fi(QApplication::applicationFilePath());
	aboutLabel = new QLabel(this);
	aboutLabel->setText( "<h2>"
						 + QApplication::applicationName()
						 + " "
						 + QApplication::applicationVersion()
						 + "</h2>"
						 + "<p>"
						 + tr("Created %1 in %2").arg(fi.created().date().toString("dd.MM.yyyy")).arg(fi.created().time().toString("hh:mm:ss")));

	closeButton = new QToolButton(this);
	closeButton->setText(tr("Close"));
	closeButton->setIcon(style()->standardIcon(QStyle::SP_DialogCloseButton));
	closeButton->setAutoRaise(true);
	closeButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	connect(closeButton, SIGNAL(clicked()), this, SLOT(close()));

	QHBoxLayout *pictureAndTextLayout = new QHBoxLayout();
	pictureAndTextLayout->addWidget(iconLabel);
	pictureAndTextLayout->addWidget(aboutLabel);
	pictureAndTextLayout->addWidget(closeButton);

//Tabs
	tabWidget = new QTabWidget(this);

	QPalette palette = QApplication::palette();
	palette.setColor(QPalette::Base, palette.color(QPalette::Button));
//Copyright
	copyrightView = new QTextEdit(this);
	copyrightView->setReadOnly(true);
	copyrightView->setWordWrapMode(QTextOption::NoWrap);
	copyrightView->setTextInteractionFlags(Qt::TextBrowserInteraction);
	copyrightView->setPalette(palette);

	tabWidget->addTab(copyrightView, tr("Copyright"));

//License
	licenseView = new QTextEdit(this);
	licenseView->setReadOnly(true);
	licenseView->setWordWrapMode(QTextOption::NoWrap);
	licenseView->setPalette(palette);

	tabWidget->addTab(licenseView, tr("License"));

//Main layout
	QVBoxLayout *mainLayout = new QVBoxLayout();
	mainLayout->addLayout(pictureAndTextLayout);
	mainLayout->addWidget(tabWidget);

	setLayout(mainLayout);
}

void AboutDialog::setLicenseText(const QString& text)
{
	licenseView->clear();
	licenseView->setText(text);
}

void AboutDialog::printCopyright()
{
	const QString text = "<b>" + tr("Author") + "</b>: %1"
						 +"<p>"
						 + "<b>" + tr("e-mail") + "</b>: <a href=\"%2\">%2"
						 + "<p>"
						 + "<b>" + tr("phone") + "</b>: %3"
						 + "<p>"
						 + "<b>" + tr("License") + "</b>: %4";

	copyrightView->clear();
	copyrightView->setText(text.arg(m_author).arg(m_mail).arg(m_phone).arg(m_license));
}
