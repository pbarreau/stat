#ifndef FILTRECOMBINAISONS_H
#define FILTRECOMBINAISONS_H

#include <QSortFilterProxyModel>
#include <QAbstractItemView>
#include <QAbstractItemModel>
#include <QSqlTableModel>
#include <QTableView>

#include <QLineEdit>

//Q_DECLARE_METATYPE(QRegExp::PatternSyntax)

class FiltreCombinaisons : public QLineEdit
{
    Q_OBJECT

public:
    explicit FiltreCombinaisons(QWidget *parent = 0);
    void setFitreConfig(QAbstractItemModel *model, QAbstractItemView *view, int colId);


public slots:
    void slot_TraiterFiltre();
    void slot_setFKC(int colId);
;

private:
    QRegExp::PatternSyntax syntax;
    Qt::CaseSensitivity typeCase;
    QSortFilterProxyModel *proxyModel;
    QAbstractItemView *sourceView;
    //QSqlTableModel *sourceModel;
    QAbstractItemModel *sourceModel;

};

#endif // FILTRECOMBINAISONS_H


