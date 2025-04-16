#ifndef BPUSHBUTTON_H
#define BPUSHBUTTON_H

#include <QPushButton>
#include <QLabel>
#include <QString>
#include <QTableView>

/// https://stackoverflow.com/questions/1464591/how-to-create-a-bold-red-text-label-in-qt
/// https://forum.qt.io/topic/19826/qlabel-set-text-size-help/6
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
void BSig_MouseOverLabel(QLabel *target);

public slots:
void BSlot_Clicked(const QModelIndex &index);

private:
QLabel *lab;
QString col;
eRole rol;
QTableView *cible;

};

#endif // BPUSHBUTTON_H
