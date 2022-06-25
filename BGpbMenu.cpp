#ifndef QT_NO_DEBUG
#include <QDebug>
#include "BTest.h"
#endif

#include <QSqlDatabase>
#include <QSqlError>
#include <QMessageBox>
#include <QSqlQuery>

#include "BGpbMenu.h"
#include "Bc.h"
#include "db_tools.h"
/*
 * BGpbMenu::BGpbMenu(const int in_zn, const etCount in_typ, const QString cnx, BTbView *in_parent)
    :QGroupBox(nullptr),zn(in_zn),typ(in_typ),parent(in_parent)
*/

/*
 * Cette classe affiche un menu lorsque l'on clique sur le titre
 * du group box
 * cela permet de faire des selections sur
 * l'ensemble des elements du tableau
 * les requetes sur la table filtres associee a l'analyse
 * sont directement effectuees ici
 */

BGpbMenu::BGpbMenu(const BFlt *conf, BView_1 *in_parent)
 :QGroupBox(nullptr),BFlt(*conf), parent(in_parent)
{
 db_gbm = db_flt;

 //QString cnx_test = db_gbm .connectionName();

 zn=inf_flt->zne;
 tb_flt = inf_flt->tb_flt;
 tb_tirages = inf_flt->tb_ref;
 typ = inf_flt->typ;
 use_gpb = nullptr;

 prepareMenuData();
}

void  BGpbMenu::slot_ShowMenu(const QGroupBox *cible, const QPoint &p)
{
 if(typ==E_CountGrp)
  return;

 use_gpb = cible;
 QString info = cible->title();

 menu->setTitle(info);
 menu->popup(p);
}

void BGpbMenu::mousePressEvent ( QMouseEvent * event )
{
 Q_UNUSED(event)

 /// https://doc.qt.io/qt-5/signalsandslots.html
 /// https://openclassrooms.com/fr/courses/1894236-programmez-avec-le-langage-c/1899731-utilisez-les-signaux-et-les-slots

 emit sig_ShowMenu(this,event->screenPos().toPoint());
}

void BGpbMenu::prepareMenuData(void)
{
 QMenu * menu_l0 = new QMenu();

 QString lib[] = {"Selection", "Totalité"};
 QString itm[] = {"Reset", "Reserver", "Choisir", "Filtrer"};

 int nb_lib = sizeof(lib)/sizeof(QString);
 int nb_itm = sizeof(itm)/sizeof(QString);

 for(int lib_id=0;lib_id<nb_lib;lib_id++){
  /// top level menu
  QMenu * menu_l1 = new QMenu(lib[lib_id]);
  menu_l0->addMenu(menu_l1);

  QActionGroup *act_grp = new  QActionGroup(menu_l1);
  QString act_grp_name = "lib_"+QString::number(lib_id).rightJustified(2,'0');
  act_grp->setObjectName(act_grp_name);

  /// sub menu info
  for(int itm_id = 0; itm_id < nb_itm; itm_id++){
   QAction *one_action = new QAction(itm[itm_id],act_grp);
   one_action->setCheckable(true);
   menu_l1->addAction(one_action);
   act_grp->addAction(one_action)->setData(itm_id+1);
  }
  menu_l0->addSeparator();

  /// connect slot
  connect(act_grp,SIGNAL(triggered(QAction*)),this,SLOT(slot_ManageFlts(QAction*)));
 }
 menu = menu_l0;
}

void BGpbMenu::slot_ManageFlts(QAction *all_cmd)
{
 QActionGroup *origin = qobject_cast<QActionGroup *>(sender());
 QString name = origin->objectName();
 QStringList tmp = name.split('_');
 int  src_grp = -1;
 bool use_selection = true;

 if(tmp.size()==2){
  src_grp = QString(tmp.at(1)).toInt();
  if(src_grp){
   use_selection = false;
  }
 }

 Bp::F_Flts flt_def = Bp::noFlt;

 int req = all_cmd->data().toInt();

 switch (req) {

  case 1:
   flt_def = ~(Bp::fltWanted | Bp::fltSelected | Bp::fltFiltred);
   break;
  case 2:
   flt_def =  ~(Bp::fltSelected | Bp::fltFiltred) & Bp::fltWanted ;
   break;

  case 3:
   flt_def = (Bp::fltWanted|Bp::fltSelected) & ~Bp::fltFiltred;
   break;

  case 4:
   flt_def = Bp::fltWanted|Bp::fltSelected|Bp::fltFiltred;
   break;
  default:
   ;//rien
 }

 setNewFlt(flt_def,use_selection);

 parent->updateTitle(menu->title());
}

void BGpbMenu::setNewFlt(Bp::F_Flts flt_def, bool only_selection)
{
 bool b_retVal = true;
 QSqlQuery query(db_gbm);

 QString sql_msg = "";

 /// Rechercher ce qui manque dans table Filtres
 /// le rajouter
 /// effectuer le traitement flt
 ///
 /// https://www.sqlitetutorial.net/sqlite-intersect/
 /// https://www.sqlitetutorial.net/sqlite-except/
 /// https://www.sqlitetutorial.net/sqlite-insert/
 /// https://www.sqlitetutorial.net/sqlite-update/

 /*
  * insert into filtres
  * with
  * t1 as (select tb1.id as id from (r_B_fdj_elm_z1) as tb1),
  * t2 as (select tb1.val as val from (filtres) as tb1 where ((tb1.zne=0) and (tb1.typ=1))),
  * t3 as (select t1.id from t1 EXCEPT SELECT t2.val from t2)
  *
  * select NULL,0,1,10,t3.id,t3.id,-2,0 from t3
  */

 sql_msg = "insert into "+tb_flt+
       "  "
       "with  "
       "t1 as (select tb1.id as id from (r_"+tb_tirages+
       "_"+BCount::label[typ]+
       "_z"+QString::number(zn+1)+
       ") as tb1), "
       "t2 as (select tb1.val as val from ("+tb_flt+
       ") as tb1 where ((tb1.zne="+QString::number(zn)+
       " and (tb1.typ="+QString::number(typ)+
       ")))), "
       "t3 as (select t1.id from t1 EXCEPT SELECT t2.val from t2) "
       " "
       "select NULL,"+QString::number(zn)+
       ","+QString::number(typ)+
       ","+QString::number(typ*10)+
       ",t3.id,t3.id,-2,0 from t3 ";

#ifndef QT_NO_DEBUG
 QString filePrefix = "BGpbMenu";
 QString dbgFile = filePrefix +"_err.txt";
 BTest::writetoFile(dbgFile,sql_msg,false);
#endif

 b_retVal = query.exec(sql_msg);

 if(b_retVal){
  QString only_selected = "";

  /*
   * update filtres  set flt=(flt & ~0x1C) |(0x8) where ((zne=0) and (typ=1))
   */

  if(only_selection){
   /// Verifier si il y a une selection
   QItemSelectionModel *cur_selection = parent->selectionModel();
   QModelIndexList lst_cells = cur_selection->selectedIndexes();
   if(lst_cells.size()){
    QModelIndex un_index;
    QStringList lstBoules;
    QString lst_selections = "";

    /// Parcourir les selections
    foreach(un_index, lst_cells)
    {
     const QAbstractItemModel * pModel = un_index.model();
     int value = pModel->index(un_index.row(), 0).data().toInt();
     // Construire la liste des boules
     lstBoules << QString::number(value);
    }
    lst_selections = lstBoules.join(",");

    only_selected = " and (col in("+lst_selections+"))";
   }
  }
  /// Mettre la nouvelle valeur de filtre
  Bp::F_Flts msk = (Bp::fltWanted|Bp::fltSelected|Bp::fltFiltred) ;
  unsigned int val_msk = msk.operator unsigned int() & 0xFFFF;
  unsigned int neg_msk = ~val_msk;

  unsigned int val_flt = flt_def.operator unsigned int() & 0xFFFF;
  val_flt = val_flt & val_msk;

  sql_msg = "update "+tb_flt+
        "  set flt=(flt & 0x"+QString::number(neg_msk,16)+
        ") | (0x"+QString::number(val_flt,16)+
        ") where ((zne="+QString::number(zn)+
        ") and (typ="+QString::number(typ)+
        ")"+only_selected+
        ")";
#ifndef QT_NO_DEBUG
  BTest::writetoFile(dbgFile,sql_msg,true);
#endif

  b_retVal = query.exec(sql_msg);
 }

 if(b_retVal==false){
  DB_Tools::DisplayError("BGpbMenu::setNewFlt",&query,sql_msg);
 }

}
