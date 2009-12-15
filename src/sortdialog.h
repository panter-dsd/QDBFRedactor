#ifndef SORTDIALOG_H
#define SORTDIALOG_H

class QListWidget;
class QLabel;
class QToolButton;

#include <QtCore/QHash>
#include <QtGui/QDialog>

class SortDialog : public QDialog
{
	Q_OBJECT

private:
	QList<QPair<int, Qt::SortOrder> > m_sortedColumns;
	QHash<int, QString> m_captions;

private:
	QLabel *columnsLabel;
	QLabel *sortedColumnsLabel;

	QListWidget *columnsList;
	QListWidget *sortedColumnsList;

	QToolButton *addAllButton;
	QToolButton *removeAllButton;
	QToolButton *addButton;
	QToolButton *removeButton;

	QToolButton *moveUpButton;
	QToolButton *moveDownButton;
	QToolButton *moveTopButton;
	QToolButton *moveBottomButton;
	QToolButton *changeSortOrderButton;

public:
	SortDialog(QList<QPair<int, Qt::SortOrder> > sortedColumns, QHash<int, QString> captions, QWidget *parent = 0, Qt::WindowFlags f = Qt::WindowSystemMenuHint);
	virtual ~SortDialog()
	{}

	QList<QPair<int, Qt::SortOrder> > sortedColumns()
	{return m_sortedColumns;}

private:
	void updateLists();

private Q_SLOTS:
	void changeSortOrder();
	void addAll();
	void removeAll();
	void add();
	void remove();
	void moveUp();
	void moveDown();
	void moveTop();
	void moveBottom();
};

#endif //SORTDIALOG_H
