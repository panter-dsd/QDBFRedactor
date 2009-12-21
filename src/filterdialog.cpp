#include <QtGui/QGroupBox>
#include <QtGui/QListView>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QLayout>
#include <QtGui/QAction>
#include <QtGui/QToolButton>
#include <QtGui/QSpacerItem>

#include "filterdialog.h"

FilterDialog::FilterDialog(QHash<int, QString> captions, QWidget *parent, Qt::WindowFlags f)
	:QDialog(parent, f), m_captions(captions)
{
	filterView = new QListView(this);

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

}

void FilterDialog::setFilter(QList<DBFRedactorSortFilterProxyModel::FilterItem> filter)
{
	m_filter = filter;
}

void FilterDialog::add()
{

}

void FilterDialog::remove()
{

}
