#ifndef QT_NO_DEBUG
#include <QDebug>
#endif

#include <QSortFilterProxyModel>
#include <QSqlQueryModel>

#include <QHeaderView>
#include <QVBoxLayout>

#include <QMessageBox>

#include "BTbView.h"

#include "db_tools.h"

BTbView::BTbView(int in_zn, etCount in_typ, QString cnx, QTableView * parent)
    :QTableView(parent),zn(in_zn),cal(in_typ)
{
 // Etablir connexion a la base
 db_tbv = QSqlDatabase::database(cnx);
 if(db_tbv.isValid()==false){
  QString str_error = db_tbv.lastError().text();
  QMessageBox::critical(nullptr, cnx, str_error,QMessageBox::Yes);
  return;
 }

 myGpb = new BGpbMenu(cnx);
 construireMenu();
}

BTbView::~BTbView()
{
 delete myGpb;
}

QString BTbView::mkTitle(int zn, etCount eCalcul, QTableView *view)
{
 QSortFilterProxyModel *m = qobject_cast<QSortFilterProxyModel *>(view->model());
 QSqlQueryModel  * sqm_tmp = qobject_cast<QSqlQueryModel  *>(m->sourceModel());

 QString title = "";
 QString ref_info = "Initial : %1, R(%1), C(%1), F(%1), Disponible(s) : %1";
 QStringList tmp_lst = ref_info.split(",");

 Bp::F_Flts msk = Bp::F_Flt::fltWanted|Bp::F_Flt::fltSelected|Bp::F_Flt::fltFiltred;
 Bp::F_Flts keys[]={Bp::F_Flt::fltWanted,Bp::F_Flt::fltSelected,Bp::F_Flt::fltFiltred};
 int nb_items = sizeof (keys)/sizeof(Bp::F_Flts);

 /// Necessaire pour compter toutes les lignes de reponses
 while (sqm_tmp->canFetchMore())
 {
  sqm_tmp->fetchMore();
 }
 /// Determination nb ligne par proxymodel
 int nb_lgn_ftr = m->rowCount();
 if(eCalcul==eCountGrp){
  int count=view->horizontalHeader()->count();
  nb_lgn_ftr = nb_lgn_ftr * count;
 }

 title = tmp_lst[0].arg(nb_lgn_ftr)+", ";

 /// interrogation table Filtres
 /*
  * Select count(id) as T from Filtres
  * where (
  * (zne=0) and
  * (typ=1) and
  * ( ((flt & 0x1C) & 0x4) = 0x4))
  */

 bool b_retVal = true;
 QSqlQuery query(db_tbv);
 int memo_choisi = -1;
 QString tblFlt = "Filtres";
 QString msg = "";
 QString msg_1 ="Select count(id) as T from "+
                 tblFlt+
                 " where ( (zne="+
                 QString::number(zn)+
                 ") and (typ="+
                 QString::number(eCalcul)+
                 ")";

 for(int i = 0; (i< nb_items) && b_retVal ; i++){
  QString msg_2 = " and ( ((flt & 0x"+
                  QString::number(msk,16).toUpper()+
                  ") & 0x"+
                  QString::number(keys[i],16).toUpper()+
                  ") = 0x"+
                  QString::number(keys[i],16).toUpper()+
                  "))";

  msg= msg_1+msg_2;

#ifndef QT_NO_DEBUG
	qDebug()<< "msg_1:"<<msg_1;
	qDebug()<< "msg_2:"<<msg_2;
	qDebug()<< "msg:"<<msg;
#endif

	b_retVal = query.exec(msg);

	int total = 0;
	if(query.first()){
	 total = query.value(0).toInt();
	}
	title = title + tmp_lst[i+1].arg(total)+", ";

	if(i==1){
	 memo_choisi = total;
	}

 }
 title = title+ tmp_lst[tmp_lst.size()-1].arg(nb_lgn_ftr-memo_choisi);

 if(!b_retVal){
  DB_Tools::DisplayError("BCount::mkTitle",&query,msg);
  QMessageBox::warning(nullptr,"BCount","mkTitle",QMessageBox::Ok);
 }

 QString st_total = ref_info.arg(nb_lgn_ftr);

 return title;
}


void BTbView::updateTitle()
{
 /// Mettre ensuite l'analyse du marquage
 QString st_total = mkTitle(zn,cal,this);
 myGpb->setTitle(st_total);
}

/*
void  BTbView::slot_ShowMenu(const QGroupBox *cible)
{
 menu->popup(cible->mapToGlobal(pos()));
}

void BTbView::mousePressEvent ( QMouseEvent * event )

{
 emit sig_ShowMenu(myGpb);
}
*/

void BTbView::construireMenu(void)
{

 menu = new QMenu();

 QAction *isWanted = menu->addAction("Reserver",this,SLOT(slot_isWanted(bool)));
 isWanted->setCheckable(true);
 isWanted->setEnabled(true);

 QAction *isChoosed = menu->addAction("Choisir",this,SLOT(slot_isChoosed(bool)));
 isChoosed->setCheckable(true);
 isChoosed->setDisabled(true);

 QAction *isFiltred = menu->addAction("Filtrer",this,SLOT(slot_isFiltred(bool)));
 isFiltred->setCheckable(true);
 isFiltred->setDisabled(true);
}

BGpbMenu *BTbView::getGpb()
{
 return myGpb;
}

QGroupBox * BTbView::getScreen()
{
 /// Mettre ensuite l'analyse du marquage
 updateTitle();

 QVBoxLayout *layout = new QVBoxLayout;
 layout->addWidget(this, Qt::AlignCenter|Qt::AlignTop);
 myGpb->setLayout(layout);

 return myGpb;
}
