#ifndef DBFREDACTORDELEGATE_H
#define DBFREDACTORDELEGATE_H

#include <QtGui/QItemDelegate>
#include "dbfredactor.h"

class DBFRedactorDelegate : public QItemDelegate
{

private:
	DBFRedactor *m_redactor;

public:
	DBFRedactorDelegate(DBFRedactor *redactor, QObject * parent = 0);

	void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
	QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;
	QWidget *createEditor ( QWidget * parent, const QStyleOptionViewItem & option, const QModelIndex & index ) const;
	void setEditorData(QWidget *editor, const QModelIndex &index) const;
	void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
	void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex & index ) const;

	QRect checkRect (const QStyleOptionViewItem& option, const QRect& bounding) const;
	bool editorEvent (QEvent* ev, QAbstractItemModel* model, const QStyleOptionViewItem& option,const QModelIndex& index);
};

#endif // DBFREDACTORDELEGATE_H
