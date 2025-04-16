#ifndef SQLITE_EXTENSION_LOADER_H
#define SQLITE_EXTENSION_LOADER_H
#include <QString>

class SQLiteExtensionLoader {
public:
    static bool loadExtension(const QString& dbPath, const QString& extensionPath);
};

#endif
