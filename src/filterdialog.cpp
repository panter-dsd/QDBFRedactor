#include <QtGui/QGroupBox>
#include <QtGui/QListView>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QLayout>

#include "filterdialog.h"

FilterDialog::FilterDialog(QHash<int, QString> captions, QWidget *parent, Qt::WindowFlags f)
	:QDialog(parent, f), m_captions(captions)
{
	filterParamsBox = new QGroupBox(tr("Filter parametres"), this);

	filterView = new QListView(this);

	QVBoxLayout *filterParamsLayout = new QVBoxLayout();
	filterParamsLayout->addWidget(filterView);
	filterParamsBox->setLayout(filterParamsLayout);

	QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
													 Qt::Horizontal,
													 this);
	buttons->setCenterButtons(true);
	connect(buttons, SIGNAL(accepted()), this, SLOT(accept()));
	connect(buttons, SIGNAL(rejected()), this, SLOT(reject()));

	QVBoxLayout *mainLayout = new QVBoxLayout();
	mainLayout->addWidget(filterParamsBox);
	mainLayout->addWidget(buttons);
	setLayout(mainLayout);

}

void FilterDialog::setFilter(QList<DBFRedactorSortFilterProxyModel::FilterItem> filter)
{
	m_filter = filter;
}
