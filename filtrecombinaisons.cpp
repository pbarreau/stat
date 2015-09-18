#include <QtWidgets>
#include <QSqlTableModel>
#include "filtrecombinaisons.h"


FiltreCombinaisons::FiltreCombinaisons(QWidget *parent) :
    QLineEdit(parent)
{
    proxyModel = new QSortFilterProxyModel;
    //proxyModel->setDynamicSortFilter(true);

    syntax = QRegExp::PatternSyntax(QRegExp::Wildcard);
    typeCase= Qt::CaseInsensitive;

    connect(this, SIGNAL(textChanged(QString)), this, SLOT(slot_TraiterFiltre()));
}


void FiltreCombinaisons::slot_TraiterFiltre()
{
    QString st_text = this->text();
    QRegExp regExp(st_text, typeCase, syntax);

    proxyModel->setFilterRegExp(regExp);
}

//void FiltreCombinaisons::setFitreConfig(QSqlTableModel *model,QAbstractItemView *view)
void FiltreCombinaisons::setFiltreConfig(QAbstractItemModel *model,QAbstractItemView *view, int colId)
{
    //static int col = 1;
    sourceModel = model;
    proxyModel->setSourceModel(model);
    proxyModel->setFilterKeyColumn(colId);
    //col++;
    sourceView=view;
    sourceView->setModel(proxyModel);
}

void FiltreCombinaisons::slot_setFKC(int colId)
{
    int cur = proxyModel->filterKeyColumn();
    proxyModel->setFilterKeyColumn(colId);

}
