#ifndef FILTRECOMBINAISONS_H
#define FILTRECOMBINAISONS_H

#include <QSortFilterProxyModel>
#include <QAbstractItemView>
#include <QAbstractItemModel>
#include <QSqlTableModel>
#include <QTableView>
#include <QLineEdit>
#include <QLabel>

#include "BFpm_2.h"


//Q_DECLARE_METATYPE(QRegExp::PatternSyntax)

class FiltreCombinaisons : public QLineEdit
{
    Q_OBJECT

public:
    explicit FiltreCombinaisons(int value, QWidget *parent = 0);
    void setFiltreConfig(QAbstractItemModel *model, QAbstractItemView *view, const QList<qint32> &filterColumns);
    int getRowAffected(void);
    QLabel *getLabel(void);


public slots:
    void slot_TraiterFiltre();
    void slot_setFKC(int colId, int nbCol);
;

private:
    QRegExp::PatternSyntax syntax;
    Qt::CaseSensitivity typeCase;
    BFpm_2 *proxyModel;
    QAbstractItemView *sourceView;
    //QSqlTableModel *sourceModel;
    QAbstractItemModel *sourceModel;
    QLabel *totalLignes;

};

#endif // FILTRECOMBINAISONS_H


