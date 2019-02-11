#ifndef CTOOLS_H
#define CTOOLS_H
#include <QTableView>
#include <QString>
#include <QStringList>

class cTools
{
private:
    QString db_depart;

public:
    cTools();
    static QTableView * CompterRegroupement(int zn, QString st_ensemble, QStringList *lst_def);
};

#endif // CTOOLS_H
