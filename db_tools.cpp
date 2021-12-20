#ifndef QT_NO_DEBUG
#include <QDebug>
#include <QSqlError>
#include "BTest.h"
#endif

#include <QFile>
#include <QString>
#include <QStringList>
#include <QMessageBox>
#include <QSqlError>
#include <QSqlQuery>
#include <QApplication>
#include <QTextStream>

#include <QThread>

#include "db_tools.h"

/// cette fonction construit une chaine sous contrainte
/// on peut mettre n indice (loop) apres tb1 si inc1 = vrai
/// idem pour chacun des elments de tb2 si inc2 = vrai
/// chacun des elements construit de tb1 sera mis en
/// relation avec 1 element de tb2 par l'operateur op1
/// chaque element de tb2 etant separé par op2
/// ex : F(5,"tb3.",true, " as ", lst, true, "," =
/// avec lst ayant b
/// 'tb3.b1 as b1,'...'tb5.b5 as b5,'
QString DB_Tools::GEN_Where_3(int loop,
															QString tb1,bool inc1,QString op1,
															QStringList &tb2,bool inc2,QString op2
															)
{
 QString ret_msg = "";
 QString ind_1 = "";
 QString ind_2 = "";

 QString flag = " and ";

 for(int j=0; j< tb2.size();j++)
 {
  ret_msg = ret_msg + "(";
  for(int i = 0; i<loop;i++)
  {
   // Mettre un nombre apres  1er table
   if(inc1)
   {
    ind_1 = tb1+QString::number(i+1);
   }
   else
   {
    ind_1 = tb1;
   }

   // Mettre un nombre apres  2eme table
   if(inc2)
   {
    ind_2 = tb2.at(j)+QString::number(i+1);
   }
   else
   {
    ind_2 = tb2.at(j);
   }

   // Construire message
   ret_msg = ret_msg
             + ind_1
             + op1
             + ind_2
             + " " + op2 + " ";
  }
  // retirer le dernier operateur (op2)
  ret_msg.remove(ret_msg.length()-op2.length()-1, op2.length()+1);

  ret_msg =  ret_msg + ")";
  ret_msg = ret_msg + flag;
 }
 // retirer le dernier operateur
 ret_msg.remove(ret_msg.length()-flag.length(),flag.length());

#ifndef QT_NO_DEBUG
 qDebug() << "GEN_Where_3\n";
 qDebug() << "SQL msg:\n"<<ret_msg<<"\n-------";
#endif

 return ret_msg;
}

/// http://www.sqlitetutorial.net/sqlite-primary-key/
bool DB_Tools::myCreateTableAs(QSqlQuery query, QString tblName, QString pid, QString asCode)
{
 bool b_retVal=false;
 QSqlQuery q_atom(query);
 QString msg = "select sql from sqlite_master where (type = 'table' and tbl_name='"+
               tblName+"'); ";
 //QString msg_s = "";
#if 0
 QString atomic[]=
 {
  {"PRAGMA foreign_keys=off;"},
  {"BEGIN TRANSACTION;"},
  {"ALTER TABLE "+tblName+" RENAME TO old_"+tblName+";"},
  {msg+";"},
  {"INSERT INTO "+tblName+" SELECT * FROM old_"+tblName+";"},
  {"drop table if exists old_"+tblName+";"},
  {"COMMIT;"},
  {"PRAGMA foreign_keys=on;"}
 };

#endif

 if((b_retVal=q_atom.exec(asCode))){
  if((b_retVal=q_atom.exec(msg))){
   q_atom.first();
   if((b_retVal=q_atom.isValid())){
    msg=q_atom.value(0).toString();
    msg = msg.simplified();
    msg = msg.replace("\"","'");
    msg = msg.remove("INT");
    msg = msg.replace(","," integer,");
    msg = msg.replace(")"," integer, primary key ("+pid+"))");
    QString atomic[]=
    {
     {"PRAGMA foreign_keys=off;"},
     {"BEGIN TRANSACTION;"},
     {"ALTER TABLE "+tblName+" RENAME TO old_"+tblName+";"},
     {msg+";"},
     {"INSERT INTO "+tblName+" SELECT * FROM old_"+tblName+";"},
     {"drop table if exists old_"+tblName+";"},
     {"COMMIT;"},
     {"PRAGMA foreign_keys=on;"}
    };
    int items = sizeof(atomic)/sizeof(QString);
    for(int item=0; (item<items) && b_retVal; item++){
     msg = atomic[item];
#ifndef QT_NO_DEBUG
     qDebug() << atomic[item];
#endif
     b_retVal = q_atom.exec(msg);

    }
   }
  }
 }
#ifndef QT_NO_DEBUG
 qDebug() << "SQL msg:\n"<<msg<<"\n-------";
#endif
 if(!b_retVal){
  QString ErrLoc = "DB_Tools::myCreateTableAs";
  DB_Tools::DisplayError(ErrLoc,&q_atom,msg);
 }

 return b_retVal;
}

#if 0
QString DB_Tools::makeTableFromSelect(QString select)
{
 QString msg = "";
 msg = select.simplified();
 msg = msg.remove("INT");
 msg = msg.replace(","," integer,");
 msg = msg + tr(",primary key (id)");
 return msg;
}
#endif

QString DB_Tools::innerJoin(stJoinArgs ja)
{
 QString arg1 = ja.arg1;
 QString arg2 = ja.arg2;
 QString arg3 = ja.arg3;
 QString arg4 = ja.arg4;
 QString msg = "";

 msg = "select " + arg1 + " from ("+arg2+")as tbLeft "
                                         "inner join ("+arg3+")as tbRight "
                                                             "on ("+arg4+")";

#ifndef QT_NO_DEBUG
 qDebug() << "DB_Tools::innerJoin\n";
 qDebug() << "msg:\n"<<msg<<"\n-------";
#endif

 return msg;
}

QString DB_Tools::leftJoin(stJoinArgs ja)
{
 QString arg1 = ja.arg1;
 QString arg2 = ja.arg2;
 QString arg3 = ja.arg3;
 QString arg4 = ja.arg4;
 QString msg = "";

 msg = "select "
       + arg1
       + " from ("
       +arg2
       +")as tbLeft "
        "left join ("
       +arg3
       +")as tbRight "
        "on ("
       +arg4
       +")";

#ifndef QT_NO_DEBUG
 qDebug() << "DB_Tools::leftJoin";
 qDebug() << msg<<"\n-------\n";
 qDebug() << "Arg 1:"<<arg1;
 qDebug() << "Arg 2:"<<arg2;
 qDebug() << "Arg 3:"<<arg3;
 qDebug() << "Arg 4:"<<arg4;
#endif

 return msg;
}

QString DB_Tools::innerJoinFiltered(stJoinArgs ja,QString arg5)
{
 QString msg = "";

 msg = innerJoin(ja)+"where("+arg5+")";

#ifndef QT_NO_DEBUG
 qDebug() << "DB_Tools::innerJoinFiltered\n";
 qDebug() << "msg:\n"<<msg<<"\n-------";
#endif

 return msg;
}

QString DB_Tools::leftJoinFiltered(stJoinArgs ja,QString arg5)
{
 QString msg = "";

 msg = leftJoin(ja)+"where("+arg5+")";

 DB_Tools::DisplayError("DB_Tools::leftJoinFiltered",NULL,msg);

 return msg;
}

DB_Tools::eCort DB_Tools::createOrReadTable(QString tbl_name, QString cnx, QString tbl_code, QString *tbl_read)
{
 DB_Tools::eCort ret_val = eCort_NotSet;

 QSqlDatabase db = QSqlDatabase::database(cnx);
 QSqlQuery query(db);

 QString sql_msg = tbl_code;

 if(isDbGotTbl(tbl_name,cnx)==false){
  sql_msg = "Create table if not exists \"" +
            tbl_name +"\" as \n"+ sql_msg;

  QString filePrefix = "DBT_Cort_" +tbl_name;
  QString dbgFile  = "";
  if(!query.exec(sql_msg)){
#ifndef QT_NO_DEBUG
   dbgFile = filePrefix +"_err.txt";
   BTest::writetoFile(dbgFile,sql_msg,false);
   DisplayError(tbl_name, nullptr, dbgFile);
#endif
   if(tbl_read != nullptr){*tbl_read = sql_msg;}
   ret_val=eCort_ErrCreate;
  }
  else{
#ifndef QT_NO_DEBUG
   dbgFile = filePrefix+"_ok.txt";
   BTest::writetoFile(dbgFile,sql_msg,false);
#endif
   sql_msg = "Select * from '"+tbl_name+"'";
   if(tbl_read != nullptr){*tbl_read = sql_msg;}

#ifndef QT_NO_DEBUG
   dbgFile = filePrefix+"_use.txt";
   BTest::writetoFile(dbgFile,sql_msg,false);
#endif
   ret_val=eCort_Ok;
  }
 }
 else{
  ret_val = eCort_Ok;
  sql_msg = "Select * from \""+tbl_name+"\"";
  if(tbl_read != nullptr){*tbl_read = sql_msg;}
 }

 return ret_val;
}

bool DB_Tools::isDbGotTbl(QString tbl, QString cnx, tbTypes etbTypes, bool silence)
{
 bool b_retVal = false;
 QSqlDatabase db = QSqlDatabase::database(cnx);
 QSqlQuery query(db);
 QString msg_err= "";

 QString kind = "";
 QString m_type = "";

 if((etbTypes==etbTempView) || (etbTypes==etbTempTbl)){
  m_type = "_temp_";
 }
 else {
  m_type = "_";
 }

 switch (etbTypes) {
  case etbTempView:
  case etbView:
   kind = "view";
   break;

  case etbTempTbl:
  case etbTable:
   kind = "table";
   break;

  default:
   ;
 }

 QString msg[]{
  {"SELECT name FROM sqlite"+m_type+"master "
                                    "WHERE type='"+kind+"' AND name='"+tbl+"';"}
 };

 if((b_retVal = query.exec(msg[0])))
 {

  if((b_retVal=query.first()))
  {
   msg_err = QString("Presence Table ")+tbl;
  }
  else{
   msg_err = QString("Absence Table ")+tbl;
  }

  if(!silence){
   QMessageBox::information(NULL,"Test :",msg_err,QMessageBox::Ok);
  }
 }
 return b_retVal;
}

bool DB_Tools::checkHavingTableAndKey(QString tbl, QString key, QString cnx)
{
 bool b_retVal = false;
 QSqlDatabase db = QSqlDatabase::database(cnx);
 QSqlQuery query(db);
 QString msg_err= "";

 QString msg[]{
  {"SELECT name FROM sqlite_master "
   "WHERE type='table' AND name='"+tbl+"';"},
  {"SELECT t1."+key+" from ("+tbl+") as t1; "}
 };

 if((b_retVal = query.exec(msg[0])))
 {
  query.first();
  if(query.isValid())
  {
   /// La table existe faire test sur la clef
   if((b_retVal = query.exec(msg[1]))){
    query.first();
    b_retVal = query.isValid();
   }
   else{
    msg_err = QString("Inconnu \"")+key+QString("\"\ndans table ") +tbl;
    QMessageBox::information(NULL,"Test",msg_err,QMessageBox::Ok);
   }
  }
  else{
   msg_err = QString("Absence Table ")+tbl;
   QMessageBox::critical(NULL,"Test",msg_err,QMessageBox::Ok);
  }
 }
 return b_retVal;
}

QString DB_Tools::getLstDays(QString cnx_db_name, QString tbl_ref)
{
 QString st_tmp = "";

 bool b_retVal = false;

 QSqlDatabase cur_db = QSqlDatabase::database(cnx_db_name);
 QSqlQuery query(cur_db) ;
 QString msg = "";

 QString st_table = "J";

 msg = "select distinct substr(tb1."+st_table+",1,3) as J from ("+
       tbl_ref+") as tb1 order by J asc;";

 b_retVal = query.exec(msg);


 if (b_retVal && (b_retVal = query.first()))
 {
  // 1 ou plus de resultat ?
  int nb_items = 0;
  query.last();
  nb_items= query.at()+1;
  query.first();

  do
  {
   //count(CASE WHEN  J like 'lundi%' then 1 end) as LUN,
   st_tmp = st_tmp + "cast (count(CASE WHEN  J like '"+
            query.value(0).toString()+"%' then 1 end) as int) as "+
            query.value(0).toString()+",\n";
  }while(query.next());

  //supprimer derniere ','
  st_tmp.remove(st_tmp.length()-2,1);
  st_tmp = st_tmp + " ";
 }

#ifndef QT_NO_DEBUG
 qDebug() << "Msg : "<<msg;
 qDebug() << "Date(s) : "<<st_tmp;
#endif

 if(!b_retVal){
  DisplayError("DB_Tools::getLstDays",&query,msg);
  QMessageBox::warning(nullptr,"DB_Tools","getLstDays",QMessageBox::Ok);
 }


 return st_tmp;
}

bool DB_Tools::tbFltGet(stTbFiltres *in_out, QString cnx)
{
 /// https://www.fayewilliams.com/2011/07/13/gdb-conditional-breakpoints/

 bool b_retVal = false;

 // Etablir connexion a la base
 QSqlDatabase db_1 = QSqlDatabase::database(cnx);
 if(db_1.isValid()==false){
  QString str_error = db_1.lastError().text();
  QMessageBox::critical(nullptr, cnx, str_error,QMessageBox::Yes);
  DB_Tools::DisplayError("DB_Tools::flt_DbRead",nullptr,"");
  in_out->sta = Bp::E_Sta::Er_Db;
  return b_retVal;
 }

 QSqlQuery query(db_1);

 ///
 /// rappel : ret-> = (*ret).
 ///

 /// Verifier si info presente dans table
 QString tbFiltre = in_out->tb_flt;

 QString msg = "";
 if((in_out->id>0) && (in_out->dbt==1)){
  msg = "Select *  from "+
        tbFiltre+
        " where ("
        "id="+QString::number(in_out->id)+")";
 }
 else {
  msg = "Select *  from "+
        tbFiltre+
        " where ("
        "zne="+QString::number(in_out->zne)+" and "+
        "typ="+QString::number(in_out->typ)+" and "+
        "lgn="+QString::number(in_out->lgn)+" and "+
        "col="+QString::number(in_out->col)+" and "+
        "val="+QString::number(in_out->val)+")";
 }

#ifndef QT_NO_DEBUG
 qDebug() << "flt_DbRead : "<<msg;
#endif
 b_retVal = query.exec(msg);
 if(b_retVal){
  in_out->sta = Bp::E_Sta::Ok_Query;

  if((b_retVal = query.first())){
   in_out->sta = Bp::E_Sta::Ok_Result;

   /// comptage des reponses
   query.last();
   in_out->dbt = query.at() +1;
   query.first();
   in_out->id = query.value("id").toInt();

   int int_flt = query.value("flt").toInt();
   Bp::F_Flts tmp = static_cast<Bp::F_Flts>(int_flt);
   in_out->b_flt = tmp;
   in_out->pri = query.value("pri").toInt();
  }
  else {
   in_out->dbt = -1;
   in_out->sta = Bp::E_Sta::Er_Result;
  }
 }
 else {
  in_out->sta = Bp::E_Sta::Er_Query;
  DB_Tools::DisplayError("DB_Tools::flt_DbRead",&query,msg);
 }

 return b_retVal;
}

void DB_Tools::genStop(QString fnName)
{
 QString text = "Code exception : "+fnName;
 QMessageBox::question(nullptr,"Exception !!",text);
}

bool DB_Tools::tbFltSet(stTbFiltres *in_out, QString cnx)
{
 bool b_retVal = false;
 if(in_out->val <0){
  in_out->sta = Bp::E_Sta::Er_Query;
  return b_retVal;
 }

 // Etablir connexion a la base
 QSqlDatabase db_1 = QSqlDatabase::database(cnx);
 if(db_1.isValid()==false){
  QString str_error = db_1.lastError().text();
  QMessageBox::critical(nullptr, cnx, str_error,QMessageBox::Yes);
  DB_Tools::DisplayError("DB_Tools::setdbFlt",nullptr,"");
  in_out->sta = Bp::E_Sta::Er_Db;
  return b_retVal;
 }

 QSqlQuery query(db_1);
 QString tbFiltre = in_out->tb_flt;
 QString msg = "";

 Bp::F_Flts flt_def = in_out->b_flt;
 Bp::F_Flts msk = Bp::fltTirLast | Bp::fltTirPrev|
                  Bp::fltWanted|Bp::fltSelected|
                  Bp::fltFiltred|Bp::fltSeenAft|
                  Bp::fltSeenBfr|Bp::fltSeenNot;

 if(flt_def == Bp::noFlt){
  flt_def = msk.operator~();
 }
 flt_def = flt_def & 0xFFFF;
 unsigned int val_flt = flt_def.operator unsigned int();
 unsigned int val_msk = msk.operator unsigned int();

 /// Analyse de la lecture precedente/config utilisateur
 if((in_out->id>0) && (in_out->dbt==1)){
  msg = "update "+in_out->tb_flt+
        " set pri="+QString::number(in_out->pri)+
        ", flt=(flt | 0x"+QString::number(val_msk,16)+
        ") & (0x"+QString::number(val_flt,16)+
        ") where ("
        "id="+QString::number(in_out->id)+")";

 }
 else if(in_out->dbt<=0){
  /// Faire message insert
  msg ="insert into "+
       tbFiltre+
       " (id, zne, typ,lgn,col,val,pri,flt)"
       " values (NULL,"
       +QString::number(in_out->zne)+","
       +QString::number(in_out->typ)+","
       +QString::number(in_out->lgn)+","
       +QString::number(in_out->col)+","
       +QString::number(in_out->val)+","
       +QString::number(in_out->pri)+","
       +QString::number(in_out->b_flt)+")";
  /// BUG ? : in_out pas de mise a jour de id
  in_out->dbt=1;
 }
 else if (in_out->dbt == 1) {
  /// Faire un update
  msg = "update "+in_out->tb_flt+
        " set pri="+QString::number(in_out->pri)+
        ", flt=(flt | 0x"+QString::number(val_msk,16)+
        ") & (0x"+QString::number(val_flt,16)+
        ") where ("
        "zne="+QString::number(in_out->zne)+" and "+
        "typ="+QString::number(in_out->typ)+" and "+
        "lgn="+QString::number(in_out->lgn)+" and "+
        "col="+QString::number(in_out->col)+" and "+
        "val="+QString::number(in_out->val)+")";
 }
 else {
  in_out->dbt++;
  /// faire une analyse ici
  DB_Tools::genStop("DB_Tools::tbFltSet");
  b_retVal = false;
 }

#ifndef QT_NO_DEBUG
 qDebug() << "msg: "<<msg;
#endif
 b_retVal = query.exec(msg);

 if(!b_retVal){
  DB_Tools::DisplayError("DB_Tools::tbFltSet",&query,msg);
  QMessageBox::warning(nullptr,"DB_Tools","tbFltSet",QMessageBox::Ok);
 }

 return b_retVal;
}

void DB_Tools::DisplayError(QString fnName, QSqlQuery *pCurrent,QString sqlCode)
{
 QString db_list = QSqlDatabase::connectionNames().join(", ");
 QString dbError = "";
 QString sqlError = "";
 QString sqlText = "";
 QString sqlGood = "";

 /// Lournal des erreurs
 QFile file("Sql_Errors.log");
 if (!file.open(QIODevice::Append | QIODevice::Text)){
  /// WriteOnly ou Append
  /// https://openclassrooms.com/forum/sujet/qt-ecrire-un-texte-dans-un-fichier-txt-75563
  QMessageBox::information(nullptr, "Pgm", "Fichiers des erreurs!!",QMessageBox::Yes);
 }
 QTextStream err_logs(&file);

 /// Analyse des connection ouvertes
 QStringList lst_cnx = QSqlDatabase::connectionNames();
 int nb_db = lst_cnx.size();

 QString err_id = "Cnx %1/%2 : %3\n";
 for (int i= 0; i< nb_db;i++) {
  QString cnx = lst_cnx.at(i).simplified();
  QSqlDatabase db = QSqlDatabase::database(cnx);

  if(cnx.isEmpty()){
   cnx = "default";
  }

  QString cnx_id = err_id.arg(i+1).arg(nb_db).arg(cnx);
  QString db_err = db.lastError().text();

  db_err = db_err.simplified();
  if(db_err.isEmpty()){
   db_err = "Aucune a priori";
  }

  if(pCurrent !=nullptr)
  {
   sqlGood = pCurrent->executedQuery();
   sqlText = pCurrent->lastQuery();

   sqlError = pCurrent->lastError().text();
   sqlError = sqlError.simplified();
   if(sqlError.isEmpty()){
    sqlError = "Aucune a priori";
   }
  }
  else
  {
   sqlText = sqlCode.simplified();
   if(sqlText.isEmpty()){
    sqlText = "Code usr absent";
   }

   sqlError = "QSqlQuery non renseignee";
  }

  QString msg = cnx_id
                +QString("\ndb error :\n")+db_err + "\n"
                +QString("\nSql considere bon :\n")+sqlGood + "\n"
                +QString("\nSql en cours :\n")+sqlText + "\n"
                +QString("\n\nFn usr : ")+fnName + "\n"
                +QString("\nSql demande :\n")+sqlCode + "\n"
                +QString("\nSql Error :\n")+sqlError + "\n";

  QMessageBox::information(nullptr, "Pgm", msg,QMessageBox::Yes);
#ifndef QT_NO_DEBUG
	err_logs << "\n\n-------------\n"<<msg;
	qDebug() <<msg;
#endif

 }
 //QCoreApplication::exit(-1);
 //QApplication::exit();
}


bool DB_Tools::SupprimerResultatsPrecedent(QString cnx, QString type, QString key)
{
 bool b_retVal = true;

 // Etablir connexion a la base
 QSqlDatabase db_1 = QSqlDatabase::database(cnx);
 if(db_1.isValid()==false){
  QString str_error = db_1.lastError().text();
  QMessageBox::critical(nullptr, cnx, str_error,QMessageBox::Yes);
  DB_Tools::DisplayError("DB_Tools::setdbFlt",nullptr,"");
  return b_retVal;
 }

 QString msg = "";
 QSqlQuery query(db_1);

 msg = "SELECT name FROM sqlite_master "
       "WHERE(type='"+type+"' AND name glob '"+key+"');";
 b_retVal = query.exec(msg);

 if(b_retVal)
 {
  if(query.first())
  {
   QStringList lst_tbl;
   QString target = "";
   do
   {
    target = query.value("name").toString();
    lst_tbl.append(target);
   }while(query.next());

   /// Suppression des tables
   do{
    target = lst_tbl.takeFirst();
    msg = "drop "+type+" if exists " + target;
    b_retVal = query.exec(msg);
   }while(b_retVal && lst_tbl.size());

   /// reorganisation de la base
   if(b_retVal){
    /*
    msg = "vacuum";
    b_retVal = query.exec(msg);
    */
   }
  }
 }

 if(!b_retVal)
 {
  QString ErrLoc = "Supprimer Resultats:";
  DB_Tools::DisplayError(ErrLoc,&query,msg);
 }

 return b_retVal;
}

#if QT_LIB_DBG_ON
/// https://doc.qt.io/archives/qq/qq03-big-brother.html#xref1
void DB_Tools::dumpAllObjectTrees()
{
 dumpRecursive( QObject::objectTrees(), 0 );
}

void DB_Tools::dumpRecursive( const QObjectList *list,
                              QListViewItem *parent )
{
 if ( list == 0 )
  return;
 QListView *listView = 0;
 QListViewItem *child;
 if ( parent == 0 ) {
  listView = new QListView( 0 );
  listView->setRootIsDecorated( TRUE );
  listView->addColumn( "Class" );
  listView->addColumn( "Name" );
  listView->addColumn( "Geometry" );
  listView->setSorting( -1 );
  listView->show();
 }
 QObjectListIt it( *list );
 QObject *obj;
 while ( (obj = it.current()) ) {
  if ( obj == listView ) {
   ++it;
   continue;
  }
  QString flags;
  if ( obj->isWidgetType() ) {
   QWidget *w = (QWidget *) obj;
   if ( w->isVisible() ) {
    flags.sprintf( "<%d,%d,%d,%d>", w->x(),
                   w->y(), w->width(),
                   w->height() );
   } else {
    flags = "invisible";
   }
  }
  child = parent ? new QListViewItem( parent )
                 : new QListViewItem( listView );
  child->setText( 0, obj->className() );
  child->setText( 1, obj->name() );
  child->setText( 2, flags );
  dumpRecursive( it.current()->children(), child );
  ++it;
 }
}
#endif
