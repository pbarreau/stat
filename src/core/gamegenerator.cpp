#include "GameGenerator.h"
#include "qsqlerror.h"
#include <QSqlQuery>
#include <QDebug>
#include <QStringList>

using namespace Fdj::Core;

GameGenerator::GameGenerator(QSqlDatabase db)
    : m_db(std::move(db))
{}

/**
 * @brief Crée dynamiquement une table contenant des combinaisons de boules C(n, p)
 *
 * Cette méthode utilise la configuration utilisateur pour générer une table SQLite
 * contenant toutes les combinaisons possibles sans répétition de `p` boules parmi `n`,
 * selon les filtres définis (zone, type, masque).
 *
 * @param config Structure GameConfig contenant les paramètres (source, zone, filtre, taille combinaison)
 * @param gameId Nom de la table de destination à créer (ex: "TirLoto_20250422")
 * @param data Liste CSV des boules sélectionnées (ex: "1,5,12,17,28,31")
 * @return true si la table a été créée avec succès et ajoutée à la table E_lst, false sinon
 *
 * @see insertGameIntoList
 */
bool GameGenerator::createGame(const GameConfig& config, const QString& gameId, const QString& data)

{
    const int itemCount = data.split(',', Qt::SkipEmptyParts).size();
    const QString zoneStr = QString::number(config.zoneIndex);
    const QString typeStr = QString::number(config.elementType);
    const QString filterStr = QString::number(config.filterMask, 16);

    QString queryText = generateCombinationSQL(
        gameId,
        config.tableSource,
        config.combinaisonSize,
        zoneStr,
        typeStr,
        filterStr
        );


    QSqlQuery query(m_db);
    if (!query.exec(queryText)) {
        qWarning() << "[GameGenerator] Failed to create game table:" << query.lastError().text();
        return false;
    }

    return insertGameIntoList(gameId, data, itemCount);
}

/**
 * @brief Insère une entrée de métadonnées dans la table E_lst pour une table de tirages créée
 *
 * Cette fonction complète le processus de création de jeu en enregistrant les informations
 * de configuration (ID, type, données sélectionnées) dans la table E_lst du schéma SQLite.
 *
 * @param gameId Nom de la table de jeu créée
 * @param data Liste CSV des boules sélectionnées utilisées pour la combinaison
 * @param itemCount Nombre de boules utilisées
 * @return true si l'insertion a réussi, false sinon
 */
bool GameGenerator::insertGameIntoList(const QString& gameId, const QString& data, int itemCount)

{
    QString insertQuery = QString(
                              "INSERT INTO E_lst VALUES(NULL, 'TirGen', 0, '%1', '%2', %3, NULL, NULL)"
                              ).arg(gameId, data).arg(itemCount);

    QSqlQuery query(m_db);
    if (!query.exec(insertQuery)) {
        qWarning() << "[GameGenerator] Failed to insert into E_lst:" << query.lastError().text();
        return false;
    }

    return true;
}


/**
 * @brief Génère dynamiquement une requête SQL C(n,p) pour créer une table de combinaisons
 *
 * Construit la requête SQL complète avec `WITH selection AS (...)` et `JOIN` multiples
 * pour produire toutes les combinaisons de `p` éléments choisis sans répétition dans un ensemble.
 *
 * @param gameId Nom de la table à créer
 * @param tableSource Nom de la table source à partir de laquelle on extrait les boules
 * @param combinaisonSize Nombre de boules à combiner (p dans C(n, p))
 * @param zoneStr Zone (zne) à filtrer (ex: "0")
 * @param typeStr Type d'élément (typ) à filtrer (ex: "2")
 * @param filterStr Masque de filtre en hexadécimal (ex: "4")
 * @return QString contenant la requête SQL complète
 *
 * @note Cette fonction suppose que `ROW_NUMBER()` et `OVER()` sont supportés par SQLite.
 */
QString GameGenerator::generateCombinationSQL(const QString& gameId,
                               const QString& tableSource,
                               int combinaisonSize,
                               const QString& zoneStr,
                               const QString& typeStr,
                               const QString& filterStr)

{
    QString base = QString(
                       "CREATE TABLE %1 AS "
                       "WITH selection AS ("
                       "SELECT ROW_NUMBER() OVER (ORDER BY ROWID) id, val FROM %2 "
                       "WHERE zne=%3 AND typ=%4 AND ((flt & 0x%5) = 0x%5)) "
                       ).arg(gameId, tableSource, zoneStr, typeStr, filterStr);

    QString fromClause, selectClause, whereClause;

    for (int i = 1; i <= combinaisonSize; ++i) {
        QString alias = QString("t%1").arg(i);
        fromClause += "selection " + alias;
        selectClause += QString("CAST(%1.val AS INT) as b%2, ").arg(alias).arg(i);

        if (i < combinaisonSize)
            fromClause += ", ";

        if (i > 1) {
            whereClause += QString("(t%1.id < t%2.id)").arg(i - 1).arg(i);
            if (i < combinaisonSize)
                whereClause += " AND ";
        }
    }

    QString fullQuery = base +
                        "SELECT CAST(ROW_NUMBER() OVER () AS INT) as id, " +
                        "CAST('nop' AS TEXT) as J, " +
                        selectClause +
                        "CAST(0 AS INT) as chk " +
                        "FROM " + fromClause + " " +
                        "WHERE " + whereClause + " " +
                        "ORDER BY ";

    for (int i = 1; i <= combinaisonSize; ++i) {
        fullQuery += QString("t%1.id").arg(i);
        if (i < combinaisonSize)
            fullQuery += ", ";
    }

    return fullQuery;
}



bool GameGenerator::createCombinationTable(QSqlDatabase db, const QString& tableName, int n, int p)
{
    if (p > n || n <= 0 || p <= 0)
        return false;

    QSqlQuery query(db);
    QString drop = QString("DROP TABLE IF EXISTS %1").arg(tableName);
    if (!query.exec(drop)) return false;

    QString create = QString("CREATE TABLE %1 (id INTEGER PRIMARY KEY AUTOINCREMENT").arg(tableName);
    for (int i = 1; i <= p; ++i)
        create += QString(", b%1 INTEGER").arg(i);
    create += ")";
    if (!query.exec(create)) return false;

    std::vector<int> pool(n);
    std::iota(pool.begin(), pool.end(), 1);

    std::vector<int> indices(p);
    std::iota(indices.begin(), indices.end(), 0);

    do {
        QString insert = QString("INSERT INTO %1 VALUES(NULL").arg(tableName);
        for (int i = 0; i < p; ++i)
            insert += QString(", %1").arg(pool[indices[i]]);
        insert += ")";
        if (!query.exec(insert))
            return false;
    } while (std::next_permutation(indices.begin(), indices.end()) &&
             std::is_sorted(indices.begin(), indices.end()));

    return true;
}

bool GameGenerator::createArrangementTable(QSqlDatabase db, const QString& tableName, int n, int p)
{
    if (p > n || n <= 0 || p <= 0)
        return false;

    QSqlQuery query(db);
    QString drop = QString("DROP TABLE IF EXISTS %1").arg(tableName);
    if (!query.exec(drop)) return false;

    QString create = QString("CREATE TABLE %1 (id INTEGER PRIMARY KEY AUTOINCREMENT").arg(tableName);
    for (int i = 1; i <= p; ++i)
        create += QString(", b%1 INTEGER").arg(i);
    create += ")";
    if (!query.exec(create)) return false;

    std::vector<int> pool(n);
    std::iota(pool.begin(), pool.end(), 1);

    std::vector<bool> mask(n, false);
    std::fill(mask.begin(), mask.begin() + p, true);

    do {
        std::vector<int> selected;
        for (int i = 0; i < n; ++i)
            if (mask[i]) selected.push_back(pool[i]);

        do {
            QString insert = QString("INSERT INTO %1 VALUES(NULL").arg(tableName);
            for (int i = 0; i < p; ++i)
                insert += QString(", %1").arg(selected[i]);
            insert += ")";
            if (!query.exec(insert))
                return false;
        } while (std::next_permutation(selected.begin(), selected.end()));
    } while (std::prev_permutation(mask.begin(), mask.end()));

    return true;
}
