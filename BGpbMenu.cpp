#ifndef QT_NO_DEBUG
#include <QDebug>
#endif

#include <QSqlDatabase>
#include <QSqlError>
#include <QMessageBox>
#include <QSqlQuery>

#include "BGpbMenu.h"
#include "compter.h"
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

BGpbMenu::BGpbMenu(const BFlt *conf, BTbView *in_parent)
    :QGroupBox(nullptr),BFlt(*conf), parent(in_parent)///zn(in_zn),typ(in_typ),parent(in_parent)
{
 db_gbm = db_flt;

 //QString cnx_test = db_gbm .connectionName();

 zn=inf_flt->zne;
 tb_flt = inf_flt->tb_flt;
 tb_tirages = inf_flt->tb_ref;
 typ = inf_flt->typ;
 use_gpb = nullptr;

 gbm_Menu();
}

void  BGpbMenu::slot_ShowMenu(const QGroupBox *cible, const QPoint &p)
{
 if(typ==eCountGrp)
  return;

 use_gpb = cible;
 menu->popup(p);
}

void BGpbMenu::mousePressEvent ( QMouseEvent * event )
{
 Q_UNUSED(event)

 /// https://doc.qt.io/qt-5/signalsandslots.html
 /// https://openclassrooms.com/fr/courses/1894236-programmez-avec-le-langage-c/1899731-utilisez-les-signaux-et-les-slots

 emit sig_ShowMenu(this,event->screenPos().toPoint());
}

void BGpbMenu::gbm_Menu(void)
{
 menu = new QMenu("All");

 QString choix[]={"tous : Reset","tous : Reserver", "tous : Choisir", "tous : Filtrer"};
 int nb_chx = sizeof(choix)/sizeof(QString);

 menu->addSection("ALL");
 menu->addSeparator();
 QActionGroup *usr_sel = new  QActionGroup(menu);

 for(int i =1; i<=nb_chx;i++)
 {
  QAction *radio = new QAction(choix[i-1],usr_sel);
  radio->setCheckable(true);
  menu->addAction(radio);
  usr_sel->addAction(radio)->setData(i);
 }
 connect(usr_sel,SIGNAL(triggered(QAction*)),this,SLOT(slot_ManageFlts(QAction*)));
}

void BGpbMenu::slot_ManageFlts(QAction *all_cmd)
{
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

 setNewFlt(flt_def);

 parent->updateTitle();
}

void BGpbMenu::setNewFlt(Bp::F_Flts flt_def)
{
 bool b_retVal = true;
 QSqlQuery query(db_gbm);

 QString msg = "";

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

 msg = "insert into "+tb_flt+
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
 qDebug() << msg;
#endif

 b_retVal = query.exec(msg);

 if(b_retVal){
  /*
   * update filtres  set flt=(flt & ~0x1C) |(0x8) where ((zne=0) and (typ=1))
   */

	/// Mettre la nouvelle valeur de filtre
	Bp::F_Flts msk = (Bp::fltWanted|Bp::fltSelected|Bp::fltFiltred) ;
	unsigned int val_msk = msk.operator unsigned int() & 0xFFFF;
	unsigned int neg_msk = ~val_msk;

	unsigned int val_flt = flt_def.operator unsigned int() & 0xFFFF;
	val_flt = val_flt & val_msk;

	msg = "update "+tb_flt+
				"  set flt=(flt & 0x"+QString::number(neg_msk,16)+
				") | (0x"+QString::number(val_flt,16)+
				") where ((zne="+QString::number(zn)+
				") and (typ="+QString::number(typ)+
				"))";
#ifndef QT_NO_DEBUG
	qDebug() << msg;
#endif

  b_retVal = query.exec(msg);
 }

 if(b_retVal==false){
  DB_Tools::DisplayError("BGpbMenu::setNewFlt",&query,msg);
 }

}
