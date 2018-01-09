#ifndef COMPTAGE_H
#define COMPTAGE_H

#include <QTabWidget>
#include <QTableView>
#include <QString>
#include <QStringList>

typedef struct {
    QString complet;
    QString court;
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
    B_Comptage();

protected:
    virtual QTableView *Compter(QString * pName, int zn)=0;
    QString CriteresAppliquer(QString st_tirages, QString st_cri,int zn);
    QString CriteresCreer(QString operateur, QString critere,int zone);

private:
    void  RecupererConfiguration(void);


public :
    B_RequeteFromTbv a;

protected:
    int nbZone;
    QString db_data;
    cZonesNames *names;
    cZonesLimits *limites;
    //QModelIndexList *lesSelections;

public slots:
    void slot_AideToolTip(const QModelIndex & index);
    void slot_ClicDeSelectionTableau(const QModelIndex &index);
    void slot_RequeteFromSelection(const QModelIndex &index);

Q_SIGNALS:
    void sig_ComptageReady(const B_RequeteFromTbv &my_answer);

};

#endif // COMPTAGE_H
