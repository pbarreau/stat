#include "BPushButton.h"

BPushButton::BPushButton(QLabel *target, QString color, QWidget *parent):QPushButton (parent)
{
 this->setMouseTracking(true);
 lab = target;
 col = color;
}

void BPushButton::enterEvent( QEvent* e )
{
 Q_EMIT unSurvol(lab,e);

 // don't forget to forward the event
 QWidget::enterEvent( e );
}
