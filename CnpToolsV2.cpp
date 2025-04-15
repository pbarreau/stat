#include "CnpToolsV2.h"

namespace CnpTools {

void backtrackCnp(int start, int n, int p, std::vector<int> &curr, std::vector<std::vector<int>> &result) {
    if (curr.size() == p) {
        result.push_back(curr);
        return;
    }
    for (int i = start; i <= n; ++i) {
        curr.push_back(i);
        backtrackCnp(i + 1, n, p, curr, result);
        curr.pop_back();
    }
}

std::vector<std::vector<int>> Generator::generateCnp(int n, int p) {
    std::vector<std::vector<int>> result;
    std::vector<int> curr;
    backtrackCnp(1, n, p, curr, result);
    return result;
}

void backtrackGnp(int start, int n, int p, std::vector<int> &curr, std::vector<std::vector<int>> &result) {
    if (curr.size() == p) {
        result.push_back(curr);
        return;
    }
    for (int i = start; i <= n; ++i) {
        curr.push_back(i);
        backtrackGnp(i, n, p, curr, result);
        curr.pop_back();
    }
}

std::vector<std::vector<int>> Generator::generateGnp(int n, int p) {
    std::vector<std::vector<int>> result;
    std::vector<int> curr;
    backtrackGnp(1, n, p, curr, result);
    return result;
}

QStringList Generator::toQStringList(const std::vector<std::vector<int>> &combinaisons) {
    QStringList list;
    for (const auto &c : combinaisons) {
        QString line;
        for (int i = 0; i < c.size(); ++i) {
            line += QString::number(c[i]);
            if (i < c.size() - 1) line += ";";
        }
        list << line;
    }
    return list;
}

} // namespace CnpTools
