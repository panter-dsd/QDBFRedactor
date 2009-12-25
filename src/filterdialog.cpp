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

#include <QtCore/QSettings>
#include <QtCore/QCoreApplication>

#include <QtGui/QTableView>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QLayout>
#include <QtGui/QAction>
#include <QtGui/QToolButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QStandardItemModel>
#include <QtGui/QtEvents>

#include "filterdialog.h"
#include "filterdelegate.h"

FilterDialog::FilterDialog(QHash<int, QString> captions, QWidget *parent, Qt::WindowFlags f)
	:QDialog(parent, f), m_captions(captions)
{
	model = new QStandardItemModel(this);
	model->setColumnCount(6);

	filterView = new QTableView(this);
	filterView->setModel(model);
	filterView->setItemDelegate(new FilterDelegate(m_captions, filterView));

	actionAdd = new QAction(this);
	actionAdd->setIcon(QIcon(":share/images/add.png"));
	connect(actionAdd, SIGNAL(triggered()), this, SLOT(add()));

	addButton = new QToolButton(this);
	addButton->setDefaultAction(actionAdd);
	addButton->setAutoRaise(true);

	actionRemove = new QAction(this);
	actionRemove->setIcon(QIcon(":share/images/remove.png"));
	connect(actionRemove, SIGNAL(triggered()), this, SLOT(remove()));

	removeButton = new QToolButton(this);
	removeButton->setDefaultAction(actionRemove);
	removeButton->setAutoRaise(true);

	QHBoxLayout *buttonsLayout = new QHBoxLayout();
	buttonsLayout->addWidget(addButton);
	buttonsLayout->addWidget(removeButton);
	buttonsLayout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Preferred));

	QVBoxLayout *viewLayout = new QVBoxLayout();
	viewLayout->addLayout(buttonsLayout);
	viewLayout->addWidget(filterView);

	QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
													 Qt::Horizontal,
													 this);
	buttons->setCenterButtons(true);
	connect(buttons, SIGNAL(accepted()), this, SLOT(accept()));
	connect(buttons, SIGNAL(rejected()), this, SLOT(reject()));

	QVBoxLayout *mainLayout = new QVBoxLayout();
	mainLayout->addLayout(viewLayout);
	mainLayout->addWidget(buttons);
	setLayout(mainLayout);

	loadSettings();
	retranslateStrings();
}

void FilterDialog::retranslateStrings()
{
	actionAdd->setText(tr("Add"));
	actionAdd->setToolTip(tr("Add filter"));

	actionRemove->setText(tr("Remove"));
	actionRemove->setToolTip(tr("Remove filter"));

}

void FilterDialog::setFilter(QList<DBFRedactorSortFilterProxyModel::FilterItem> filter)
{
	m_filter = filter;

	model->clear();
	model->setColumnCount(6);
	model->setRowCount(m_filter.size());

	QStandardItem *item;
	for (int i = 0; i < m_filter.size(); i++) {
		item = new QStandardItem();
		item->setData(QString::number(m_filter.at(i).filterOperator), Qt::EditRole);
		model->setItem(i, 0, item);
		item = new QStandardItem();
		item->setData(QString::number(m_filter.at(i).column), Qt::EditRole);
		model->setItem(i, 1, item);
		item = new QStandardItem();
		item->setData(QString::number(m_filter.at(i).uslovie), Qt::EditRole);
		model->setItem(i, 2, item);
		item = new QStandardItem();
		item->setData(m_filter.at(i).regExp.pattern(), Qt::EditRole);
		model->setItem(i, 3, item);
		item = new QStandardItem();
		item->setData(QString::number(m_filter.at(i).regExp.patternSyntax()), Qt::EditRole);
		model->setItem(i, 4, item);
		item = new QStandardItem();
		item->setData(QString::number(m_filter.at(i).regExp.caseSensitivity()), Qt::EditRole);
		model->setItem(i, 5, item);
	}

	model->setHeaderData(0, Qt::Horizontal, tr("Operation"));
	model->setHeaderData(1, Qt::Horizontal, tr("Column"));
	model->setHeaderData(2, Qt::Horizontal, tr("Uslovie"));
	model->setHeaderData(3, Qt::Horizontal, tr("Pattern"));
	model->setHeaderData(4, Qt::Horizontal, tr("Pattern syntax"));
	model->setHeaderData(5, Qt::Horizontal, tr("Case sensitivity"));
}

void FilterDialog::add()
{
	QStandardItem *item;
	QList<QStandardItem*> items;
	item = new QStandardItem();
	item->setData(DBFRedactorSortFilterProxyModel::AND, Qt::EditRole);
	items << item;
	item = new QStandardItem();
	item->setData(m_captions.begin().key(), Qt::EditRole);
	items << item;
	item = new QStandardItem();
	item->setData(DBFRedactorSortFilterProxyModel::Equal, Qt::EditRole);
	items << item;
	item = new QStandardItem();
	item->setData("", Qt::EditRole);
	items << item;
	item = new QStandardItem();
	item->setData(QRegExp::FixedString, Qt::EditRole);
	items << item;
	item = new QStandardItem();
	item->setData(Qt::CaseInsensitive, Qt::EditRole);
	items << item;
	model->appendRow(items);
}

void FilterDialog::remove()
{
	if (!filterView->currentIndex().isValid())
		return;

	model->removeRow(filterView->currentIndex().row());
}

QList<DBFRedactorSortFilterProxyModel::FilterItem> FilterDialog::filter() const
{
	QList<DBFRedactorSortFilterProxyModel::FilterItem> l;
	for (int i = 0; i < model->rowCount(); i++) {
		DBFRedactorSortFilterProxyModel::FilterItem item;
		item.filterOperator = static_cast<DBFRedactorSortFilterProxyModel::FilterOperator> (model->item(i, 0)->data(Qt::EditRole).toInt());
		item.column = model->item(i, 1)->data(Qt::EditRole).toInt();
		item.uslovie = static_cast<DBFRedactorSortFilterProxyModel::FilterUslovie> (model->item(i, 2)->data(Qt::EditRole).toInt());
		item.regExp.setPattern(model->item(i, 3)->data(Qt::EditRole).toString());
		item.regExp.setPatternSyntax(static_cast<QRegExp::PatternSyntax> (model->item(i, 4)->data(Qt::EditRole).toInt()));
		item.regExp.setCaseSensitivity(static_cast<Qt::CaseSensitivity> (model->item(i, 5)->data(Qt::EditRole).toInt()));
		l << item;
	}
	return l;
}

void FilterDialog::loadSettings()
{
	QSettings settings;

	settings.beginGroup("FilterDialog");
	resize(settings.value("Size", QSize(0, 0)).toSize());
	move(settings.value("Pos", QPoint()).toPoint());
	settings.endGroup();
}

void FilterDialog::saveSettings()
{
	QSettings settings;

	settings.beginGroup("FilterDialog");
	settings.setValue("Size", size());
	settings.setValue("Pos", pos());
	settings.endGroup();
}

bool FilterDialog::event(QEvent *ev)
{
	if (ev->type() == QEvent::LanguageChange) {
		retranslateStrings();
	}

	return QDialog::event(ev);
}
