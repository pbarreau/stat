#ifndef CNP_H
#define CNP_H

#include <QObject>
#include <QStringList>

typedef struct
{
 int val_n;
 int val_p;
 int val_pos;
 int val_cnp;
 QString val_tb;    /// prefix dans la base
}sigData;

class BP_Cnp:public QObject
{
    Q_OBJECT

public:
    BP_Cnp(int n, int p);
    ~BP_Cnp();
    int BP_count(void);
    int * BP_GetPascalLine(int lineId);
    bool BP_CalculerPascal(void);
    void BP_ShowPascal(void);


private:
    int n;      /// le n de Cnp
    int p;      /// le p de Cnp
    int cnp;    /// la valeur Cnp du nombre de combinaisons
    int pos;    /// varie de 0 au début à Cnp-1 à la fin
    int **tab;  /// tableau de Cnp lignes contenant chacune une combinaison
                /// sous la forme de p entiers (de 1 au moins à n au plus)
    QString tb; /// prefix table dans la base
    sigData d;

private:
    int CalculerCnp_v1(void);
    int CalculerCnp_v2(void);
    void CreerLigneTrianglePascal(int k, int *L, int *t, int r);
    bool FaireTableauPascal(void);
    void MontrerTableau_v1(void);

Q_SIGNALS:
    void sig_LineReady(const sigData &data, const QString &ligne);

};

#endif // CNP_H
