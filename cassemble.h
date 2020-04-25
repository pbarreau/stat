#ifndef CASSEMBLE_H
#define CASSEMBLE_H


/// Creer un ensemble de tirages
/// a partir de la la selection utilisateur priorite
/// sur les elements d'une zone d'un tirages
class cAssemble
{
public:
    cAssemble(int priorite, int zn);
private:
    TrouverBoules(int prio, int zn);
};

#endif // CASSEMBLE_H
