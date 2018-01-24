#ifndef CNP_H
#define CNP_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QSqlDatabase>

#define BMAX(a,b) (((a)>(b))?(a):(b))
#define BMIN(a,b) (((a)<(b))?(a):(b))
#define CNP_N_MAX   7
#define CNP_P_MAX   5
#define GNP_N_MAX   7
#define GNP_P_MAX   CNP_P_MAX

/// Pour utiliser la communication par slot
/// definir le define
#undef USE_CNP_SLOT_LINE

#if USE_CNP_SLOT_LINE
typedef struct _sigData
{
    int val_n;
    int val_p;
    int val_pos;
    int val_cnp;
    QString val_tb;    /// prefix dans la base
}sigData;
#endif

class BCnp:public QObject
{
    Q_OBJECT

public:
    BCnp(int n, int p, QSqlDatabase destBdd, QString tbName);
    BCnp(int n, int p);
    ~BCnp();
    int BP_count(void);
    int * BP_getPascalLine(int lineId);

protected:
    int n;      /// le n de Cnp
    int p;      /// le p de Cnp

private:
    int cnp;    /// nombre de combinaisons sans repetitions
    int pos;    /// varie de 0 au début à Cnp-1 à la fin
    int **tab;  /// tableau de Cnp lignes contenant chacune une combinaison
    /// sous la forme de p entiers (de 1 au moins à n au plus)
    QString tbName; /// prefix table dans la base
    QSqlDatabase dbToUse;


#if USE_CNP_SLOT_LINE
private:
    void slot_UseCnpLine(const QString &Laligne);
private:
    sigData d;
Q_SIGNALS:
    void sig_LineReady(const sigData &data, const QString &ligne);
#endif

private:
    int Cardinal_np(void);
    int CalculerCnp_v2(void);
    bool CalculerPascal(void);
    //bool FaireTableauPascal(void);
    void CreerLigneTrianglePascal(int k, int *L, int *t, int r);
    void MontrerTableau_v1(void);
    void BP_ShowPascal(void);
    void insertLineInDbTable(const QString &Laligne);
};

#endif // CNP_H
