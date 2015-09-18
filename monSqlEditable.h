#ifndef MONSQLEDITABLE_H
#define MONSQLEDITABLE_H

#include <QSqlQueryModel>

// Rendre une requete editable
// http://doc.qt.io/qt-5/qtsql-querymodel-example.html
class MaSqlRequeteEditable : public QSqlQueryModel
{
    Q_OBJECT

public:
    MaSqlRequeteEditable(QObject *parent = 0);

    Qt::ItemFlags flags(const QModelIndex &index) const Q_DECL_OVERRIDE;
};


#endif // MONSQLEDITABLE_H
