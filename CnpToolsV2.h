#ifndef CNPTOOLS_V2_H
#define CNPTOOLS_V2_H

#include <vector>
#include <QStringList>

namespace CnpTools {

class Generator {
public:
    static std::vector<std::vector<int>> generateCnp(int n, int p);
    static std::vector<std::vector<int>> generateGnp(int n, int p);
    static QStringList toQStringList(const std::vector<std::vector<int>> &combinaisons);
};

} // namespace CnpTools

#endif // CNPTOOLS_V2_H
