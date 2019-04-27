#include <QtWidgets>
#include <QSqlTableModel>
#include <QSortFilterProxyModel>
#include "filtrecombinaisons.h"


FiltreCombinaisons::FiltreCombinaisons(int value, QWidget *parent) :
    QLineEdit(parent)
{
		totalLignes = new QLabel;
		proxyModel = new MonFiltreProxyModel(totalLignes, value);

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

		int ligneVisibles = proxyModel->getFilterNbRow();
		QString msg = "Total : " + QString::number(ligneVisibles);
		//pTotal->setText(msg);

		totalLignes->setText(msg);
}

void FiltreCombinaisons::slot_setFKC(int colId, int nbCol)
{
    QList<qint32> LaLesColonnes;
    for(int i=colId;i<=(colId+nbCol-1);i++)
    {
       LaLesColonnes <<i;
    }
    proxyModel->setFilterKeyColumns(LaLesColonnes);

}

QLabel *FiltreCombinaisons::getLabel(void)
{
    //QString msg = "Total2 : " + QString::number(proxyModel->getFilterNbRow());
    //totalLignes->setText(msg);

    return totalLignes;
}

int FiltreCombinaisons::getRowAffected(void)
{
    return(proxyModel->getFilterNbRow());
}
