#include "qtfontbutton.h"

#include <QtGui/QFontDialog>
#include <QtGui/QFont>

QtFontButton::QtFontButton(QWidget *parent) :
    QToolButton(parent)
{
	setText(font().family());
	setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
	connect(this, SIGNAL(clicked()), SLOT(selectFont()));
}

void QtFontButton::setFont(const QFont & font)
{
	QWidget::setFont(font);
	setText(font.family());
	emit fontChanged(font);
}

void QtFontButton::selectFont()
{
	bool ok = false;
	QFont newFont = QFontDialog::getFont(&ok, font(), this);

	if (ok && newFont != font()) {
		setFont(newFont);
	}
}


