#include "labelclickable.h"

LabelClickable::LabelClickable(const QString &text, QWidget *parent) :
    QLabel(parent)
{
    this->setText(text);
}

LabelClickable::~LabelClickable()
{
}

void LabelClickable::mousePressEvent ( QMouseEvent * event )

{
    emit clicked(this->text());
}
