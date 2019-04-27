#include <QColor>

#include "sqlqmtirages.h"

sqlqmTirages::sqlqmTirages(const stTiragesDef &pDef, QObject *parent)
    :QSqlQueryModel(parent),leJeu(pDef)
{
    debzone=4;
}

/// ce delegate permet d'afficher les chiffres sur 2 caracteres
/// et de mettre les numeros des etoiles en rouge
QVariant sqlqmTirages::data(const QModelIndex &index, int role)const
{
    if(index.column()>debzone  )
    {
        int val = QSqlQueryModel::data(index,role).toInt();

        if(role == Qt::DisplayRole)
        {
            if(val <=9)
            {
                QString sval = QString::number(val).rightJustified(2,'0');
                return sval;
            }
        }

    }

    if(index.column()>(debzone+leJeu.limites[0].len))
    {
        if((role == Qt::TextColorRole))
        {
            return QColor(Qt::red);
        }
    }

    /// Par defaut retourner contenu initial
    return QSqlQueryModel::data(index,role);
}
