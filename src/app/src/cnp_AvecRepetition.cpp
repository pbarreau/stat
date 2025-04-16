#ifndef QT_NO_DEBUG
#include <QDebug>
#endif

#include <math.h>

#include <QMessageBox>
#include <QSqlDatabase>
#include  <QSqlError>

#include <QString>
#include <QStringList>
#include <QSqlDatabase>

#include "cnp_AvecRepetition.h"
#include "db_tools.h"

BGnp::BGnp(stGameConf *pGame, QString tb_dest):BCnp()
{
 QString cnx = pGame->db_ref->cnx;
 bool b_retVal = true;
 addr = nullptr;

 // Etablir connexion a la base
 db_1 = QSqlDatabase::database(cnx);
 if(db_1.isValid()==false){
  QString str_error = db_1.lastError().text();
  QMessageBox::critical(nullptr, cnx, str_error,QMessageBox::Yes);
  return;
 }
 addr=this; /// memo de cet objet

 b_retVal = mktTblGnp(pGame,tb_dest);

 if(!b_retVal){
  addr = nullptr;
 }
}

bool BGnp::mktTblGnp(stGameConf *pGame, QString tb_dest)
{
 bool b_retVal = true;
 int nb_zn = pGame->znCount;
 QString cnx = pGame->db_ref->cnx;

 for(int zn=0; (zn < nb_zn) && b_retVal; zn ++){
  QString tmp_tbl = tb_dest+"_z"+QString::number(zn+1);

	if(DB_Tools::isDbGotTbl(tmp_tbl, cnx) == false)
	{
	 if((b_retVal = creerTables(pGame,zn,tmp_tbl)))
	 {
		if(pGame->limites[zn].len > 2 ){
		 /// Sous forme de A(n,p)
		 b_retVal = mkAnp(pGame,zn,tmp_tbl);
		}
		else {
		 b_retVal = mkCnp(pGame,zn,tmp_tbl);
		}
	 }
	}
 }

 return b_retVal;
}

bool BGnp::creerTables(stGameConf *pGame, int z_id, QString tb_dest)
{
 bool b_retVal = true;
 QString msg = "create table if not exists " + tb_dest;
 QSqlQuery query(db_1);

 QString ref = pGame->names[z_id].abv+"%1 int";
 QString cols = "";

 int max = pGame->limites[z_id].max;
 int nb_10 = max/10;
 int reste = max%10;
 int len = nb_10+((reste >0) ?1:0);
 if( max == 50){ ///// HACK
  len ++;
 }

 for (int i = 0; i< len; i++) {
  cols = cols + ref.arg(i+1);
  if(i<len-1){
   cols = cols + ",";
  }
 }
 msg = msg +"(id integer primary key,"+ cols+",tip text)";

#ifndef QT_NO_DEBUG
 qDebug() <<msg;
#endif

 b_retVal= query.exec(msg);

 return b_retVal;
}

bool BGnp::mkAnp(stGameConf *pGame, int z_id, QString tb_dest)
{
 /// https://www.tutorialspoint.com/sqlite/sqlite_insert_query.htm
 bool b_retVal = true;
 QSqlQuery query(db_1);

 QString key = "tz"+QString::number(z_id+1);
 QString msg = "with choix as (select "
               +key
               +" from B_elm WHERE ("
               +key
               +" not null))";
 msg = msg + "insert into "+tb_dest+" select NULL,";

 QString ref_0 = pGame->names[z_id].abv+"%1";
 QString ref_1 = "t%1";
 QString ref_2 = "t%1."+key;
 QString ref_3 = "cast("+ref_2+" as int) as "+ref_0;

 QString sql_1="";
 QString sql_2="";
 QString sql_3="";
 QString sql_4="";
 QString sql_5="";

 int max = pGame->limites[z_id].max;
 int win = pGame->limites[z_id].win;
 int len = (int)floor(max/10)+1;

 for (int i = 0; i< len; i++) {
  sql_1 = sql_1 + ref_3.arg(i+1);
  sql_2 = sql_2 + ref_2.arg(i+1);
  sql_3 = sql_3 + "choix as "+ref_1.arg(i+1);
  sql_4 = sql_4 + ref_0.arg(i+1);
  sql_5 = sql_5 + ref_0.arg(i+1);

	if(i<len-1){
	 sql_1 = sql_1 + ",";
	 sql_2 = sql_2 + "||'/'||";
	 sql_3 = sql_3 + ",";
	 sql_4 = sql_4 + "+";
	 sql_5 = sql_5 + ",";
	}
 }

 msg = msg
       + sql_1
       + ", cast(("
       + sql_2
       + ") as text) as tip from("
       + sql_3
       + ") where(("
       + sql_4
       + "="
       + QString::number(win)
       + ")) order by "
       + sql_5;

#ifndef QT_NO_DEBUG
 qDebug() <<msg;
#endif

 b_retVal= query.exec(msg);

 return b_retVal;
}

bool BGnp::mkCnp(stGameConf *pGame, int z_id, QString tb_dest)
{
 /// https://www.tutorialspoint.com/sqlite/sqlite_insert_query.htm
 bool b_retVal = true;
 QSqlQuery query(db_1);

 QString key = "z"+QString::number(z_id+1);
 QString msg = "with choix as (select id,"
               +key
               +" from B_elm WHERE ("
               +key
               +" not null))";
 msg = msg + "insert into "+tb_dest+" select NULL,";

 QString ref_0 = pGame->names[z_id].abv+"%1";
 QString ref_1 = "t%1";
 QString ref_2 = "printf('%02d',t%1."+key+")";
 QString ref_3 = "cast("+ref_2+" as int) as "+ref_0;
 QString ref_4 = "("+ref_1+".id < t%2.id)";

 QString sql_1="";
 QString sql_2="";
 QString sql_3="";
 QString sql_4="";
 QString sql_5="";

 int len = pGame->limites[z_id].len;
 for (int i = 0; i< len; i++) {
  sql_1 = sql_1 + ref_3.arg(i+1);
  sql_2 = sql_2 + ref_2.arg(i+1);
  sql_3 = sql_3 + "choix as "+ref_1.arg(i+1);
  sql_5 = sql_5 + ref_0.arg(i+1);

	if(i<len-1){
	 sql_1 = sql_1 + ",";
	 sql_2 = sql_2 + "||'/'||";
	 sql_3 = sql_3 + ",";
	 sql_4 = sql_4 + ref_4.arg(i+1).arg(i+2);
	 sql_5 = sql_5 + ",";
	}

	if(i<len-2){
	 sql_4 = sql_4 + " AND ";
	}
 }

 if(sql_4.size()){
  sql_4 = " where("+sql_4+")";
 }

 msg = msg
       + sql_1
       + ", cast(("
       + sql_2
       + ") as text) as tip from("
       + sql_3
       + ") "
       + sql_4
       + " order by "
       + sql_5;

#ifndef QT_NO_DEBUG
 qDebug() <<msg;
#endif

 b_retVal= query.exec(msg);

 return b_retVal;
}

BGnp *BGnp::self()
{
 return addr;
}

BGnp::BGnp(int n, int p):BCnp((n+p-1),p)
{
}
BGnp::BGnp(int n, int p, QString cnx_bdd):BCnp((n+p-1),p, cnx_bdd,"")
{
}
BGnp::BGnp(int n, int p, QString cnx_bdd, QString tab="My"):BCnp((n+p-1),p, cnx_bdd,tab)
{
}
BGnp::~BGnp()
{
}

/// https://denishulo.developpez.com/tutoriels/access/combinatoire/#LIV-A

#if 0
QString ConstruireRequete(QString item, int n, QString name, int step);
QString LireTableau(int *t, int n);

BP_Gnp::BP_Gnp(int n, int p)
{
    gnp = Cardinal_np();
    pos = 0;
    tab = NULL;

    coefItem = GenereCombinaison(n,p);
}

BP_Gnp::~BP_Gnp()
{
    if(tab !=NULL){
        for(int i = 0; i< gnp;i++){
            delete(tab[i]);
        }
        delete(tab);
    }
}

bool BP_Gnp::CalculerGamma(void)
{
    bool isOK = false;

    if(tab==NULL){
        isOK = FaireTableauGamma();
    }

    return isOK;
}

bool BP_Gnp::FaireTableauGamma(void)
{
    bool b_retVal = false;
    tab = new int *[gnp]; /// tableau de pointeur d'entiers de Cnp lignes

    /// Allocation memoire OK ?
    if(tab != NULL){
        /// initialisation recursion
        int *L = new int [p];
        int *t = new int [n];

        if(t != NULL){

            for(int i =0; i<n;i++) t[i]=i;
        }

        /// demarrage
        if((L != NULL) && (t !=NULL)){
            CreerLigneTrianglePascal(0,L,t,n);
            b_retVal = true;
        }
    }

    return b_retVal;
}

double BP_Gnp::CalculerFactorielle(double *x)
{
    double i;
    double result=1;
    if(*x >= 1)
    {
        for(i=*x;i>1;i--)
        {
            result = result*i;
        }
        return result;
    }
    else
        if(*x == 0)
            return 1;
    return 0;
}

double BP_Gnp::CalculerGnp(void)
{
    return(gnp);
}

double BP_Gnp::Cardinal_np()
{
    double v1 = n+p-1;
    double v2 = n-1;
    double v3 = p;

    double tmp = 0;

    tmp = CalculerFactorielle(&v1)/(CalculerFactorielle(&v3)*CalculerFactorielle(&v2));

    return tmp;

}

QStringList BP_Gnp::coef(void)
{
    return (coefItem);
}

QStringList BP_Gnp::GenereCombinaison(int n, int k)
{
    int tab[1024];
    //int ptrTab = 0;
    int ptrCur = 0;
    int tmp =0;

    tab[0]=k;
    memset(tab+1,0,sizeof(tab)-1);

    QString ligne = "";
    QStringList lst;
    while(tab[n-1] != k)
    {
        ligne = LireTableau(tab, n);
        lst << ligne;
        tmp = tab[n-1];
        tab[n-1] =0;
        ptrCur=n-1;
        while(tab[ptrCur]==0) ptrCur--;
        tab[ptrCur]--;
        tab[ptrCur+1]=tmp+1;

    }
    ligne = LireTableau(tab, n);
    lst << ligne;

    return lst;
}

QString BP_Gnp::MakeSqlFromGamma(stTiragesDef *pTirDef, int step, int k)
{
    int zone = 0;
    int n = pTirDef->nbElmZone[zone];
    QString name = pTirDef->nomZone[zone];
    QString tmp1 = "";
    QString tmp2 = "";
    BP_Gnp a(n,k);
    QStringList lesCoefs = a.coef();

    for(int i = 0; i< lesCoefs.size(); i++)
    {
        tmp1 = ConstruireRequete(lesCoefs.at(i),n,name, step);
        tmp2 = tmp2 + "("+ tmp1 +")" + "or";
    }
    tmp2.remove(tmp2.length()-2,2);

#ifndef QT_NO_DEBUG
qDebug()<< tmp2;
#endif

    return tmp2;
}

QString ConstruireRequete(QString item, int n, QString name, int step)
{
    QStringList lst = item.split(";");
    QString tmp = "";

        for(int i=1;i<lst.size();i++)
        {
         tmp = tmp + "("+name+lst.at(i)+
                 "="+name+lst.at(0)+"+"+
                 QString::number(i*step)+")";
         tmp = tmp + "and";
        }
        tmp.remove(tmp.length()-3,3);

        return tmp;
}

QString LireTableau(int *t, int n)
{
    QString tmp = "";

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < t[i]; ++j) {
            tmp = tmp + QString::number(i+1)+";";
        }
    }
    tmp.remove(tmp.length()-1,1);
    return tmp;
}
#endif
