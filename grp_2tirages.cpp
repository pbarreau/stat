#include "SyntheseGenerale.h"

void SyntheseGenerale::slot_grpSel(const QModelIndex &index)
{
 QString st_titre = "";
 QStringList **pList = tabEcarts->getSqlGrp();

 QString st_critere = "";
 QString sqlReq =*(uneDemande.st_LDT_Depart);
 QTableView *view = qobject_cast<QTableView *>(sender());
 QTableView *ptrSel = NULL;
 QList < QTabWidget *> *id_tab[2]={NULL};

 int *path = getPathToView(view, &id_tab[0], &ptrSel);

 QItemSelectionModel *selectionModel = view->selectionModel();
 QAbstractItemModel *sqm_tmp = qobject_cast<QSqlQueryModel *>(view->model());
 int nbcol = sqm_tmp->columnCount();
 //int nbJ = nbcol - BDelegateCouleurFond::Columns::TotalElement -3;
 //QSortFilterProxyModel *m = qobject_cast<QSortFilterProxyModel *>(view->model());



 /// il y a t'il une selection
 QModelIndexList indexes =  selectionModel->selectedIndexes();

 if(indexes.size())
 {


  QModelIndex un_index;
  int curCol = 0;
  int occure = 0;

  /// Parcourir les selections
  foreach(un_index, indexes)
  {
   curCol = un_index.model()->index(un_index.row(), un_index.column()).column();
   occure = un_index.model()->index(un_index.row(), 0).data().toInt();
   if(curCol)
   {
    st_critere = "("+pList[path[0]][0].at(curCol-1)+")";
    sqlReq =grp_TrouverTirages(curCol,occure,sqlReq,st_critere,path[0]);
   }
  }
 }

#if 0
 /// on informe !!!
 if(st_titre!="")
 {
  st_titre.remove(st_titre.length()-1,1);
  // signaler que cet objet a construit la requete
  sqlReq = "/*CAS "+unNom+" */"
           + sqlReq +
           "/*FIN CAS "+unNom+" */";

  a.db_data = sqlReq;
  a.tb_data = "g"+QString::number(total)+":"+st_titre;
  emit sig_ComptageReady(a);
 }
#endif

 //----------
 stCurDemande *etude = new stCurDemande;
 *etude = uneDemande;


 etude->origine = Tableau2;
 etude->db_cnx = db_0.connectionName();

 etude->st_titre = "test";//CreatreTitle(&uneDemande);
 etude->st_TablePere = REF_BASE;
 etude->cur_dst = 0;
 etude->req_niv = 0;
 etude->st_Ensemble_1 = uneDemande.st_Ensemble_1;
 etude->ref = uneDemande.ref;
 etude->st_LDT_Filtre = new QString;
 etude->st_jourDef = new QString;
 *(etude->st_jourDef) = CompteJourTirage(db_0.connectionName());
 *(etude->st_LDT_Filtre) = sqlReq;
 etude->barycentre = mon_brc_tmp;

#ifndef QT_NO_DEBUG
 qDebug()<<etude->st_titre;
 qDebug()<<etude->st_TablePere;
 qDebug()<<*(etude->st_Ensemble_1);
 qDebug()<<*(etude->st_LDT_Filtre);
 qDebug()<<*(etude->st_jourDef);
 qDebug()<<"etude->st_jourDef";
#endif


 // Nouvelle de fenetre de detail de cette selection
 SyntheseDetails *unDetail = new SyntheseDetails(etude,pEcran,ptabTop);
 connect( ptabTop, SIGNAL(tabCloseRequested(int)) ,
          unDetail, SLOT(slot_FermeLaRecherche(int) ) );


}


QString SyntheseGenerale::grp_TrouverTirages(int col, int nb, QString st_tirages, QString st_cri, int zn)
{
 static int demande=0;

 QString st_tmp =  getFieldsFromZone(zn,"tb1");//CriteresCreer("=","or",zn);
 QString st_return =
   "/*S"+QString::number(demande)+"a '"+st_cri+"'*/"+
   "select tb1.*, count(tb2.B) as N"+QString::number(col)+ " "+
   "from (" + st_tirages.remove(";")+
   ") as tb1 "
   "left join "
   "("
   "select id as B from Bnrz where (z"+QString::number(zn+1)+
   " not null  and ("+st_cri+")) ) as tb2 " +
   "on "+
   "(tb2.b in("
   +st_tmp+
   ")) group by tb1.id"+
   "/*S"+QString::number(demande)+"a*/";


 st_return =
   "/*S"+QString::number(demande)+"b*/"+
   "select * from("+
   st_return+
   ")as tb1 where(tb1.N"+QString::number(col)+ "="+
   QString::number(nb)+")/*S"+QString::number(demande)+"b*/;";

 demande++;

#ifndef QT_NO_DEBUG
 qDebug() << st_return;
#endif

 return(st_return);
}

