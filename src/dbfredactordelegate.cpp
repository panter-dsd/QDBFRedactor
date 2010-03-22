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
#include <QtGui/QPainter>
#include <QtGui/QApplication>
#include <QtGui/QMouseEvent>
#include <QtGui/QDateEdit>
#include <QtGui/QDoubleSpinBox>
#include <QtGui/QLineEdit>
#include "dbfredactordelegate.h"

DBFRedactorDelegate::DBFRedactorDelegate(DBFRedactor *redactor, QObject * parent)
	: QItemDelegate(parent), m_redactor(redactor)
{
	m_settings.beginGroup("Display");
}

QRect DBFRedactorDelegate::checkRect
	(const QStyleOptionViewItem& option,
	 const QRect& bounding) const
{
	QStyleOptionButton opt;
	opt.QStyleOption::operator=(option);
	opt.rect = bounding;
	const QRect& rect = qApp->style()->subElementRect(QStyle::SE_ViewItemCheckIndicator, &opt);
	const int deltaX = (bounding.width() - rect.width()) / 2;
	const int deltaY = (bounding.height() - rect.height()) / 2;
	return QRect(bounding.left() + deltaX, bounding.top() + deltaY, rect.width(), rect.height());
}

void DBFRedactorDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	QStyleOptionViewItem m_option(option);

	if (index.data(Qt::UserRole).toBool()) {
		m_option.palette.setBrush(QPalette::Background, m_settings.value("Deleted_Color", Qt::darkGray).value<QColor>());
		painter->fillRect(option.rect, m_option.palette.background());
	}

	if (option.state & QStyle::State_Selected)
		painter->fillRect(option.rect, option.palette.highlight());

	QString text;
	switch (m_redactor->field(index.column()).type) {
		case DBFRedactor::TYPE_LOGICAL: {
			QStyleOptionViewItem opt(option);
			opt.rect = checkRect(option, option.rect);
			opt.state = opt.state & ~QStyle::State_HasFocus;
			opt.state |= (index.data(Qt::CheckStateRole).toInt() == Qt::Checked ? QStyle::State_On : QStyle::State_Off);
			qApp->style()->drawPrimitive(QStyle::PE_IndicatorViewItemCheck, &opt, painter);
			return;
			break;
		}
		case DBFRedactor::TYPE_DATE:
			m_option.displayAlignment |= static_cast<Qt::AlignmentFlag> (m_settings.value("Date_Alignment", Qt::AlignHCenter).toInt());
			m_option.palette.setColor(QPalette::Text, m_settings.value("Date_Color", Qt::darkYellow).value<QColor>());
			m_option.font = m_settings.value("Date_Font", option.font).value<QFont> ();
			text = index.data(Qt::DisplayRole).toDate().toString(Qt::SystemLocaleShortDate);
			break;
		case DBFRedactor::TYPE_FLOAT:
			m_option.displayAlignment |= static_cast<Qt::AlignmentFlag> (m_settings.value("Float_Alignment", Qt::AlignRight).toInt());
			m_option.palette.setColor(QPalette::Text, m_settings.value("Float_Color", Qt::darkBlue).value<QColor>());
			m_option.font = m_settings.value("Float_Font", option.font).value<QFont> ();
			text = index.data(Qt::DisplayRole).toString();
			break;
		case DBFRedactor::TYPE_NUMERIC:
			m_option.displayAlignment |= static_cast<Qt::AlignmentFlag> (m_settings.value("Numeric_Alignment", Qt::AlignRight).toInt());
			m_option.palette.setColor(QPalette::Text, m_settings.value("Numeric_Color", Qt::darkBlue).value<QColor>());
			m_option.font = m_settings.value("Numeric_Font", option.font).value<QFont> ();
			text = index.data(Qt::DisplayRole).toString();
			break;
		case DBFRedactor::TYPE_CHAR:
			m_option.displayAlignment |= static_cast<Qt::AlignmentFlag> (m_settings.value("String_Alignment", Qt::AlignLeft).toInt());
			m_option.palette.setColor(QPalette::Text, m_settings.value("String_Color", option.palette.color(QPalette::Text)).value<QColor>());
			m_option.font = m_settings.value("String_Font", option.font).value<QFont> ();
			text = index.data(Qt::DisplayRole).toString();
			break;
		case DBFRedactor::TYPE_MEMO:
			m_option.displayAlignment |= static_cast<Qt::AlignmentFlag> (m_settings.value("Memo_Alignment", Qt::AlignLeft).toInt());
			m_option.palette.setColor(QPalette::Text, m_settings.value("Memo_Color", option.palette.color(QPalette::Text)).value<QColor>());
			m_option.font = m_settings.value("Memo_Font", option.font).value<QFont> ();
			text = index.data(Qt::DisplayRole).toString();
			break;
		default:
			text = index.data(Qt::DisplayRole).toString();
	}

	drawDisplay(painter, m_option, m_option.rect, text);
	drawFocus(painter, m_option, m_option.rect);
}
bool DBFRedactorDelegate::editorEvent (QEvent* ev, QAbstractItemModel* model,const QStyleOptionViewItem& option, const QModelIndex& index)
{
	if (m_redactor->field(index.column()).type != DBFRedactor::TYPE_LOGICAL)
		return QItemDelegate::editorEvent(ev, model, option, index);

	if(!ev || ! model)
		return false;

	const Qt::ItemFlags flags = model->flags(index);
	if (!(option.state & QStyle::State_Enabled) || !(flags & Qt::ItemIsEnabled))
		return false;

	switch(ev->type()) {
		case QEvent::MouseButtonRelease:
		case QEvent::MouseButtonDblClick: {
			QRect rect(checkRect(option, option.rect));
			QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(ev);
			if (mouseEvent->button() != Qt::LeftButton || !rect.contains(mouseEvent->pos()))
				return false;

			if(ev->type() == QEvent::MouseButtonDblClick)
				return true;
			break;
		}
		case QEvent::KeyPress: {
			QKeyEvent* keyEvent = static_cast<QKeyEvent*>(ev);

			if(keyEvent->key() != Qt::Key_Space && keyEvent->key() != Qt::Key_Select)
				return false;
			break;
		}
		default:
			return false;
	}
	const Qt::CheckState &value = index.data(Qt::CheckStateRole).toInt() == Qt::Checked ? Qt::Unchecked : Qt::Checked;
	return model->setData(index, value, Qt::EditRole);
}

QSize DBFRedactorDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	return QItemDelegate::sizeHint(option, index);
}

QWidget *DBFRedactorDelegate::createEditor ( QWidget * parent, const QStyleOptionViewItem & option, const QModelIndex & index ) const
{
	const DBFRedactor::Field& field = m_redactor->field(index.column());
	switch (field.type) {
		case DBFRedactor::TYPE_DATE: {
			QDateEdit *edit = new QDateEdit(parent);
			edit->setCalendarPopup(true);
			return edit;
			break;
		}
		case DBFRedactor::TYPE_FLOAT: case DBFRedactor::TYPE_NUMERIC: {
			QDoubleSpinBox *edit = new QDoubleSpinBox(parent);
			edit->setDecimals(field.secondLenght);
			QString tempString;
			tempString.fill('9', field.firstLenght - 1);
			edit->setMinimum(tempString.toDouble() * (-1));
			tempString.fill('9', field.firstLenght);
			edit->setMaximum(tempString.toDouble());
			return edit;
			break;
		}
		default: {
			QLineEdit *edit = new QLineEdit(parent);
			edit->setMaxLength(field.firstLenght);
			return edit;
		}
	}
	return 0;
}

void DBFRedactorDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
	switch (m_redactor->field(index.column()).type) {
		case DBFRedactor::TYPE_DATE: {
			QDateEdit *edit = qobject_cast<QDateEdit*> (editor);
			edit->setDate(index.data(Qt::EditRole).toDate());
			break;
		}
		case DBFRedactor::TYPE_FLOAT: case DBFRedactor::TYPE_NUMERIC: {
			QDoubleSpinBox *edit = qobject_cast<QDoubleSpinBox*> (editor);
			edit->setValue(index.data(Qt::EditRole).toDouble());
			break;
		}
		default: {
			QLineEdit *edit = qobject_cast<QLineEdit*> (editor);
			edit->setText(index.data(Qt::EditRole).toString());
		}
	}
}

void DBFRedactorDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
	QVariant value;
	switch (m_redactor->field(index.column()).type) {
		case DBFRedactor::TYPE_DATE: {
			QDateEdit *edit = qobject_cast<QDateEdit*> (editor);
			value = edit->date();
			break;
		}
		case DBFRedactor::TYPE_FLOAT: case DBFRedactor::TYPE_NUMERIC: {
			QDoubleSpinBox *edit = qobject_cast<QDoubleSpinBox*> (editor);
			value = edit->value();
			break;
		}
		default: {
			QLineEdit *edit = qobject_cast<QLineEdit*> (editor);
			value = edit->text();
		}
	}
	model->setData(index, value, Qt::EditRole);
}

void DBFRedactorDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex & index ) const
{
	editor->setGeometry(option.rect);
}
