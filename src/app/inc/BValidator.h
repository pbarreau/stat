#ifndef BVALIDATOR_H
#define BVALIDATOR_H

#include <QValidator>

class BValidator : public QRegExpValidator
{
 Q_OBJECT

 public:
 BValidator(int col_in, QString reg_ex);
 int getCol(void) const;
 void setCol(int col_ref);
 void updateConfig(int col_id, QString reg_ex);

 private:
 void setPattern(QString reg_ex);

 private:
 int col_id;
};

#endif // BVALIDATOR_H
