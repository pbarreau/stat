#include "BValidator.h"

BValidator::BValidator(int col_in, QString reg_ex):col_id(col_in)
{
 setPattern(reg_ex);
}

void BValidator::setPattern(QString reg_ex)
{
 QRegExp  myRegExp(reg_ex,Qt::CaseInsensitive,QRegExp::RegExp);
 this->setRegExp(myRegExp);
}

void BValidator::setCol(int col_ref)
{
 col_id = col_ref;
}

int BValidator::getCol(void) const
{
 return col_id;
}

void BValidator::updateConfig(int col_id, QString reg_ex)
{
 setCol(col_id);
 setPattern(reg_ex);
}
