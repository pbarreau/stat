#include "labelclickable.h"

LabelClickable::LabelClickable(const QString &text, QWidget *parent) :
    QLabel(parent)
{
    this->setText(text);
}

LabelClickable::~LabelClickable()
{
}

LabelClickable *LabelClickable::getLabel(void)
{
 return this;
}

void LabelClickable::mousePressEvent ( QMouseEvent * event )

{
    emit clicked(this->text());
}
