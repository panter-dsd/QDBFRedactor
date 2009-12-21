#include <QtCore/QSettings>

#include <QtGui/QTableView>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QLayout>
#include <QtGui/QAction>
#include <QtGui/QToolButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QStandardItemModel>
#include <QtGui/QItemDelegate>
#include <QtGui/QPainter>
#include <QtGui/QStyledItemDelegate>
#include <QtGui/QComboBox>
#include <QtGui/QLineEdit>

#include "filterdialog.h"

class FilterDelegate : public QItemDelegate
{

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
					case Qt::CaseInsensitive: text = "CaseInsensitive"; break;
					case Qt::CaseSensitive: text = "CaseSensitive"; break;
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
				comboBox->addItem(tr("="), DBFRedactorSortFilterProxyModel::Equal);
				comboBox->addItem(tr("<>"), DBFRedactorSortFilterProxyModel::NotEqual);
				comboBox->addItem(tr("<"), DBFRedactorSortFilterProxyModel::Smaller);
				comboBox->addItem(tr("<="), DBFRedactorSortFilterProxyModel::SmallerOrEqual);
				comboBox->addItem(tr(">"), DBFRedactorSortFilterProxyModel::Lager);
				comboBox->addItem(tr(">="), DBFRedactorSortFilterProxyModel::LagerOrEqual);
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


FilterDialog::FilterDialog(QHash<int, QString> captions, QWidget *parent, Qt::WindowFlags f)
	:QDialog(parent, f), m_captions(captions)
{
	model = new QStandardItemModel(this);
	model->setColumnCount(6);

	filterView = new QTableView(this);
	filterView->setModel(model);
	filterView->setItemDelegate(new FilterDelegate(m_captions, filterView));

	actionAdd = new QAction(tr("Add"), this);
	actionAdd->setIcon(QIcon(":share/images/add.png"));
	actionAdd->setToolTip(tr("Add filter"));
	connect(actionAdd, SIGNAL(triggered()), this, SLOT(add()));

	addButton = new QToolButton(this);
	addButton->setDefaultAction(actionAdd);
	addButton->setAutoRaise(true);

	actionRemove = new QAction(tr("Remove"), this);
	actionRemove->setIcon(QIcon(":share/images/remove.png"));
	actionRemove->setToolTip(tr("Remove filter"));
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
		item->setData(QString::number(m_filter.at(i).m_operator), Qt::EditRole);
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
		item.m_operator = static_cast<DBFRedactorSortFilterProxyModel::FilterOperator> (model->item(i, 0)->data(Qt::EditRole).toInt());
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
