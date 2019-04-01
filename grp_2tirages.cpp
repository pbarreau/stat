#include "SyntheseGenerale.h"

void SyntheseGenerale::slot_grpSel(const QModelIndex &index)
{
 QString st_titre = "";
 //QStringList **pList = tabEcarts->getSqlGrp();

 QString st_critere = "";
 QString sqlReq =*(uneDemande.st_LDT_Depart);
 QTableView *view = qobject_cast<QTableView *>(sender());
 QTableView *ptrSel = NULL;
 QList < QTabWidget *> *id_tab[2]={NULL};

 int *path = getPathToView(view, &id_tab[0], &ptrSel);

 QItemSelectionModel *selectionModel = view->selectionModel();
 QAbstractItemModel *sqm_tmp = qobject_cast<QSqlQueryModel *>(view->model());
 int nbcol = sqm_tmp->columnCount();



 /// il y a t'il une selection
 QModelIndexList indexes =  selectionModel->selectedIndexes();

 if(indexes.size())
 {
  sqlReq =grp_sqlFromSelection(view, path[0]);
#if 0
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
#endif
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

QString SyntheseGenerale::grp_sqlFromSelection(QTableView *view, int path)
{
 QString msg = "";
 QItemSelectionModel *selectionModel = view->selectionModel();
 QAbstractItemModel *sqm_tmp = qobject_cast<QSqlQueryModel *>(view->model());
 //int nbcol = sqm_tmp->columnCount();

 /// Parcourir les selections
 QList <QPair<int,stSelInfo*>*> cur_sel;
 QModelIndexList indexes =  selectionModel->selectedIndexes();
 QModelIndex un_index;
 //int curCol = 0;
 //int occure = 0;


 /// Effacer la selection precedente
 int nb_selLines = cur_sel.size();
 if(nb_selLines){
  /// liberer la memoire occupee par p
  qDeleteAll(cur_sel.begin(), cur_sel.end());

  /// puis detacher de la liste
  cur_sel.clear();
 }

 /// Regrouper selection selon colonnes
 foreach(un_index, indexes){
  int cur_key =un_index.model()->index(un_index.row(), un_index.column()).column();
  QPair<int,stSelInfo *> *p = NULL;

  /// Parcourir existant et rajout si necessaire
  bool isPresent = false;
  for(int pos=0; pos< cur_sel.size(); pos++){
   p=cur_sel.at(pos);
   if(p->first==cur_key){
    isPresent = true;
    p->second->lstSel->append(un_index);
    break;
   }
  }

  if(isPresent == false){
   p = new QPair<int,stSelInfo *>;
   stSelInfo *info = new stSelInfo;
   QModelIndexList *sel = new QModelIndexList;

   /// config
   sel->append(un_index);
   info->qtv = view;
   info->lstSel = sel;

   p->first = cur_key;
   p->second = info;

   /// rajout
   cur_sel.append(p);
  }

 }

 /// --------------
 QString items = "";
 QString where = "";
 QStringList allas;
 QString as_def = "";

 /// Parcourir la liste pour creer le code SQL
 int nbScanCol = cur_sel.size();
 for (int pos=0; pos < nbScanCol; pos++) {
  int key = cur_sel.at(pos)->first;
  QString msg_key = grp_SqlFromKey(path, key);
  as_def = "r_"+QString::number(pos);


  QModelIndexList *lesSelDelaCol = cur_sel.at(pos)->second->lstSel;
  QString msg_col = "";
  int nbSelInCol = lesSelDelaCol->size();
  for (int uneSel=0;uneSel< nbSelInCol; uneSel++) {
   un_index = lesSelDelaCol->at(uneSel);
   int occure = un_index.model()->index(un_index.row(), 0).data().toInt();
   msg_col = msg_col + QString::number(occure);
   if(uneSel<nbSelInCol-1){
    msg_col = msg_col + ",";
   }
  }
#ifndef QT_NO_DEBUG
  qDebug() << msg_col;
#endif

  items = items + "("+msg_key+")as "+as_def;
  where = where +"("+as_def+".N"+QString::number(key)+ " in("
          +msg_col+"))";
  allas = allas << as_def+".id";


#ifndef QT_NO_DEBUG
  qDebug() << items;
  qDebug() << where;
  qDebug() << allas;
#endif

  if(pos< nbScanCol -1){
   items = items + " , ";
   where = where + " and ";
  }
 }

 QString clef = "";
 QString cl_1 = "";
 QString cl_2 = "";
 QString cl_3 = "";
 int nbClef = allas.size();
 for (int loop=1;loop <= nbClef; loop++) {
  QString item = allas.at(loop-1);
  cl_1 = "("+item+")";

  if(loop%2 == 0){
   cl_2 = "("+allas.at(loop-2)+"="+item+")";
  }
  if(loop%3==0){
   cl_3 = cl_2 + " and ";
  }

  /// --------
  if(cl_3.size()){
   clef = clef + cl_3;
  }
  else if (cl_2.size()) {
   if(nbClef == 2){
    clef = cl_2;
   }
   else{
    clef = clef + "and"+ cl_2;
   }
  }else {
   clef = clef + cl_1;
  }
 }

 where = "("+clef+")and("+where+")";
 msg = "select "+as_def+".* from " + items + " where("+where+")";

#ifndef QT_NO_DEBUG
 qDebug() << msg;
#endif

 return msg;
}

QString SyntheseGenerale::grp_SqlFromKey(int zn, int col)
{

 QStringList **pList = tabEcarts->getSqlGrp();
 QString st_cri = pList[zn][0].at(col-1);
 QString st_tirages = *(uneDemande.st_LDT_Depart);

 QString st_tmp =  getFieldsFromZone(zn,"tb1");
 QString st_return =
   "/*D'"+st_cri+"'*/"
   +"select tb1.*, count(tb2.B) as N"+QString::number(col)
   +" from (" + st_tirages.remove(";")
   +") as tb1 "
    "left join "
    "("
    "select id as B from Bnrz where (z"+QString::number(zn+1)
   +" not null  and ("+st_cri+")) ) as tb2 " +
   "on "+
   "(tb2.b in("
   +st_tmp+
   ")) group by tb1.id"+
   "/*F'"+st_cri+"'*/";


 return st_return;
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
