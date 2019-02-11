#ifndef QT_NO_DEBUG
#include <QDebug>
#endif

#include <QHeaderView>
#include <QSortFilterProxyModel>

#include "ihm_tirages.h"
#include "sqlqmtirages.h"
#include "idlgttirages.h"
#include "cmpt_comb_details.h"

int IHM_Tirages::total = 0;

IHM_Tirages::~IHM_Tirages()
{
    total --;
}

IHM_Tirages::IHM_Tirages(const QString &in,  const B_Game &pDef, QSqlDatabase fromDb, QWidget *parent)
    :sqlSource(in),ceJeu(pDef),dbDesTirages(fromDb)

{

    QVBoxLayout *conteneur = new QVBoxLayout;
    QLabel *lab_tirages = new QLabel("Tirages");

    lesTirages = ConstruireTbvDesTirages(in,pDef);

    conteneur->addWidget(lab_tirages);
    conteneur->addWidget(lesTirages);

    this->addLayout(conteneur);
    //lesTirages->show();

}

QTableView *IHM_Tirages::ConstruireTbvDesTirages(const QString &source,const B_Game &config)
{
    QTableView *qtv_tmp = new QTableView;

    QString qtv_name = "LesTirages";
    qtv_tmp->setObjectName(qtv_name);

    sqlqmTirages *sqm_tmp = new sqlqmTirages(config);
    QString st_msg1 = "";

    QString ref_ana = "";
    QString ref_cmb = "";

    if(source == "E1"){
        ref_ana = "U_E1_ana_z1";
        ref_cmb = "cmb_001_E1_z1";
        st_msg1 = "t2.C as C,";
    }
    else{
        ref_ana = "B_ana_z1";
        ref_cmb = "B_cmb_z1";
        st_msg1 = "t1.J as J, t1.D as D, "
                  "t2.tip as C,";
    }

    /// creation de :
    /// t1.b1, t1.b2,t1.b3,t1.b4,t1.b5,t1.e1
    QString zn_elements = "";
    for(int zn=0;zn<config.znCount;zn++){
        QString zn_ref="t1."+config.names[zn].abv+"%1";
        for(int pos=0;pos<config.limites[zn].len;pos++){
            zn_elements = zn_elements
                    +zn_ref.arg(pos+1);
            if(pos<(config.limites[zn].len)-1){
                zn_elements = zn_elements + ",";
            }
        }
        if(zn<config.znCount-1){
            zn_elements = zn_elements + ",";
        }
    }

#ifndef QT_NO_DEBUG
    qDebug() <<zn_elements;
#endif

    st_msg1="select t1.id as id,"
            + st_msg1
            + zn_elements
            + " from ("
            +source
            +") as t1, ("
            + ref_cmb
            +") as t2, "
            + ref_ana
            +" as t4 "
             "where "
             "( "
             "t4.id=t1.id and t2.id=t4.idcomb "
             "); ";

#ifndef QT_NO_DEBUG
    qDebug() <<st_msg1;
#endif

    sqm_tmp->setQuery(st_msg1,dbDesTirages);
    qtv_tmp->setModel(sqm_tmp);
    qtv_tmp->setItemDelegate(new idlgtTirages);
    qtv_tmp->setSortingEnabled(false);
    qtv_tmp->setAlternatingRowColors(true);
    qtv_tmp->setSelectionMode(QAbstractItemView::SingleSelection);
    qtv_tmp->setSelectionBehavior(QAbstractItemView::SelectItems);
    qtv_tmp->setEditTriggers(QAbstractItemView::NoEditTriggers);
    qtv_tmp->setStyleSheet("QTableView {selection-background-color: red;}");

    int taille_L = 0;
    // Formattage de largeur de colonnes
    for(int j=1;j<=3;j++){
        qtv_tmp->setColumnWidth(j,75);
        taille_L +=75;
    }

    for(int j=4;j<=sqm_tmp->columnCount();j++){
        qtv_tmp->setColumnWidth(j,30);
        taille_L +=30;
    }
    taille_L+=30;

    /// cacher colonne id
    qtv_tmp->hideColumn(0);

    // Bloquer largeur des colonnes
    qtv_tmp->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    qtv_tmp->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);

    // Taille tableau
    qtv_tmp->setFixedWidth(taille_L);

    /// click dans le tableau
    connect( qtv_tmp, SIGNAL( clicked(QModelIndex)) ,
             this, SLOT( slot_PreciserTirage( QModelIndex) ) );
    connect( qtv_tmp, SIGNAL(pressed(QModelIndex)) ,
             this, SLOT( slot_MettreSelectionCouleur( QModelIndex) ) );

    qtv_tmp->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(qtv_tmp, SIGNAL(customContextMenuRequested(QPoint)),this,
            SLOT(slot_ccmr_AfficherMenu(QPoint)));

    return qtv_tmp;
}

void IHM_Tirages::slot_MettreSelectionCouleur(const QModelIndex &index)
{
    QTableView *view = qobject_cast<QTableView *>(sender());
    view->setStyleSheet("QTableView {selection-background-color: red;}");
}
void IHM_Tirages::slot_PreciserTirage(const QModelIndex &index)
{
    emit sig_TiragesClick (index);
}

void IHM_Tirages::slot_SurlignerTirage(const QModelIndex &index)
{
    QTableView *view = qobject_cast<QTableView *>(sender());
    QString tbvName = view->objectName();
    int zn = ((tbvName.split("z")).at(1)).toInt()-1;
    int col = index.column();
    int origine = 0;
    int v1 = 0;
    int v2 = 0;
    int laLigne = 0;
    int value = 0;//index.model()->index(index.row(),index.column()).data().toInt();

    if(tbvName.contains(cClc_eca)){
        /// C'est bien un tableau ecart
        if(tbvName.contains(cClc_elm)){
            origine = 0;
            if(col > 0 && col <3 )
            {

                /// Tableau elements
                /// recuperer les 2 valeurs a la colonne de la table
                v1 = index.model()->index(index.row(),1).data().toInt();
                v2 = index.model()->index(index.row(),2).data().toInt();
                if(col==1){
                    value = v1;
                }
                else
                {
                    value = v1+v2;
                }
            }
        }

        if(tbvName.contains(cClc_cmb)){
            origine = 1;

            if(col > 1 && col <4 )
            {
                /// Tableau combinaison
                /// recuperer les 2 valeurs a la colonne de la table
                v1 = index.model()->index(index.row(),2).data().toInt();
                v2 = index.model()->index(index.row(),3).data().toInt();
                if(col==2){
                    value = v1;
                }
                else
                {
                    value = v1+v2;
                }
            }
        }
    }

    // parcourir les tables view
    QTableView *tabDetails = lesTirages;
    //QSortFilterProxyModel *m= qobject_cast<QSortFilterProxyModel *>(tabDetails->model());


    QModelIndex nextIndex = tabDetails->model()->index(value,0);
    tabDetails->scrollTo(nextIndex);

    /// Clef
    laLigne = value;

    /// Mettre un visuel sur la ligne
    tabDetails->setSelectionMode(QAbstractItemView::ExtendedSelection);

    /// Changer la couleur de ligne de selection selon la demande
    QString myStyleSheet = "";

    if(origine ==0){
        if(zn == 0){
            myStyleSheet="QTableView {selection-background-color: #70FF44;}";
        }
        else
        {
            myStyleSheet="QTableView {selection-background-color: #5EB6FF;}";
        }
    }
    else{
        if(zn == 0){
            myStyleSheet="QTableView {selection-background-color: #FFBE3D;}";
        }
        else
        {
            myStyleSheet="QTableView {selection-background-color: #91B4FF;}";
        }

    }

    tabDetails->setStyleSheet(myStyleSheet);
    tabDetails->selectRow(laLigne);
    tabDetails->setSelectionMode(QAbstractItemView::SingleSelection);


}

void IHM_Tirages::slot_ccmr_AfficherMenu(const QPoint pos)
{
    QTableView *view = qobject_cast<QTableView *>(sender());

    emit sig_ShowMenu (pos, view);
}
