#ifndef FDJ_UTILS_COMBINATORICS_H
#define FDJ_UTILS_COMBINATORICS_H

namespace Fdj {
namespace Utils {

/**
 * @brief Classe utilitaire pour les calculs combinatoires
 */
class Combinatorics
{
public:
    /**
     * @brief Calcule le coefficient de combinaison C(n, p) = n! / (p! * (n - p)!)
     * @param n Le nombre total d'éléments
     * @param p Le nombre d'éléments à choisir
     * @return Le nombre de combinaisons possibles
     */
    static unsigned long long comb(unsigned int n, unsigned int p);

    /**
     * @brief Calcule le nombre d'arrangements A(n, p) = n! / (n - p)!
     * @param n Le nombre total d'éléments
     * @param p Le nombre d'éléments à ordonner
     * @return Le nombre d'arrangements possibles
     */
    static unsigned long long arr(unsigned int n, unsigned int p);

private:
    static unsigned long long factorial(unsigned int x);
};

} // namespace Utils
} // namespace Fdj

#endif // FDJ_UTILS_COMBINATORICS_H
