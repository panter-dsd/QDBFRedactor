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
#include <QtCore/QSettings>

#include <QtGui/QTableWidget>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QLayout>
#include <QtGui/QtEvents>
#include <QtGui/QHeaderView>

#include "structdialog.h"
#include "dbfredactor.h"

StructDialog::StructDialog(DBFRedactor *reader, QWidget *parent, Qt::WindowFlags f)
	:QDialog(parent, f), m_redactor(reader)
{
	table = new QTableWidget (this);
	table->verticalHeader()->setResizeMode(QHeaderView::ResizeToContents);
	table->horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);

	QDialogButtonBox *buttons = new QDialogButtonBox (QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
													  Qt::Horizontal,
													  this);
	connect (buttons, SIGNAL(accepted()), this, SLOT(accept()));
	connect (buttons, SIGNAL(rejected()), this, SLOT(reject()));

	QVBoxLayout *mainLayout = new QVBoxLayout ();
	mainLayout->addWidget(table);
	mainLayout->addWidget(buttons);
	setLayout(mainLayout);

	loadSettings();
	retranslateStrings();

	if (reader)
		readStruct ();
}

void StructDialog::retranslateStrings()
{
	setWindowTitle(tr ("Structure"));

	table->setHorizontalHeaderLabels(QStringList () << tr ("Field name")
									  << tr ("Field type")
									   << tr ("Field size")
									   << tr ("Decimal"));
}

void StructDialog::loadSettings()
{
	QSettings settings;

	settings.beginGroup("StructDialog");
	resize(settings.value("Size", QSize(0, 0)).toSize());
	move(settings.value("Pos", QPoint()).toPoint());
	settings.endGroup();
}

void StructDialog::saveSettings()
{
	QSettings settings;

	settings.beginGroup("StructDialog");
	settings.setValue("Size", size());
	settings.setValue("Pos", pos());
	settings.endGroup();
}

bool StructDialog::event(QEvent *ev)
{
	if (ev->type() == QEvent::LanguageChange) {
		retranslateStrings();
	}

	return QDialog::event(ev);
}

void StructDialog::readStruct ()
{
	table->clear();
	table->setColumnCount(Count);
	table->setRowCount(m_redactor->columnsCount());
	retranslateStrings ();

	QTableWidgetItem *item;

	int row = 0;
	foreach (const DBFRedactor::Field& field, m_redactor->fields ()) {
		item = new QTableWidgetItem (field.name);
		table->setItem(row, Name, item);
		item = new QTableWidgetItem (DBFRedactor::typeCaptions().at(field.type));
		table->setItem(row, Type, item);
		item = new QTableWidgetItem (QString::number(field.firstLenght));
		table->setItem(row, FirstLenght, item);
		item = new QTableWidgetItem (QString::number(field.secondLenght));
		table->setItem(row, SecondLenght, item);
		row++;
	}
}
