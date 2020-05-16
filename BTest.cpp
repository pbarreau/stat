#ifndef QT_NO_DEBUG
#include <QDebug>
#include <QSqlError>
#include <QIODevice>

#include "BTest.h"
#include "db_tools.h"

QString BTest::getFieldsFromZone(const stGameConf *pGame, int zn, QString alias, bool visual)
{
 int len_zn = pGame->limites[zn].len;

 QString use_alias = "";

 if(alias.size()){
  use_alias = alias+".";
 }

 QString ref="";
 QString elm = pGame->names[zn].abv+"%1";
 if(visual){
  ref = "printf(\"%02d\","+use_alias+elm+") as "+elm;
 }
 else {
  ref = use_alias+elm;
 }
 //QString ref = use_alias+pGame->names[zn].abv+"%1";


 QString st_items = "";
 for(int i=0;i<len_zn;i++){
  st_items = st_items + ref.arg(i+1);
  if(i<(len_zn-1)){
   st_items=st_items+QString(",");
  }
 }

#ifndef QT_NO_DEBUG
 qDebug() <<st_items;
#endif

 return   st_items;
}

bool BTest::montestRapideSql(const stGameConf *pGame, int zn, int loop)
{
 bool b_retVal = true;
 QString cnx=pGame->db_ref->cnx;

 // Etablir connexion a la base
 QSqlDatabase db_1 = QSqlDatabase::database(cnx);
 if(db_1.isValid()==false){
  QString str_error = db_1.lastError().text();
  QMessageBox::critical(nullptr, cnx, str_error,QMessageBox::Yes);
  DB_Tools::DisplayError("BTest::montestRapideSql",nullptr,"");
  return b_retVal;
 }

 QSqlQuery query(db_1);
 QString sql_msg = "";
 QString st_cols = BTest::getFieldsFromZone(pGame,zn,"t2");

 QString ref_0 = "(t%1.id)";
 QString ref_1 = ref_0 + " as %2%3";
 QString ref_2 = "(tb0) as t%1";
 QString ref_3 = "(" + ref_0 + " < (t%2.id))";
 QString ref_4 = "t1.%1%2";
 QString ref_5 = ref_4 + " in (" + st_cols + ")";
 QString ref_6 = ref_4 + " asc";
 QString ref_7 = "(tb4) as t%1";
 QString ref_8 = "(t%1.uid = t%2.uid)";

 QString r0 = "";
 QString r1 = "";
 QString r2 = "";
 QString r3 = "";
 QString r4 = "";
 QString r5 = "";
 QString r6 = "";
 QString r7 = "";
 QString r8 = "";

 int nb_loop = loop;
 for (int i = 0; i< nb_loop; i++) {
  r0 = r0 + ref_0.arg(i+1);
  r1 = r1 + ref_1.arg(i+1).arg(pGame->names[zn].abv).arg(i+1);
  r2 = r2 + ref_2.arg(i+1);
  r4 = r4 + ref_4.arg(pGame->names[zn].abv).arg(i+1);
  r5 = r5 + ref_5.arg(pGame->names[zn].abv).arg(i+1);
  r6 = r6 + ref_6.arg(pGame->names[zn].abv).arg(i+1);
  r7 = r7 + ref_7.arg(i+1);

	if(i<nb_loop-1){
	 r0 = r0+",";
	 r1 = r1+",";
	 r2 = r2+",";
	 r4 = r4+",";
	 r5 = r5+" and";
	 r6 = r6+",";
	 r7 = r7+",";
	}

	if(i<=nb_loop-2){
	 r3 = r3 + ref_3.arg(i+1).arg(i+2);
	 r8 = r8 + ref_8.arg(i+1).arg(i+2);

	 if((i+2)< nb_loop){
		r3 = r3 + " and";
		r8 = r8 + " and";
	 }
	}

	r0 = r0 + "\n";
	r1 = r1 + "\n";
	r2 = r2 + "\n";
	r3 = r3 + "\n";
	r4 = r4 + "\n";
	r5 = r5 + "\n";
	r6 = r6 + "\n";
	r7 = r7 + "\n";
 }

 sql_msg = sql_msg + "  tb0 as\n";
 sql_msg = sql_msg + "  (\n";
 sql_msg = sql_msg + "    select\n";
 sql_msg = sql_msg + "      t1.id\n";
 sql_msg = sql_msg + "    from\n";
 sql_msg = sql_msg + "      (B_elm) as t1 ,\n";
 sql_msg = sql_msg + "      (B_fdj) as t2\n";
 sql_msg = sql_msg + "    where\n";
 sql_msg = sql_msg + "      (\n";
 sql_msg = sql_msg + "        (\n";
 sql_msg = sql_msg + "          t2.id=1\n";
 sql_msg = sql_msg + "        )\n";
 sql_msg = sql_msg + "        and t1.z1 in(" + st_cols + " )\n";
 sql_msg = sql_msg + "      )\n";
 sql_msg = sql_msg + "  )\n";
 QString tb0 = sql_msg;

 sql_msg ="";
 sql_msg = sql_msg + "  tb1 as\n";
 sql_msg = sql_msg + "  (\n";
 sql_msg = sql_msg + "    SELECT\n";
 sql_msg = sql_msg + "      (row_number() over()) as uid ,\n";
 sql_msg = sql_msg + r1;
 sql_msg = sql_msg + "    FROM\n";
 sql_msg = sql_msg + r2;
 sql_msg = sql_msg + "    WHERE\n";
 sql_msg = sql_msg + "      (\n";
 sql_msg = sql_msg + r3;
 sql_msg = sql_msg + "      )\n";
 sql_msg = sql_msg + "    ORDER BY\n";
 sql_msg = sql_msg + r0;
 sql_msg = sql_msg + "  )\n";
 QString tb1 = sql_msg;

 sql_msg ="";
 sql_msg = sql_msg + "  tb2 as\n";
 sql_msg = sql_msg + "  (\n";
 sql_msg = sql_msg + "    select\n";
 sql_msg = sql_msg + "      t1.uid                                             ,\n";
 sql_msg = sql_msg + "      (row_number() over ( partition by t1.uid )) as lgn ,\n";
 sql_msg = sql_msg + "      t2.*\n";
 sql_msg = sql_msg + "    from\n";
 sql_msg = sql_msg + "      (tb1)   as t1 ,\n";
 sql_msg = sql_msg + "      (B_fdj) as t2\n";
 sql_msg = sql_msg + "    where\n";
 sql_msg = sql_msg + "      (\n";
 sql_msg = sql_msg + r5;
 sql_msg = sql_msg + "      )\n";
 sql_msg = sql_msg + "    order by\n";
 sql_msg = sql_msg + "      t1.uid asc\n";
 sql_msg = sql_msg + "  )\n";
 QString tb2 = sql_msg;

 sql_msg ="";
 sql_msg = sql_msg + "  tb2Count as\n";
 sql_msg = sql_msg + "  (\n";
 sql_msg = sql_msg + "    select\n";
 sql_msg = sql_msg + "      t1.uid ,\n";
 sql_msg = sql_msg + r4;
 sql_msg = sql_msg + "             ,\n";
 sql_msg = sql_msg + "      count(t2.uid) as T\n";
 sql_msg = sql_msg + "    from\n";
 sql_msg = sql_msg + "      (tb1) as t1 ,\n";
 sql_msg = sql_msg + "      (tb2) as t2\n";
 sql_msg = sql_msg + "    where\n";
 sql_msg = sql_msg + "      (\n";
 sql_msg = sql_msg + "        t1.uid=t2.uid\n";
 sql_msg = sql_msg + "      )\n";
 sql_msg = sql_msg + "    group by\n";
 sql_msg = sql_msg + "      t2.uid\n";
 sql_msg = sql_msg + "    order by\n";
 sql_msg = sql_msg + "      T desc    ,\n";
 sql_msg = sql_msg + r6;
 sql_msg = sql_msg + "  )\n";
 QString tb2c = sql_msg;

 sql_msg ="";
 sql_msg = sql_msg + "  tb3 as\n";
 sql_msg = sql_msg + "  (\n";
 sql_msg = sql_msg + "    select\n";
 sql_msg = sql_msg + "      t1.uid                                             ,\n";
 sql_msg = sql_msg + "      (row_number() over ( partition by t1.uid )) as lgn ,\n";
 sql_msg = sql_msg + "      t2.*\n";
 sql_msg = sql_msg + "    from\n";
 sql_msg = sql_msg + "      (tb2)   as t1 ,\n";
 sql_msg = sql_msg + "      (B_fdj) as t2\n";
 sql_msg = sql_msg + "    where\n";
 sql_msg = sql_msg + "      (\n";
 sql_msg = sql_msg + "        t2.id = t1.id-1\n";
 sql_msg = sql_msg + "      )\n";
 sql_msg = sql_msg + "  )\n";
 QString tb3 = sql_msg;

 sql_msg ="";
 sql_msg = sql_msg + "  tb4 as\n";
 sql_msg = sql_msg + "  (\n";
 sql_msg = sql_msg + "    select\n";
 sql_msg = sql_msg + "      t2.uid                                             ,\n";
 sql_msg = sql_msg + "      (row_number() over ( partition by t2.uid )) as lgn ,\n";
 sql_msg = sql_msg + "      t1.id\n";
 sql_msg = sql_msg + "    from\n";
 sql_msg = sql_msg + "      (B_elm) as t1 ,\n";
 sql_msg = sql_msg + "      (tb3)   as t2\n";
 sql_msg = sql_msg + "    where\n";
 sql_msg = sql_msg + "      (\n";
 sql_msg = sql_msg + "        t1.z1 in(" + st_cols + ")\n";
 sql_msg = sql_msg + "      )\n";
 sql_msg = sql_msg + "    group by\n";
 sql_msg = sql_msg + "      t2.uid ,\n";
 sql_msg = sql_msg + "      t1.id\n";
 sql_msg = sql_msg + "  )\n";
 QString tb4 = sql_msg;

 sql_msg ="";
 sql_msg = sql_msg + "  tb5 as\n";
 sql_msg = sql_msg + "  (\n";
 sql_msg = sql_msg + "    SELECT\n";
 sql_msg = sql_msg + "      t1.uid                                             ,\n";
 sql_msg = sql_msg + "      (row_number() over ( partition by t1.uid )) as lgn ,\n";
 sql_msg = sql_msg + r1;
 sql_msg = sql_msg + "    FROM\n";
 sql_msg = sql_msg + r7;
 sql_msg = sql_msg + "    WHERE\n";
 sql_msg = sql_msg + "      (\n";
 sql_msg = sql_msg + r8;
 sql_msg = sql_msg + "        and\n";
 sql_msg = sql_msg + r3;
 sql_msg = sql_msg + "      )\n";
 sql_msg = sql_msg + "    order by\n";
 sql_msg = sql_msg + r0;
 sql_msg = sql_msg + "  )\n";
 QString tb5 = sql_msg;

 sql_msg ="";
 sql_msg = sql_msg + "  tb6 as\n";
 sql_msg = sql_msg + "  (\n";
 sql_msg = sql_msg + "    select\n";
 sql_msg = sql_msg + "      t1.uid                                             ,\n";
 sql_msg = sql_msg + "      (row_number() over ( partition by t1.uid )) as pos ,\n";
 sql_msg = sql_msg + "      t1.lgn                                             ,\n";
 sql_msg = sql_msg + r4;
 sql_msg = sql_msg + "      ,count(*) as T\n";
 sql_msg = sql_msg + "    from\n";
 sql_msg = sql_msg + "      (tb5) as t1 ,\n";
 sql_msg = sql_msg + "      (tb3) as t2\n";
 sql_msg = sql_msg + "    where\n";
 sql_msg = sql_msg + "      (\n";
 sql_msg = sql_msg + "        (\n";
 sql_msg = sql_msg + "          t1.uid = t2.uid\n";
 sql_msg = sql_msg + "        )\n";
 sql_msg = sql_msg + "        and\n";
 sql_msg = sql_msg + r5;
 sql_msg = sql_msg + "      )\n";
 sql_msg = sql_msg + "    group by\n";
 sql_msg = sql_msg + "      t2.uid ,\n";
 sql_msg = sql_msg + "      t1.lgn\n";
 sql_msg = sql_msg + "    order by\n";
 sql_msg = sql_msg + "      T desc     ,\n";
 sql_msg = sql_msg + "      t1.uid asc ,\n";
 sql_msg = sql_msg + r6;
 sql_msg = sql_msg + "  )\n";
 QString tb6 = sql_msg;

 sql_msg = "";

 sql_msg = sql_msg + "with\n";
 sql_msg = sql_msg + "  -- boules dernier tirages\n";
 sql_msg = sql_msg + tb0;
 sql_msg = sql_msg + "  ,\n";
 sql_msg = sql_msg + "  -- uplet 2 de ces boules\n";
 sql_msg = sql_msg + tb1;
 sql_msg = sql_msg + "  ,\n";
 sql_msg = sql_msg + "  -- ligne contenant ces\n";
 sql_msg = sql_msg + "  -- uplets\n";
 sql_msg = sql_msg + tb2;
 sql_msg = sql_msg + "  ,\n";
 sql_msg = sql_msg + "  -- comptage\n";
 sql_msg = sql_msg + tb2c;
 sql_msg = sql_msg + "  ,\n";
 sql_msg = sql_msg + "  -- jour suivant de ces\n";
 sql_msg = sql_msg + "  -- uplets\n";
 sql_msg = sql_msg + tb3;
 sql_msg = sql_msg + "  ,\n";
 sql_msg = sql_msg + "  -- nouveaux uplets\n";
 sql_msg = sql_msg + "  -- creer une nouvelle\n";
 sql_msg = sql_msg + "  -- liste de boules\n";
 sql_msg = sql_msg + "  -- pour chacun des uid du\n";
 sql_msg = sql_msg + "  -- depart\n";
 sql_msg = sql_msg + tb4;
 sql_msg = sql_msg + "  ,\n";
 sql_msg = sql_msg + "  -- pour chaque ensemble\n";
 sql_msg = sql_msg + "  -- construit\n";
 sql_msg = sql_msg + "  -- creer la nouvelle liste\n";
 sql_msg = sql_msg + "  -- uplets\n";
 sql_msg = sql_msg + tb5;
 sql_msg = sql_msg + "  ,\n";
 sql_msg = sql_msg + "  -- compter les nouveaux\n";
 sql_msg = sql_msg + "  -- uplets\n";
 sql_msg = sql_msg + "  -- dans la listes des\n";
 sql_msg = sql_msg + "  -- tirages futures\n";
 sql_msg = sql_msg + "  -- leurs correspondant\n";
 sql_msg = sql_msg + tb6;
 sql_msg = sql_msg + "select \n";
 sql_msg = sql_msg + "  t1.*\n";
 sql_msg = sql_msg + "from\n";
 sql_msg = sql_msg + "  (tb6) as t1\n";

 if((b_retVal=query.exec(sql_msg))==false){
  DB_Tools::DisplayError("BTest::montestRapideSql",&query,sql_msg);

#ifndef QT_NO_DEBUG
	qDebug() <<tb0;
	qDebug() <<tb1;
	qDebug() <<tb2;
	qDebug() <<tb2c;
	qDebug() <<tb3;
	qDebug() <<tb4;
	qDebug() <<tb5;
	qDebug() <<tb6;
	qDebug() <<sql_msg;
#endif
 }

 writetoFile("Mes_Requetes.txt", "\n\n------------\n");
 writetoFile("Mes_Requetes.txt", sql_msg);
 return b_retVal;
}

void BTest::writetoFile(QString file_name, QString msg, bool append)
{
 QFile file(file_name);
 QIODevice::OpenMode mode = QIODevice::OpenModeFlag::NotOpen;

 if(append==true){
  mode = QIODevice::OpenModeFlag::Append;
 }
 else {
  mode = QIODevice::OpenModeFlag::WriteOnly;
 }

 if (!file.open(mode)){
  /// WriteOnly ou Append
  /// https://openclassrooms.com/forum/sujet/qt-ecrire-un-texte-dans-un-fichier-txt-75563
  QMessageBox::information(nullptr, "Pgm", "BTest::writetoFile!!",QMessageBox::Yes);
 }

 QTextStream msg_logs(&file);
 msg_logs << msg;
}
#endif
