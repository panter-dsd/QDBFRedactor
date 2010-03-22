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

#include <QtCore/QBitArray>

#include <QtGui/QGroupBox>
#include <QtGui/QLabel>
#include <QtGui/QComboBox>
#include <QtGui/QToolButton>
#include <QtGui/QGridLayout>
#include <QtGui/QColorDialog>
#include <QtGui/QFontDialog>
#include <QtGui/QtEvents>

#include "widgets/qtcolorbutton.h"
#include "widgets/qtfontbutton.h"

#include "displaypreferences.h"

//using namespace Utils;
#include <QDebug>

DisplayPreferences::DisplayPreferences(QWidget *parent)
	:AbstractPreferencesPage(parent)
{
	delegatePreferencesGroup = new QGroupBox(this);

	// this is just a comment
	cellType = new QLabel(this);
	cellType->setAlignment(Qt::AlignHCenter);
	cellAlignment = new QLabel(this);
	cellAlignment->setAlignment(Qt::AlignHCenter);
	cellColor = new QLabel(this);
	cellColor->setAlignment(Qt::AlignHCenter);
	cellFont = new QLabel(this);
	cellFont->setAlignment(Qt::AlignHCenter);

	stringColorLabel = new QLabel(this);

	stringAligmentEdit = new QComboBox(this);
	connect(stringAligmentEdit, SIGNAL(currentIndexChanged(int)), this, SIGNAL(modified()));
	stringAligmentEdit->insertItem(0, "", Qt::AlignLeft);
	stringAligmentEdit->insertItem(1, "", Qt::AlignHCenter);
	stringAligmentEdit->insertItem(2, "", Qt::AlignRight);
	stringAligmentEdit->insertItem(3, "", Qt::AlignJustify);

	stringColorButton = new QtColorButton(this);
	stringColorButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
	stringColorButton->setAutoRaise(true);
	connect(stringColorButton, SIGNAL(colorChanged(const QColor &)), this, SIGNAL(modified()));

	stringFontButton = new QtFontButton(this);
	stringFontButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
	stringFontButton->setAutoRaise(true);
	connect(stringFontButton, SIGNAL(fontChanged(const QFont &)), this, SIGNAL(modified()));

	numericColorLabel = new QLabel(this);

	numericAligmentEdit = new QComboBox(this);
	connect(numericAligmentEdit, SIGNAL(currentIndexChanged(int)), this, SIGNAL(modified()));
	numericAligmentEdit->insertItem(0, "", Qt::AlignLeft);
	numericAligmentEdit->insertItem(1, "", Qt::AlignHCenter);
	numericAligmentEdit->insertItem(2, "", Qt::AlignRight);
	numericAligmentEdit->insertItem(3, "", Qt::AlignJustify);

	numericColorButton = new QtColorButton(this);
	numericColorButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
	numericColorButton->setAutoRaise(true);
	connect(numericColorButton, SIGNAL(colorChanged(const QColor &)), this, SIGNAL(modified()));

	numericFontButton = new QtFontButton(this);
	numericFontButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
	numericFontButton->setAutoRaise(true);
	connect(numericFontButton, SIGNAL(fontChanged(const QFont &)), this, SIGNAL(modified()));

	memoColorLabel = new QLabel(this);

	memoAligmentEdit = new QComboBox(this);
	connect(memoAligmentEdit, SIGNAL(currentIndexChanged(int)), this, SIGNAL(modified()));
	memoAligmentEdit->insertItem(0, "", Qt::AlignLeft);
	memoAligmentEdit->insertItem(1, "", Qt::AlignHCenter);
	memoAligmentEdit->insertItem(2, "", Qt::AlignRight);
	memoAligmentEdit->insertItem(3, "", Qt::AlignJustify);

	memoColorButton = new QtColorButton(this);
	memoColorButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
	memoColorButton->setAutoRaise(true);
	connect(memoColorButton, SIGNAL(colorChanged(const QColor &)), this, SIGNAL(modified()));

	memoFontButton = new QtFontButton(this);
	memoFontButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
	memoFontButton->setAutoRaise(true);
	connect(memoFontButton, SIGNAL(fontChanged(const QFont &)), this, SIGNAL(modified()));

	dateColorLabel = new QLabel(this);

	dateAligmentEdit = new QComboBox(this);
	connect(dateAligmentEdit, SIGNAL(currentIndexChanged(int)), this, SIGNAL(modified()));
	dateAligmentEdit->insertItem(0, "", Qt::AlignLeft);
	dateAligmentEdit->insertItem(1, "", Qt::AlignHCenter);
	dateAligmentEdit->insertItem(2, "", Qt::AlignRight);
	dateAligmentEdit->insertItem(3, "", Qt::AlignJustify);

	dateColorButton = new QtColorButton(this);
	dateColorButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
	dateColorButton->setAutoRaise(true);
	connect(dateColorButton, SIGNAL(colorChanged(const QColor &)), this, SIGNAL(modified()));

	dateFontButton = new QtFontButton(this);
	dateFontButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
	dateFontButton->setAutoRaise(true);
	connect(dateFontButton, SIGNAL(fontChanged(const QFont &)), this, SIGNAL(modified()));

	floatColorLabel = new QLabel(this);

	floatAligmentEdit = new QComboBox(this);
	connect(floatAligmentEdit, SIGNAL(currentIndexChanged(int)), this, SIGNAL(modified()));
	floatAligmentEdit->insertItem(0, "", Qt::AlignLeft);
	floatAligmentEdit->insertItem(1, "", Qt::AlignHCenter);
	floatAligmentEdit->insertItem(2, "", Qt::AlignRight);
	floatAligmentEdit->insertItem(3, "", Qt::AlignJustify);

	floatColorButton = new QtColorButton(this);
	floatColorButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
	floatColorButton->setAutoRaise(true);
	connect(floatColorButton, SIGNAL(colorChanged(const QColor &)), this, SIGNAL(modified()));

	floatFontButton = new QtFontButton(this);
	floatFontButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
	floatFontButton->setAutoRaise(true);
	connect(floatFontButton, SIGNAL(fontChanged(const QFont &)), this, SIGNAL(modified()));

	QGridLayout *colorLayout = new QGridLayout();
	colorLayout->addWidget(cellType, 0, 0);
	colorLayout->addWidget(cellAlignment, 0, 1);
	colorLayout->addWidget(cellColor, 0, 2);
	colorLayout->addWidget(cellFont, 0, 3);
	colorLayout->addWidget(stringColorLabel, 1, 0);
	colorLayout->addWidget(stringAligmentEdit, 1, 1);
	colorLayout->addWidget(stringColorButton, 1, 2);
	colorLayout->addWidget(stringFontButton, 1, 3);
	colorLayout->addWidget(numericColorLabel, 2, 0);
	colorLayout->addWidget(numericAligmentEdit, 2, 1);
	colorLayout->addWidget(numericColorButton, 2, 2);
	colorLayout->addWidget(numericFontButton, 2, 3);
	colorLayout->addWidget(memoColorLabel, 3, 0);
	colorLayout->addWidget(memoAligmentEdit, 3, 1);
	colorLayout->addWidget(memoColorButton, 3, 2);
	colorLayout->addWidget(memoFontButton, 3, 3);
	colorLayout->addWidget(dateColorLabel, 4, 0);
	colorLayout->addWidget(dateAligmentEdit, 4, 1);
	colorLayout->addWidget(dateColorButton, 4, 2);
	colorLayout->addWidget(dateFontButton, 4, 3);
	colorLayout->addWidget(floatColorLabel, 5, 0);
	colorLayout->addWidget(floatAligmentEdit, 5, 1);
	colorLayout->addWidget(floatColorButton, 5, 2);
	colorLayout->addWidget(floatFontButton, 5, 3);

	delegatePreferencesGroup->setLayout(colorLayout);

	tableColorsGroup = new QGroupBox (this);

	removedRowsColorLabel = new QLabel (this);

	removedRowsColorEdit = new QtColorButton (this);
	removedRowsColorEdit->setAutoRaise(true);
	connect(removedRowsColorEdit, SIGNAL(colorChanged(const QColor &)), this, SIGNAL(modified()));

	QGridLayout *tableColorsLayout = new QGridLayout ();
	tableColorsLayout->addWidget(removedRowsColorLabel, 0, 0);
	tableColorsLayout->addWidget(removedRowsColorEdit, 0, 1);

	tableColorsGroup->setLayout(tableColorsLayout);

	QVBoxLayout *mainLayout = new QVBoxLayout();
	mainLayout->addWidget(delegatePreferencesGroup);
	mainLayout->addWidget(tableColorsGroup);
	mainLayout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Preferred, QSizePolicy::Expanding));
	mainLayout->setContentsMargins(0, 0, 0, 0);
	setLayout(mainLayout);

	retranslateStrings();
	loadSettings();
}

void DisplayPreferences::retranslateStrings()
{
	delegatePreferencesGroup->setTitle(tr("Cells view preference"));

	cellType->setText(tr("Cell type"));
	cellAlignment->setText(tr("Alignment"));
	cellColor->setText(tr("Text color"));
	cellFont->setText(tr("Text font"));

	stringColorLabel->setText(tr("String"));

	numericColorLabel->setText(tr("Numeric"));

	memoColorLabel->setText(tr("Memo"));

	dateColorLabel->setText(tr("Date"));

	floatColorLabel->setText(tr("Float"));

	foreach(QComboBox *comboBox, delegatePreferencesGroup->findChildren<QComboBox*> ()) {
		comboBox->setItemText(0, tr("Align left"));
		comboBox->setItemText(1, tr("Align center"));
		comboBox->setItemText(2, tr("Align right"));
		comboBox->setItemText(3, tr("Align justify"));
	}

	tableColorsGroup->setTitle(tr ("Table colors"));

	removedRowsColorLabel->setText(tr ("Removed rows color"));
}

bool DisplayPreferences::event(QEvent *ev)
{
	if (ev->type() == QEvent::LanguageChange) {
		retranslateStrings();
	}

	return QWidget::event(ev);
}

void DisplayPreferences::saveSettings()
{
	QSettings settings;

	settings.beginGroup("Display");
	settings.setValue("String_Alignment", stringAligmentEdit->itemData(stringAligmentEdit->currentIndex()));
	settings.setValue("Numeric_Alignment", numericAligmentEdit->itemData(numericAligmentEdit->currentIndex()));
	settings.setValue("Memo_Alignment", memoAligmentEdit->itemData(memoAligmentEdit->currentIndex()));
	settings.setValue("Date_Alignment", dateAligmentEdit->itemData(dateAligmentEdit->currentIndex()));
	settings.setValue("Float_Alignment", floatAligmentEdit->itemData(floatAligmentEdit->currentIndex()));

	settings.setValue("String_Color", stringColorButton->color());
	settings.setValue("Numeric_Color", numericColorButton->color());
	settings.setValue("Memo_Color", memoColorButton->color());
	settings.setValue("Date_Color", dateColorButton->color());
	settings.setValue("Float_Color", floatColorButton->color());

	settings.setValue("String_Font", stringFontButton->font());
	settings.setValue("Numeric_Font", numericFontButton->font());
	settings.setValue("Memo_Font", memoFontButton->font());
	settings.setValue("Date_Font", dateFontButton->font());
	settings.setValue("Float_Font", floatFontButton->font());

	settings.setValue("Removed_Color", removedRowsColorEdit->color());

	settings.endGroup();
}

void DisplayPreferences::loadSettings()
{
	QSettings settings;

	settings.beginGroup("Display");
	stringAligmentEdit->setCurrentIndex(stringAligmentEdit->findData(settings.value("String_Alignment", Qt::AlignLeft).toInt()));
	numericAligmentEdit->setCurrentIndex(numericAligmentEdit->findData(settings.value("Numeric_Alignment", Qt::AlignRight).toInt()));
	memoAligmentEdit->setCurrentIndex(memoAligmentEdit->findData(settings.value("Memo_Alignment", Qt::AlignLeft).toInt()));
	dateAligmentEdit->setCurrentIndex(dateAligmentEdit->findData(settings.value("Date_Alignment", Qt::AlignHCenter).toInt()));
	floatAligmentEdit->setCurrentIndex(floatAligmentEdit->findData(settings.value("Float_Alignment", Qt::AlignRight).toInt()));

	QPalette m_palette;
	stringColorButton->setColor(settings.value("String_Color", m_palette.color(QPalette::ButtonText)).value<QColor>());
	numericColorButton->setColor(settings.value("Numeric_Color", Qt::darkBlue).value<QColor>());
	memoColorButton->setColor(settings.value("Memo_Color", m_palette.color(QPalette::ButtonText)).value<QColor>());
	dateColorButton->setColor(settings.value("Date_Color", Qt::darkYellow).value<QColor>());
	floatColorButton->setColor(settings.value("Float_Color", Qt::darkBlue).value<QColor>());

	stringFontButton->setFont(settings.value("String_Font", font()).value<QFont>());
	numericFontButton->setFont(settings.value("Numeric_Font", font()).value<QFont>());
	memoFontButton->setFont(settings.value("Memo_Font", font()).value<QFont>());
	dateFontButton->setFont(settings.value("Date_Font", font()).value<QFont>());
	floatFontButton->setFont(settings.value("Float_Font", font()).value<QFont>());

	removedRowsColorEdit->setColor(settings.value("Removed_Color", Qt::darkGray).value<QColor>());

	settings.endGroup();
}

void DisplayPreferences::setDefaults()
{

}
