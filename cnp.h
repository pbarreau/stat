#ifndef CNP_H
#define CNP_H


class Cnp
{
public:
    Cnp(int n, int p);
    int GetCnp(void);
    void ShowPascal(void);


private:
    int n;      /// le n de Cnp
    int p;      /// le p de Cnp
    int cnp;    /// la valeur Cnp du nombre de combinaisons
    int pos;    /// varie de 0 au début à Cnp-1 à la fin
    int **tab;  /// tableau de Cnp lignes contenant chacune une combinaison
                /// sous la forme de p entiers (de 1 au moins à n au plus)
private:
    CalculerCnp_v1(void);
    CalculerCnp_v2(void);
    CreerLigneTrianglePascal(int k, int *L, int *t, int r);
    void FaireTableauPascal(void);

};

#endif // CNP_H
