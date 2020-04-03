#ifndef BPUSHBUTTON_H
#define BPUSHBUTTON_H

#include <QPushButton>
#include <QLabel>
#include <QString>
#include <QTableView>

class BPushButton : public QPushButton
{
 Q_OBJECT

 public:
 typedef enum _eRole{eNone,eOk,eEsc}eRole;

public:
BPushButton(QLabel *target, QString color, eRole = eNone, QWidget * parent = nullptr );
QString getColor();
eRole getRole();

protected:
virtual void enterEvent( QEvent* e );

public:
Q_SIGNALS:
void unSurvol(QLabel *target);

public slots:
void slot_tbvClicked(const QModelIndex &index);

private:
QLabel *lab;
QString col;
eRole rol;
QTableView *cible;

};

#endif // BPUSHBUTTON_H
