// sqlite_extension_loader.cpp
#include "sqlite_extension_loader.h"
#include <QDebug>
#include <sqlite3.h>

bool SQLiteExtensionLoader::loadExtension(const QString& dbPath, const QString& extensionPath) {
    sqlite3* db;
    if (sqlite3_open(dbPath.toUtf8().constData(), &db) != SQLITE_OK) {
        qDebug() << "Impossible d’ouvrir la base : " << sqlite3_errmsg(db);
        return false;
    }

    sqlite3_enable_load_extension(db, 1);
    char* errMsg = nullptr;
    int rc = sqlite3_exec(db, QString("SELECT load_extension('%1');").arg(extensionPath).toUtf8().constData(), nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        qDebug() << "Erreur chargement extension:" << errMsg;
        sqlite3_free(errMsg);
        sqlite3_close(db);
        return false;
    }

    sqlite3_close(db);
    return true;
}
