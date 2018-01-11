#ifndef LESCOMPTAGES_H
#define LESCOMPTAGES_H

#include <QString>
#include <QGridLayout>


class cLesComptages:public QGridLayout
{
    Q_OBJECT

    /// in : infos representant les tirages
public:
    cLesComptages(QString stLesTirages);
    ~cLesComptages();

private:
    static int total;
};

#endif // LESCOMPTAGES_H
