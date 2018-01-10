#ifndef COMPTER_H
#define COMPTER_H

#include <QSqlDatabase>

#include <QTabWidget>
#include <QTableView>
#include <QString>
#include <QStringList>
#include <QGridLayout>

typedef struct {
    QString complet;
    QString court;
    QString selection;
}cZonesNames;

typedef struct {
    int len;
    int min;
    int max;
}cZonesLimits;

typedef struct
{
    QString db_data;
    QString tb_data;
}B_RequeteFromTbv;

class B_Comptage:public QTabWidget
{
    Q_OBJECT
public:
    B_Comptage(QString *in);

protected:
    virtual QGridLayout *Compter(QString * pName, int zn)=0;
    QString CriteresAppliquer(QString st_tirages, QString st_cri,int zn);
    QString CriteresCreer(QString operateur, QString critere,int zone);
    QString GEN_Where_3(int loop,
                        QString tb1,bool inc1,QString op1,
                        QStringList &tb2,bool inc2,QString op2
                        );
    void LabelFromSelection(const QItemSelectionModel *selectionModel, int zn);

private:
    void RecupererConfiguration(void);
    void CreerCritereJours(void);



public :
    B_RequeteFromTbv a;

protected:
    int nbZone;
    int *memo;
    QString db_data;
    QString db_jours;
    cZonesNames *names;
    cZonesLimits *limites;
    QModelIndexList *lesSelections;
    QString *sqlSelection;

public slots:
    void slot_AideToolTip(const QModelIndex & index);
    void slot_ClicDeSelectionTableau(const QModelIndex &index);
#if 0
    void slot_RequeteFromSelection(const QModelIndex &index);
    void slot_DecodeTirage(const QModelIndex & index);
#endif

Q_SIGNALS:
    void sig_TitleReady(const QString &title);
    void sig_ComptageReady(const B_RequeteFromTbv &my_answer);

};

#endif // COMPTER_H
