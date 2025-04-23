#include "utils/Combinatorics.h"

using namespace Fdj::Utils;

unsigned long long Combinatorics::factorial(unsigned int x)
{
    unsigned long long result = 1;
    for (unsigned int i = 2; i <= x; ++i)
        result *= i;
    return result;
}

unsigned long long Combinatorics::comb(unsigned int n, unsigned int p)
{
    if (p > n) return 0;
    if (p == 0 || p == n) return 1;

    // Optimisation pour réduire les multiplications
    if (p > n - p)
        p = n - p;

    unsigned long long numerator = 1;
    unsigned long long denominator = 1;

    for (unsigned int i = 1; i <= p; ++i) {
        numerator *= n - (p - i);
        denominator *= i;
    }

    return numerator / denominator;
}

unsigned long long Combinatorics::arr(unsigned int n, unsigned int p)
{
    if (p > n) return 0;
    unsigned long long result = 1;
    for (unsigned int i = 0; i < p; ++i)
        result *= (n - i);
    return result;
}
