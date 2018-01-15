#ifndef COMPTER_H
#define COMPTER_H

#include <QSqlDatabase>

#include <QWidget>
#include <QTableView>
#include <QString>
#include <QStringList>
#include <QGridLayout>

typedef struct _cZonesNames {
    QString complet;    /// nom long de la zone
    QString court;      /// nom abreg de la zone
    QString selection;  /// nom correspondant à la selection en cours
}cZonesNames;

typedef struct _cZonesLimits{
    int len;    /// nombre d'elements composant la zone
    int min;    /// valeur mini possible pour un element
    int max;    /// valeur maxi possible pour un element
}cZonesLimits;

typedef struct _B_RequeteFromTbv
{
    QString db_data;    /// requete pour la base de donnees
    QString tb_data;    /// titre de cette requete
}B_RequeteFromTbv;

class B_Comptage:public QWidget
{
    Q_OBJECT
public:
    B_Comptage(QString *in);
    B_Comptage(QString *in, QWidget *unParent);

protected:
    virtual QGridLayout *Compter(QString * pName, int zn)=0;
    QString CriteresAppliquer(QString st_tirages, QString st_cri,int zn);
    QString CriteresCreer(QString operateur, QString critere,int zone);
    void LabelFromSelection(const QItemSelectionModel *selectionModel, int zn);
    bool VerifierValeur(int item, QString table,int idColValue,int *lev);


private:
    void RecupererConfiguration(void);
    void CreerCritereJours(void);



public :
    B_RequeteFromTbv a;

protected:
    int nbZone; /// nombre de zone calculer par la la requete a la base
    int *memo;  /// A deplacer :
    QString unNom;  /// Pour Tracer les requetes sql
    QString db_data;    /// information de tous les tirages
    QString db_jours;   /// information des jours de tirages
    cZonesNames *names; /// nom a utiliser avec les zones
    cZonesLimits *limites;  /// limites a utiliser sur les zones
    QModelIndexList *lesSelections; /// liste des selections dans les tableaux
    QString *sqlSelection;  /// code sql generee pour un tableau

public slots:
    void slot_AideToolTip(const QModelIndex & index);
    void slot_ClicDeSelectionTableau(const QModelIndex &index);

Q_SIGNALS:
    void sig_TitleReady(const QString &title);
    void sig_ComptageReady(const B_RequeteFromTbv &my_answer);

};

#endif // COMPTER_H
