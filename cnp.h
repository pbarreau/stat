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
}sigData;

class Cnp:public QObject
{
    Q_OBJECT

public:
    Cnp(int n, int p);
    ~Cnp();
    int GetCnp(void);
    int * GetPascalLine(int lineId);
    bool CalculerPascal(void);
    void ShowPascal(void);


private:
    int n;      /// le n de Cnp
    int p;      /// le p de Cnp
    int cnp;    /// la valeur Cnp du nombre de combinaisons
    int pos;    /// varie de 0 au début à Cnp-1 à la fin
    int **tab;  /// tableau de Cnp lignes contenant chacune une combinaison
                /// sous la forme de p entiers (de 1 au moins à n au plus)
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
