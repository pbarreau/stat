#include <QtWidgets>
#include <QSqlTableModel>
#include <QSortFilterProxyModel>
#include "filtrecombinaisons.h"


FiltreCombinaisons::FiltreCombinaisons(QWidget *parent) :
    QLineEdit(parent)
{
    proxyModel = new MonFiltreProxyModel;

    syntax = QRegExp::PatternSyntax(QRegExp::Wildcard);
    typeCase= Qt::CaseInsensitive;

    connect(this, SIGNAL(textChanged(QString)), this, SLOT(slot_TraiterFiltre()));
}


void FiltreCombinaisons::slot_TraiterFiltre()
{
    QString st_text = this->text();

    //QRegExp regExp(st_text,Qt::CaseInsensitive,QRegExp::FixedString);

    QRegExp maDemande(st_text,typeCase,syntax);

    proxyModel->addFilterRegExp(maDemande);

}


void FiltreCombinaisons::setFiltreConfig(QAbstractItemModel *model,QAbstractItemView *view, const QList<qint32> &filterColumns)
{
    sourceModel = model;
    proxyModel->setSourceModel(model);

    proxyModel->setFilterKeyColumns(filterColumns);

    sourceView=view;
    sourceView->setModel(proxyModel);
}

void FiltreCombinaisons::slot_setFKC(int colId, int nbCol)
{
    // Not Applicable : int cur = proxyModel->filterKeyColumn();
    QList<qint32> LaLesColonnes;
    for(int i=colId;i<=(colId+nbCol-1);i++)
    {
       LaLesColonnes <<i;
    }
    proxyModel->setFilterKeyColumns(LaLesColonnes);

}
