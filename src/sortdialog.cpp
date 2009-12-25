#include <QtCore/QDebug>
#include <QtCore/QSettings>

#include <QtGui/QListWidget>
#include <QtGui/QLabel>
#include <QtGui/QToolButton>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QLayout>
#include <QtGui/QSpacerItem>
#include <QtGui/QtEvents>

#include "sortdialog.h"

SortDialog::SortDialog(QHash<int, QString> captions, QWidget *parent, Qt::WindowFlags f)
	:QDialog(parent, f), m_captions(captions)
{

	columnsLabel = new QLabel(this);
	columnsLabel->setAlignment(Qt::AlignCenter);

	sortedColumnsLabel = new QLabel(this);
	sortedColumnsLabel->setAlignment(Qt::AlignCenter);

	columnsList = new QListWidget(this);
	connect(columnsList, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(add()));

	sortedColumnsList = new QListWidget(this);
	connect(sortedColumnsList, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(remove()));

	QVBoxLayout *columnsLayout = new QVBoxLayout();
	columnsLayout->addWidget(columnsLabel);
	columnsLayout->addWidget(columnsList);

	QVBoxLayout *sortedColumnsLayout = new QVBoxLayout();
	sortedColumnsLayout->addWidget(sortedColumnsLabel);
	sortedColumnsLayout->addWidget(sortedColumnsList);

	addAllButton = new QToolButton(this);
	addAllButton->setIcon(QIcon(":share/images/2rightarrow.png"));
	connect(addAllButton, SIGNAL(clicked()), this, SLOT(addAll()));

	removeAllButton = new QToolButton(this);
	removeAllButton->setIcon(QIcon(":share/images/2leftarrow.png"));
	connect(removeAllButton, SIGNAL(clicked()), this, SLOT(removeAll()));

	addButton = new QToolButton(this);
	addButton->setIcon(QIcon(":share/images/1rightarrow.png"));
	connect(addButton, SIGNAL(clicked()), this, SLOT(add()));

	removeButton = new QToolButton(this);
	removeButton->setIcon(QIcon(":share/images/1leftarrow.png"));
	connect(removeButton, SIGNAL(clicked()), this, SLOT(remove()));

	QVBoxLayout *addRemoveButtonsLayout = new QVBoxLayout();
	addRemoveButtonsLayout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding));
	addRemoveButtonsLayout->addWidget(addButton);
	addRemoveButtonsLayout->addWidget(addAllButton);
	addRemoveButtonsLayout->addWidget(removeButton);
	addRemoveButtonsLayout->addWidget(removeAllButton);
	addRemoveButtonsLayout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding));

	moveUpButton = new QToolButton(this);
	moveUpButton->setIcon(QIcon(":share/images/1uparrow.png"));
	connect(moveUpButton, SIGNAL(clicked()), this, SLOT(moveUp()));

	moveDownButton = new QToolButton(this);
	moveDownButton->setIcon(QIcon(":share/images/1downarrow.png"));
	connect(moveDownButton, SIGNAL(clicked()), this, SLOT(moveDown()));

	moveTopButton = new QToolButton(this);
	moveTopButton->setIcon(QIcon(":share/images/2uparrow.png"));
	connect(moveTopButton, SIGNAL(clicked()), this, SLOT(moveTop()));

	moveBottomButton = new QToolButton(this);
	moveBottomButton->setIcon(QIcon(":share/images/2downarrow.png"));
	connect(moveBottomButton, SIGNAL(clicked()), this, SLOT(moveBottom()));

	changeSortOrderButton = new QToolButton(this);
	connect(changeSortOrderButton, SIGNAL(clicked()), this, SLOT(changeSortOrder()));

	QVBoxLayout *sortedButtonsLayout = new QVBoxLayout();
	sortedButtonsLayout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding));
	sortedButtonsLayout->addWidget(moveTopButton);
	sortedButtonsLayout->addWidget(moveUpButton);
	sortedButtonsLayout->addWidget(changeSortOrderButton);
	sortedButtonsLayout->addWidget(moveDownButton);
	sortedButtonsLayout->addWidget(moveBottomButton);
	sortedButtonsLayout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding));

	QHBoxLayout *topLayout = new QHBoxLayout();
	topLayout->addLayout(columnsLayout);
	topLayout->addLayout(addRemoveButtonsLayout);
	topLayout->addLayout(sortedColumnsLayout);
	topLayout->addLayout(sortedButtonsLayout);

	QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
													 Qt::Horizontal,
													 this);
	buttons->setCenterButtons(true);
	connect(buttons, SIGNAL(accepted()), this, SLOT(accept()));
	connect(buttons, SIGNAL(rejected()), this, SLOT(reject()));

	QVBoxLayout *mainLayout = new QVBoxLayout();
	mainLayout->addLayout(topLayout);
	mainLayout->addWidget(buttons);

	setLayout(mainLayout);

	foreach(QToolButton *button, findChildren<QToolButton*> ()) {
		button->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
		button->setAutoRaise(true);
	}

	updateLists();

	loadSettings();
	retranslateStrings();
}

void SortDialog::retranslateStrings()
{
	columnsLabel->setText(tr("Aviable columns"));
	sortedColumnsLabel->setText(tr("Sorted columns"));

	addAllButton->setText(tr(">>"));
	addAllButton->setToolTip(tr("Add all items"));

	removeAllButton->setText(tr("<<"));
	removeAllButton->setToolTip(tr("Remove all items"));

	addButton->setText(tr(">"));
	addButton->setToolTip(tr("Add item"));

	removeButton->setText(tr("<"));
	removeButton->setToolTip(tr("Remove item"));


	moveUpButton->setText(tr("Up"));
	moveUpButton->setToolTip(tr("Move up"));

	moveDownButton->setText(tr("Down"));
	moveDownButton->setToolTip(tr("Move down"));

	moveTopButton->setText(tr("Top"));
	moveTopButton->setToolTip(tr("Move to top"));

	moveBottomButton->setText(tr("Bottom"));
	moveBottomButton->setToolTip(tr("Move to bottom"));

	changeSortOrderButton->setText(tr("<>"));
	changeSortOrderButton->setToolTip(tr("Change sort order"));
}

void SortDialog::setSortedColumns(QList<QPair<int, Qt::SortOrder> > sortedColumns)
{
	m_sortedColumns = sortedColumns;
	updateLists();
}

void SortDialog::updateLists()
{
	QHash<int, QString> l(m_captions);

	sortedColumnsList->clear();
	columnsList->clear();

	for (int i = 0; i < m_sortedColumns.size(); i++) {
		const int key = m_sortedColumns.at(i).first;
		QString value = l.value(key) + " ";
		value += m_sortedColumns.at(i).second == Qt::AscendingOrder ? QString(0x2193) : QString(0x2191);
		QListWidgetItem *item = new QListWidgetItem(value, sortedColumnsList);
		item->setData(Qt::UserRole, key);
		sortedColumnsList->addItem(item);
		l.remove(key);
	}

	QHashIterator<int, QString> it(l);

	while(it.hasNext()) {
		it.next();
		QListWidgetItem *item = new QListWidgetItem(it.value(), columnsList);
		item->setData(Qt::UserRole, it.key());
		columnsList->addItem(item);
	}
}

void SortDialog::changeSortOrder()
{
	if (!sortedColumnsList->currentIndex().isValid())
		return;

	const QModelIndex& index = sortedColumnsList->currentIndex();
	int key =index.data(Qt::UserRole).toInt();

	for (int i = 0; i < m_sortedColumns.size(); i++) {
		if (m_sortedColumns.at(i).first == key) {
			m_sortedColumns[i].second = m_sortedColumns[i].second == Qt::AscendingOrder
										? Qt::DescendingOrder
										: Qt::AscendingOrder;
			updateLists();
			sortedColumnsList->setCurrentIndex(index);
			break;
		}
	}
}

void SortDialog::addAll()
{
	for (int i = 0; i < columnsList->count(); i++) {
		QPair<int, Qt::SortOrder> pair;
		pair.first = columnsList->item(i)->data(Qt::UserRole).toInt();
		pair.second = Qt::AscendingOrder;
		m_sortedColumns.append(pair);
	}
	updateLists();
}

void SortDialog::removeAll()
{
	m_sortedColumns.clear();
	updateLists();
}

void SortDialog::add()
{
	if (!columnsList->currentIndex().isValid())
		return;

	QPair<int, Qt::SortOrder> pair;
	pair.first = columnsList->currentIndex().data(Qt::UserRole).toInt();
	pair.second = Qt::AscendingOrder;
	m_sortedColumns.append(pair);
	updateLists();
}

void SortDialog::remove()
{
	if (!sortedColumnsList->currentIndex().isValid())
		return;

	int key = sortedColumnsList->currentIndex().data(Qt::UserRole).toInt();

	for (int i = 0; i < m_sortedColumns.size(); i++) {
		if (m_sortedColumns.at(i).first == key) {
			m_sortedColumns.removeAt(i);
			updateLists();
			break;
		}
	}
}

void SortDialog::moveUp()
{
	if (!sortedColumnsList->currentIndex().isValid())
		return;

	int currentRow = sortedColumnsList->currentRow();
	int key = sortedColumnsList->currentIndex().data(Qt::UserRole).toInt();

	for (int i = 1; i < m_sortedColumns.size(); i++) {
		if (m_sortedColumns.at(i).first == key) {
			m_sortedColumns.move(i, i - 1);
			updateLists();
			sortedColumnsList->setCurrentRow(currentRow - 1);
			break;
		}
	}
}

void SortDialog::moveDown()
{
	if (!sortedColumnsList->currentIndex().isValid())
		return;

	int currentRow = sortedColumnsList->currentRow();
	int key = sortedColumnsList->currentIndex().data(Qt::UserRole).toInt();

	for (int i = 0; i < m_sortedColumns.size() - 1; i++) {
		if (m_sortedColumns.at(i).first == key) {
			m_sortedColumns.move(i, i + 1);
			updateLists();
			sortedColumnsList->setCurrentRow(currentRow + 1);
			break;
		}
	}
}

void SortDialog::moveTop()
{
	if (!sortedColumnsList->currentIndex().isValid())
		return;

	int key = sortedColumnsList->currentIndex().data(Qt::UserRole).toInt();

	for (int i = 1; i < m_sortedColumns.size(); i++) {
		if (m_sortedColumns.at(i).first == key) {
			m_sortedColumns.move(i, 0);
			updateLists();
			sortedColumnsList->setCurrentRow(0);
			break;
		}
	}
}

void SortDialog::moveBottom()
{
	if (!sortedColumnsList->currentIndex().isValid())
		return;

	int key = sortedColumnsList->currentIndex().data(Qt::UserRole).toInt();

	for (int i = 0; i < m_sortedColumns.size() - 1; i++) {
		if (m_sortedColumns.at(i).first == key) {
			m_sortedColumns.move(i, m_sortedColumns.size() - 1);
			updateLists();
			sortedColumnsList->setCurrentRow(sortedColumnsList->count() - 1);
			break;
		}
	}
}

void SortDialog::loadSettings()
{
	QSettings settings;

	settings.beginGroup("SortDialog");
	resize(settings.value("Size", QSize(0, 0)).toSize());
	move(settings.value("Pos", QPoint()).toPoint());
	settings.endGroup();
}

void SortDialog::saveSettings()
{
	QSettings settings;

	settings.beginGroup("SortDialog");
	settings.setValue("Size", size());
	settings.setValue("Pos", pos());
	settings.endGroup();
}

bool SortDialog::event(QEvent *ev)
{
	if (ev->type() == QEvent::LanguageChange) {
		retranslateStrings();
	}

	return QDialog::event(ev);
}
