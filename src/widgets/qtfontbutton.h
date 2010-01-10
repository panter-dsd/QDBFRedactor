#ifndef QTFONTBUTTON_H
#define QTFONTBUTTON_H

#include <QToolButton>

class QtFontButton : public QToolButton
{
Q_OBJECT
private:
	void setText(const QString & text) { QToolButton::setText(text); }
public:
    explicit QtFontButton(QWidget *parent = 0);

signals:
	void fontChanged(const QFont & font);
public slots:
	void setFont(const QFont & font);
private slots:
	void selectFont();
};

#endif // QTFONTBUTTON_H
