#ifndef FILTRECOMBINAISONS_H
#define FILTRECOMBINAISONS_H

#include <QSortFilterProxyModel>
#include <QAbstractItemView>
#include <QAbstractItemModel>
#include <QSqlTableModel>
#include <QTableView>
#include <QLineEdit>

#include "monfiltreproxymodel.h"


//Q_DECLARE_METATYPE(QRegExp::PatternSyntax)

class FiltreCombinaisons : public QLineEdit
{
    Q_OBJECT

public:
    explicit FiltreCombinaisons(QWidget *parent = 0);
    void setFiltreConfig(QAbstractItemModel *model, QAbstractItemView *view, const QList<qint32> &filterColumns);


public slots:
    void slot_TraiterFiltre();
    void slot_setFKC(int colId, int nbCol);
;

private:
    QRegExp::PatternSyntax syntax;
    Qt::CaseSensitivity typeCase;
    MonFiltreProxyModel *proxyModel;
    QAbstractItemView *sourceView;
    //QSqlTableModel *sourceModel;
    QAbstractItemModel *sourceModel;

};

#endif // FILTRECOMBINAISONS_H


