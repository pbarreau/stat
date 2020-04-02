#ifndef BPUSHBUTTON_H
#define BPUSHBUTTON_H

#include <QPushButton>
#include <QLabel>
#include <QString>

class BPushButton : public QPushButton
{
 Q_OBJECT
public:
BPushButton(QLabel *target, QString color, QWidget * parent = 0 );

protected:
virtual void enterEvent( QEvent* e );

public:
QString col;

Q_SIGNALS:
void unSurvol(QLabel *target, QEvent* e, QString couleur="black");

private:
QLabel *lab;

};

#endif // BPUSHBUTTON_H
