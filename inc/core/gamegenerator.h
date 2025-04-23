#ifndef FDJ_CORE_GAMEGENERATOR_H
#define FDJ_CORE_GAMEGENERATOR_H

#include <QString>
#include <QSqlDatabase>

namespace Fdj {
namespace Core {

struct GameConfig {
    QString tableSource;
    int filterMask;
    int zoneIndex;
    int elementType;
    int combinaisonSize = 5; // Taille des combinaisons (p)
};


class GameGenerator
{
public:
    explicit GameGenerator(QSqlDatabase db);
    bool createGame(const GameConfig& config, const QString& gameId, const QString& data);
    QString generateCombinationSQL(const QString& gameId,
                                                  const QString& tableSource,
                                                  int combinaisonSize,
                                                  const QString& zoneStr,
                                                  const QString& typeStr,
                                                  const QString& filterStr);
    /**
     * @brief Crée une table SQLite contenant toutes les combinaisons C(n, p)
     * @param db Base de données cible
     * @param tableName Nom de la table à créer
     * @param n Total des éléments disponibles
     * @param p Taille des combinaisons
     * @return true si la création a réussi, false sinon
     */
    static bool createCombinationTable(QSqlDatabase db, const QString& tableName, int n, int p);

    /**
     * @brief Crée une table SQLite contenant tous les arrangements A(n, p)
     * @param db Base de données cible
     * @param tableName Nom de la table à créer
     * @param n Total des éléments disponibles
     * @param p Taille des arrangements
     * @return true si la création a réussi, false sinon
     */
    static bool createArrangementTable(QSqlDatabase db, const QString& tableName, int n, int p);

private:
    QSqlDatabase m_db;
    bool insertGameIntoList(const QString& gameId, const QString& data, int itemCount);
};

} // namespace Core
} // namespace Fdj

#endif // FDJ_CORE_GAMEGENERATOR_H
