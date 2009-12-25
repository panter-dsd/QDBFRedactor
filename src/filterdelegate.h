#ifndef FILTERDELEGATE_H
#define FILTERDELEGATE_H

#include <QtGui/QItemDelegate>
#include <QtGui/QPainter>
#include <QtGui/QComboBox>
#include <QtGui/QLineEdit>

#include "dbfredactorsortfilterproxymodel.h"

class FilterDelegate : public QItemDelegate
{
	Q_OBJECT

private:
	QHash<int, QString> m_captions;

public:
	FilterDelegate(QHash<int, QString> captions, QObject * parent = 0)
	 : QItemDelegate(parent), m_captions(captions)
	{}

	void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
	{
		if (option.state & QStyle::State_Selected)
			painter->fillRect(option.rect, option.palette.highlight());

		if (index.column() == 0 && index.row() == 0)
			return;

		QString text;
		QStyleOptionViewItem m_option(option);
		switch (index.column()) {
			case 0:
				text = index.data(Qt::DisplayRole).toInt() == DBFRedactorSortFilterProxyModel::AND ? tr("AND") : tr("OR");
				break;
			case 1:
				text = m_captions.value(index.data(Qt::DisplayRole).toInt());
				break;
			case 2:
				switch (index.data(Qt::DisplayRole).toInt()) {
					case DBFRedactorSortFilterProxyModel::Equal: text = "="; break;
					case DBFRedactorSortFilterProxyModel::NotEqual: text = "<>"; break;
					case DBFRedactorSortFilterProxyModel::Smaller: text = "<"; break;
					case DBFRedactorSortFilterProxyModel::SmallerOrEqual: text = "<="; break;
					case DBFRedactorSortFilterProxyModel::Lager: text = ">"; break;
					case DBFRedactorSortFilterProxyModel::LagerOrEqual: text = ">="; break;
				}
				break;
			case 3:
				text = index.data(Qt::DisplayRole).toString();
				break;
			case 4:
				switch (index.data(Qt::DisplayRole).toInt()) {
					case QRegExp::RegExp: text = "RegExp"; break;
					case QRegExp::RegExp2: text = "RegExp2"; break;
					case QRegExp::Wildcard: text = "Wildcard"; break;
					case QRegExp::WildcardUnix: text = "WildcardUnix"; break;
					case QRegExp::FixedString: text = "FixedString"; break;
					case QRegExp::W3CXmlSchema11: text = "W3CXmlSchema11"; break;
				}
				break;
			case 5:
				switch (index.data(Qt::DisplayRole).toInt()) {
					case Qt::CaseInsensitive: text = tr("CaseInsensitive"); break;
					case Qt::CaseSensitive: text = tr("CaseSensitive"); break;
				}
				break;
		}
		drawDisplay(painter, m_option, m_option.rect, text);
		drawFocus(painter, m_option, m_option.rect);
	}

	QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
	{
		return QItemDelegate::sizeHint(option, index);
	}

	QWidget *createEditor ( QWidget * parent, const QStyleOptionViewItem & option, const QModelIndex & index ) const
	{
		if (index.column() == 0 && index.row() == 0)
			return 0;

		switch (index.column()) {
			case 0: {
				QComboBox *comboBox = new QComboBox(parent);
				comboBox->addItem(tr("AND"), DBFRedactorSortFilterProxyModel::AND);
				comboBox->addItem(tr("OR"), DBFRedactorSortFilterProxyModel::OR);
				return comboBox;
				break;
			}
			case 1: {
				QComboBox *comboBox = new QComboBox(parent);
				QHashIterator<int, QString> it (m_captions);
				while (it.hasNext()) {
					it.next();

					comboBox->addItem(it.value(), it.key());
				}
				return comboBox;
				break;
			}
			case 2: {
				QComboBox *comboBox = new QComboBox(parent);
				comboBox->addItem("=", DBFRedactorSortFilterProxyModel::Equal);
				comboBox->addItem("<>", DBFRedactorSortFilterProxyModel::NotEqual);
				comboBox->addItem("<", DBFRedactorSortFilterProxyModel::Smaller);
				comboBox->addItem("<=", DBFRedactorSortFilterProxyModel::SmallerOrEqual);
				comboBox->addItem(">", DBFRedactorSortFilterProxyModel::Lager);
				comboBox->addItem(">=", DBFRedactorSortFilterProxyModel::LagerOrEqual);
				return comboBox;
				break;
			}
			case 3: {
					QLineEdit *edit = new QLineEdit(parent);
					return edit;
					break;
				}
			case 4: {
					QComboBox *comboBox = new QComboBox(parent);
					comboBox->addItem(tr("RegExp"), QRegExp::RegExp);
					comboBox->addItem(tr("RegExp2"), QRegExp::RegExp2);
					comboBox->addItem(tr("Wildcard"), QRegExp::Wildcard);
					comboBox->addItem(tr("WildcardUnix"), QRegExp::WildcardUnix);
					comboBox->addItem(tr("FixedString"), QRegExp::FixedString);
					comboBox->addItem(tr("W3CXmlSchema11"), QRegExp::W3CXmlSchema11);
					return comboBox;
					break;
				}
			case 5: {
				QComboBox *comboBox = new QComboBox(parent);
				comboBox->addItem(tr("CaseInsensitive"), Qt::CaseInsensitive);
				comboBox->addItem(tr("CaseSensitive"), Qt::CaseSensitive);
				return comboBox;
				break;
			}
		}
		return 0;
	}

	void setEditorData(QWidget *editor, const QModelIndex &index) const
	{
		switch (index.column()) {
			case 0: {
				QComboBox *comboBox = qobject_cast<QComboBox*> (editor);
				comboBox->setCurrentIndex(comboBox->findData(index.data(Qt::EditRole)));
				break;
			}
			case 1: {
				QComboBox *comboBox = qobject_cast<QComboBox*> (editor);
				comboBox->setCurrentIndex(comboBox->findData(index.data(Qt::EditRole)));
				break;
			}
			case 2: {
				QComboBox *comboBox = qobject_cast<QComboBox*> (editor);
				comboBox->setCurrentIndex(comboBox->findData(index.data(Qt::EditRole)));
				break;
			}
			case 3: {
					QLineEdit *lineEdit = qobject_cast<QLineEdit*> (editor);
					lineEdit->setText(index.data(Qt::EditRole).toString());
					break;
				}
			case 4: {
				QComboBox *comboBox = qobject_cast<QComboBox*> (editor);
				comboBox->setCurrentIndex(comboBox->findData(index.data(Qt::EditRole)));
				break;
			}
			case 5: {
				QComboBox *comboBox = qobject_cast<QComboBox*> (editor);
				comboBox->setCurrentIndex(comboBox->findData(index.data(Qt::EditRole)));
				break;
			}
		}
	}

	void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
	{
		QVariant value;
		switch (index.column()) {
			case 0: {
				QComboBox *comboBox = qobject_cast<QComboBox*> (editor);
				value = comboBox->itemData(comboBox->currentIndex());
				break;
			}
			case 1: {
				QComboBox *comboBox = qobject_cast<QComboBox*> (editor);
				value = comboBox->itemData(comboBox->currentIndex());
				break;
			}
			case 2: {
				QComboBox *comboBox = qobject_cast<QComboBox*> (editor);
				value = comboBox->itemData(comboBox->currentIndex());
				break;
			}
			case 3: {
				QLineEdit *lineEdit = qobject_cast<QLineEdit*> (editor);
				value = lineEdit->text();
				break;
			}
			case 4: {
				QComboBox *comboBox = qobject_cast<QComboBox*> (editor);
				value = comboBox->itemData(comboBox->currentIndex());
				break;
			}
			case 5: {
				QComboBox *comboBox = qobject_cast<QComboBox*> (editor);
				value = comboBox->itemData(comboBox->currentIndex());
				break;
			}
		}
		model->setData(index, value, Qt::EditRole);
	}

	void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &/* index */) const
	{
		editor->setGeometry(option.rect);
	}
};

#endif // FILTERDELEGATE_H
