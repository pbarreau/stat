#ifndef QT_NO_DEBUG
#include <QDebug>
#endif

#include <QSqlDatabase>
#include <QSqlRecord>

#include <QSqlQuery>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include <QMenu>
#include <QAction>

#include "gererbase.h"
#include "tirages.h"

QString req_msg(int zone, int boule, stTiragesDef *ref);

void GererBase::DistributionSortieDeBoule(int boule, QStandardItemModel *modele,stTiragesDef *pRef)
{
  bool status = false;

  QSqlQuery query;
  QString msg;


  QSqlQuery selection;
  QString msg1;
  QStringList tmp;
  double EcartMoyen = 0.0;
  int nbTirages=0;
  int calcul;
  int lgndeb=0, lgnfin=0;
  int nbTotCouv = 0, EcartMax=0, EcartCourant = 0, EcartPrecedent=0;
  int a_loop = 0;


  // recuperation du nombre de tirage total
  msg= "select count (*) from tirages";
  status = query.exec(msg);
  query.first();
  nbTirages = query.value(0).toInt();
  query.finish();


  // creation d'une table temporaire
  msg = "DROP table IF EXISTS tmp_couv";
  status = query.exec(msg);
#ifndef QT_NO_DEBUG
  if(!status){
	qDebug() << "ERROR:" << query.executedQuery()  << "-" << query.lastError().text();
  }
#endif

  msg =  "create table tmp_couv (id INTEGER PRIMARY KEY, depart int, fin int, taille int)";
  status = query.exec(msg);
#ifndef QT_NO_DEBUG
  if(!status){
	qDebug() << "ERROR:" << query.executedQuery()  << "-" << query.lastError().text();
  }
#endif



  // requete a effectuer
  msg = "insert into tmp_couv (id, depart, fin, taille) values (:id, :depart, :fin, :taille)";
  query.prepare(msg);

  // Recuperation des lignes ayant la boule
  msg = req_msg(0,boule,pRef);
  status = selection.exec(msg);


  //Partir de la fin des tirages trouves
  selection.last(); // derniere ligne ayant le numero
  if(selection.isValid()){
	lgndeb = nbTirages;
	nbTirages = 0; //calcul des intervales
	a_loop = 1;
	do
	{
	  QSqlRecord rec  = selection.record();
	  calcul = rec.value(0).toInt();
	  lgnfin = selection.value(0).toInt();

	  query.bindValue(":depart", lgndeb);
	  query.bindValue(":fin", lgnfin);
	  query.bindValue(":taille", lgndeb-lgnfin+1);
	  // Mettre dans la base
	  status = query.exec();

	  nbTirages += (lgndeb-lgnfin);
	  lgndeb = lgnfin-1;
	  a_loop++;
	}while(selection.previous());
	//selection.finish();
	//query.finish();
  }

  // Rajouter une ligne pour ecart le plus recent
  lgnfin = 1;
  query.bindValue(":depart", lgndeb);
  query.bindValue(":fin", lgnfin);
  query.bindValue(":taille", lgndeb-lgnfin+1);
  // Mettre dans la base
  status = query.exec();
  nbTirages += (lgndeb-lgnfin);

  // calcul des ecarts pour la boule
  msg = "select count (*) from tmp_couv";
  status = query.exec(msg);
  query.first();
  nbTotCouv = query.value(0).toInt();

  // Moyenne
  if(a_loop>0)
	EcartMoyen = double(nbTirages)/a_loop;

  // recherche l'ecart le plus grand
  msg = "select max(taille) from tmp_couv";
  status = query.exec(msg);
  query.first();
  EcartMax = query.value(0).toInt();

  //recherche de l'ecart courant et suivant
  msg = "select taille from tmp_couv";
  status = query.exec(msg);
  query.last();
  EcartCourant = query.value(0).toInt();
  query.previous();
  EcartPrecedent = query.value(0).toInt();

  QStandardItem *item1 = new QStandardItem;
  item1->setData(EcartCourant,Qt::DisplayRole);
  modele->setItem(boule-1,1,item1);

  QStandardItem *item2 = new QStandardItem( QString::number(222));
  item2->setData(EcartPrecedent,Qt::DisplayRole);
  modele->setItem(boule-1,2,item2);

  QStandardItem *item3 = new QStandardItem( );
  QString valEM = QString::number(EcartMoyen,'g',2);
  //item3->setData(EcartMoyen,Qt::DisplayRole);
  item3->setData(valEM.toDouble(),Qt::DisplayRole);
  modele->setItem(boule-1,3,item3);


  QStandardItem *item4 = new QStandardItem( QString::number(222));
  item4->setData(EcartMax,Qt::DisplayRole);
  modele->setItem(boule-1,4,item4);
}

void GererBase::AfficherMaxOccurenceBoule(int boule,QLabel *l_nb)
{
  QSqlQuery query(db);
  QString msg;
  int value = 0;
  bool status = false;

  // Recherche du maximum pour cette boule
  msg = "create view r_boul as select * from tirages where (b1=" +
		QString::number(boule) + " or b2=" + QString::number(boule)
		+ " or b3=" + QString::number(boule) + " or b4=" + QString::number(boule) + " or b5=" + QString::number(boule) + ");";
  status = query.exec(msg);

  if(status){
	msg = "select count (*) from r_boul";
	status = query.exec(msg);

	if(status){
	  query.first();

	  if(query.isValid()){

		QSqlRecord rec  = query.record();
		value = rec.value(0).toInt();

		l_nb->setText(QString("Boule %1 : %2 fois ").arg( boule ).arg(value) );
	  }
	}
  }
}

void GererBase::MontrerResultatRechercheVoisins(QStandardItemModel *modele,int b_id)
{
  QSqlQuery query;
  QString msg;
  bool status = true;

  msg = "select * from r"+QString::number(b_id) + " ;";
  status = query.exec(msg);

  if(status)
  {
	status = query.first();
	if(query.isValid())
	{
	  int position = 0;
	  modele->sort(0);//
	  do{
		QSqlRecord ligne = query.record();
		int r0 = ligne.value(1).toInt();
		int rp1 = ligne.value(2).toInt();
		int rp2 = ligne.value(3).toInt();
		int rn1 = ligne.value(4).toInt();
		int rn2= ligne.value(5).toInt();

		QStandardItem *item_0 = modele->item(position,0);
		item_0->setData(position+1,Qt::DisplayRole);
		modele->setItem(position,0,item_0);

		//QStandardItem *item_1 = new QStandardItem( QString::number(222));
		QStandardItem *item_1 = modele->item(position,1);
		item_1->setData(r0,Qt::DisplayRole);
		modele->setItem(position,1,item_1);

		//QStandardItem *item_2 = new QStandardItem( QString::number(222));
		QStandardItem *item_2 = modele->item(position,2);
		item_2->setData(rp1,Qt::DisplayRole);
		modele->setItem(position,2,item_2);

		//QStandardItem *item_3 = new QStandardItem( QString::number(222));
		QStandardItem *item_3 = modele->item(position,3);
		item_3->setData(rp2,Qt::DisplayRole);
		modele->setItem(position,3,item_3);

		//QStandardItem *item_4 = new QStandardItem( QString::number(222));
		QStandardItem *item_4 = modele->item(position,4);
		item_4->setData(rn1,Qt::DisplayRole);
		modele->setItem(position,4,item_4);

		//QStandardItem *item_5 = new QStandardItem( QString::number(222));
		QStandardItem *item_5 = modele->item(position,5);
		item_5->setData(rn2,Qt::DisplayRole);
		modele->setItem(position,5,item_5);

		position++;
	  }while(query.next());
	}

  }

}

void GererBase::RechercherVoisinDeLaBoule(int b_id, int max_voisins)
{
  QSqlQuery query;
  QString msg;
  bool status = true;
  int r0 =0, rn1=0,rn2=0,rp1=0,rp2=0;

  // Recherche des voisins de la boule
  for(int voisin=1;(voisin<=max_voisins) && status;voisin++)
  {
	if(voisin != b_id)
	{
	  // Boule sortant avec
	  r0 = TotalRechercheVoisinADistanceDe(0,voisin);
	}
	else
	{
	  r0 = 0;
	}

	rn1 = TotalRechercheVoisinADistanceDe(1,voisin);
	rp1 = TotalRechercheVoisinADistanceDe(-1,voisin);
	rn2 = TotalRechercheVoisinADistanceDe(2,voisin);
	rp2 = TotalRechercheVoisinADistanceDe(-2,voisin);

	// mise a jour dans la base
	msg = "update r"+QString::number(b_id) + " " +
		  "set r0=" +QString::number(r0)+ ", " +
		  "rp1=" +QString::number(rp1)+ ", " +
		  "rp2=" +QString::number(rp2)+ ", " +
		  "rn1=" +QString::number(rn1)+ ", " +
		  "rn2=" +QString::number(rn2)+ " " +
		  "where (id="+QString::number(voisin)+");";
	status = query.exec(msg);
  }
}

bool GererBase::CreerTableVoisinsDeBoule(int b_id, int max_voisins)
{
  QSqlQuery query;
  QString msg;
  bool status = false;

  // Creation d'une table si pas encore cree pour memoriser la recherche
  msg =  "create table if not exists r"+QString::number(b_id)+ " " +
		 "(id INTEGER PRIMARY KEY, r0 int, rp1 int, rp2 int, rn1 int, rn2 int);";
  status = query.exec(msg);

  if(status)
  {
	// On essai de recuperer une valeur pour voir si les infos dans table
	msg = "select count (*) from r" +QString::number(b_id)+ "; ";
	status = query.exec(msg);

	if(status)
	{
	  status = query.first();
	  if(query.isValid())
	  {
		int calcul = query.value(0).toInt();
		if(!calcul){
		  // Table pas encore cree mettre a 0 les donnees
		  for(int voisin=1;(voisin<=max_voisins) && status;voisin++)
		  {
			msg = "insert into r"+QString::number(b_id)+
				  " (id, r0, rp1, rp2, rn1, rn2) values (null, 0,0,0,0,0);";
			status = query.exec(msg);
		  }
		}
		else
		{
		  // La table existe deja et a des infos des voisins
		  status = false;
		}
	  }
	}
  }

  return status;
}

void GererBase::RechercheVoisin(int boule, stTiragesDef *pConf,
								QLabel *l_nb, QStandardItemModel *modele)
{
  QSqlQuery query(db);
  QString msg;
  bool status = false;
  //int calcul = 0, voisin = 0;
  //int rp1 = 0, rp2 = 0, rn1 = 0, rn2 = 0;
  //int resu = 0;
  int zn = 0;
  QString mvoisins[5]={"r0","rp1","rp2","rn1","rn2"};

  if (iAffichageVoisinEnCoursDeLaBoule != boule)
  {
	iAffichageVoisinEnCoursDeLaBoule = boule;
	AfficherMaxOccurenceBoule(boule, l_nb);
	int max_voisin = pConf->limites[zn].max;
	status = CreerTableVoisinsDeBoule(boule, max_voisin);

	if(status)
	{
	  RechercherVoisinDeLaBoule(boule, max_voisin);
	}

	  // Affichage des resultats dans la vue
	MontrerResultatRechercheVoisins(modele, boule);
  }

#if 0
  // Recherche des voisins de la boule
  for(voisin=1;(voisin<=(pConf->limites[zn].max)) && status;voisin++)
  {
	if(voisin != boule){
	  // Boule sortant avec
#if 0
	  msg = "select count (*) from r_boul where (b1=" + QString::number(voisin) + " or b2=" + QString::number(voisin)
			+ " or b3=" + QString::number(voisin) + " or b4=" + QString::number(voisin) + " or b5=" + QString::number(voisin) + ")";
	  status = query.exec(msg);
	  query.first();
	  //QSqlRecord rec  = query.record();
	  resu = query.value(0).toInt();
#endif
	  resu = TotalRechercheVoisinADistanceDe(0,voisin);
	}
	else
	{
	  resu = 0;
	}
	QStandardItem *item = new QStandardItem( QString::number(222));
	item->setData(resu,Qt::DisplayRole);
	modele->setItem(voisin-1,1,item);

	rn1 = TotalRechercheVoisinADistanceDe(1,voisin);
	rp1 = TotalRechercheVoisinADistanceDe(-1,voisin);
	rn2 = TotalRechercheVoisinADistanceDe(2,voisin);
	rp2 = TotalRechercheVoisinADistanceDe(-2,voisin);

	// mise a jour dans la base
	if (calcul == 0){
	  msg = "update r"+QString::number(boule) + " " +
			"set r0=" +QString::number(resu)+ ", " +
			"rp1=" +QString::number(rp1)+ ", " +
			"rp2=" +QString::number(rp2)+ ", " +
			"rn1=" +QString::number(rn1)+ ", " +
			"rn2=" +QString::number(rn2)+ " " +
			"where (id="+QString::number(voisin)+");";
	  status = query.exec(msg);
	}

	//calcul = rp1 + rp2 + rn1 + rn2;
	QStandardItem *item2 = new QStandardItem( QString::number(222));
	item2->setData(rp1,Qt::DisplayRole);
	modele->setItem(voisin-1,2,item2);

	//calcul =calcul + resu;
	QStandardItem *item3 = new QStandardItem( QString::number(222));
	item3->setData(rp2,Qt::DisplayRole);
	modele->setItem(voisin-1,3,item3);

	QStandardItem *item4 = new QStandardItem( QString::number(222));
	item4->setData(rn1,Qt::DisplayRole);
	modele->setItem(voisin-1,4,item4);

	QStandardItem *item5 = new QStandardItem( QString::number(222));
	item5->setData(rn2,Qt::DisplayRole);
	modele->setItem(voisin-1,5,item5);


  }
#endif

  // creer les tables avec les meilleurs de facon ordonne
  for(int i=0;(i<(pConf->nbElmZone[zn])) && status;i++)
  {
	// voir si la table existe
	msg = "select * from tb_"+QString::number(boule)+mvoisins[i]+";";
	status = query.exec(msg);

	if(!query.isValid())
	{
	  // La table n'existe pas encore
	  msg = "create table tb_" +QString::number(boule)+mvoisins[i]+
			" as select id,"+mvoisins[i]+" from " +
			"r"+QString::number(boule)+ " order by " + mvoisins[i]+" desc limit 10;";
	  status = query.exec(msg);
	}
  }

  // Selection les meilleurs resultats de chacun
  msg = "select * from union_" + QString::number(boule)+";";
  status = query.exec(msg);

  if(!query.isValid())
  {
	// La table n'existe pas encore
	msg = "create table union_" + QString::number(boule)+
		  " as select r"+ QString::number(boule)+".id,"+
		  "(r0+rp1+rp2+rn1+rn2) 'T' from r"+ QString::number(boule)+
		  " where id in ("+
		  "select id from tb_"+QString::number(boule)+mvoisins[0]+ " union " +
		  "select id from tb_"+QString::number(boule)+mvoisins[1]+ " union " +
		  "select id from tb_"+QString::number(boule)+mvoisins[2]+ " union " +
		  "select id from tb_"+QString::number(boule)+mvoisins[3]+ " union " +
		  "select id from tb_"+QString::number(boule)+mvoisins[4]+ " " +
		  ") order by T desc;";
	status = query.exec(msg);
  }
  // Recherche terminee finir avec cette vue
  msg = "drop view r_boul";
  query.exec(msg);

}

int GererBase::TotalRechercheVoisinADistanceDe(int dist, int voisin)
{
  QString msg = "";
  QSqlQuery query(db);
  int calcul = 0;
  bool status;

  // Selection des lignes voisines de celle de reference
  msg = "create view rn1 as select * from tirages inner join r_boul on tirages.id = r_boul.id + %1";
  msg = (msg).arg(dist);
  status = query.exec(msg);

  // Comptage des lignes ayant la boule comme voisine
  msg = "select count (*) from rn1 where (b1=" + QString::number(voisin) + " or b2=" + QString::number(voisin)
		+ " or b3=" + QString::number(voisin) + " or b4=" + QString::number(voisin) + " or b5=" + QString::number(voisin) + ")";
  status = query.exec(msg);
  query.first();
  calcul = query.value(0).toInt();

  // On detruit la vue des resultats de cette boule
  msg = "drop view rn1";
  status = query.exec(msg);

  return calcul;
}

void GererBase::CouvertureBase(QStandardItemModel *dest,stTiragesDef *pRef)
{
  QSqlQuery query;
  QSqlQuery sauve;
  QSqlQuery position;
  QString msg = "";
  bool depart_couverture = false;
  int lgndeb=0, lgnfin=0;
  int memo_boule[50]= {0};
  int ordr_boule[50]= {0};
  int nb_boules = 0;
  int i = 0, j= 0;
  int id_couv=0;
  bool status;
  int zn = 0;

  stTiragesDef ref = *pRef;

  // recuperer tous les tirages
  msg= "select * from tirages";
  status = query.exec(msg);

  // Se positionner au debut des tirages du jeu
  status = query.last();
  depart_couverture = true;
  lgndeb = query.value(0).toInt();
  if(query.isValid())
  {
	// requete a effectuer
	msg = "insert into " + QString::fromLocal8Bit(CL_TCOUV) + ref.nomZone[zn]  +
		  " (id, depart, fin, taille) values (:id, :depart, :fin, :taille)";
	status = sauve.prepare(msg);

	do
	{
	  QSqlRecord rec  = query.record();

	  // Sauvegarder le debut de couverture
	  if(depart_couverture){
		id_couv++;
		sauve.bindValue(":depart", lgndeb);

		// creer colonne pour ordre d'arrivee
		CreerColonneOrdreArrivee(id_couv, &ref);
		depart_couverture = false;
	  }

	  // prendre toutes les boules de la zone pour le tirage concerne
	  for(i = 0; (i<ref.nbElmZone[zn])&& (nb_boules<ref.limites->max);i++)
	  {
		int boule = rec.value(2+i).toInt();

		// Cette boule est elle connue
		if(!memo_boule[boule-1])
		{
		  // non alors memoriser l'ordre d'arrivee
		  ordr_boule[nb_boules]= boule;
		  msg = "update " + QString::fromLocal8Bit(CL_TOARR) + ref.nomZone[zn] +
				" set " + QString::fromLocal8Bit(CL_CCOUV) +
				QString::number(id_couv) + "=" +QString::number(boule)+
				" where (id="+QString::number(nb_boules+1)+");";
		  status = position.exec(msg);
		  nb_boules++;
		}

		// Un boule de plus comporte le numero "boule"
		memo_boule[boule-1]++;
	  }

	  // A ton atteind la converture ?
	  if(nb_boules == ref.limites->max)
	  {
		// Oui
		// il faudra une nouvelle colonne pour l'ordre d'arrivee des boules
		depart_couverture = true;

		// remettre a zero compteur apparition
		nb_boules = 0;
		memset(memo_boule, 0, (ref.limites->max)*sizeof(int));
		memset(ordr_boule, 0, (ref.limites->max)*sizeof(int));

		lgnfin = rec.value(0).toInt();
		sauve.bindValue(":fin", lgnfin);
		sauve.bindValue(":taille", lgndeb-lgnfin+1);
		// Mettre dans la base
		status = sauve.exec();
#ifndef QT_NO_DEBUG
		if(!status){
		  qDebug() << "ERROR:" << query.executedQuery()  << "-" << query.lastError().text();
		}
#endif
		// Est ce la derniere boule du tirage qui a permis la couverture
		if( i != ref.nbElmZone[zn]-1)
		{
		  // Creer une nouvelle colonne couverture
		  // creer colonne pour ordre d'arrivee
		  CreerColonneOrdreArrivee(id_couv+1, &ref);

		  // Non, alors indiquer les voisines comme nouvelles
		  lgndeb = lgnfin;
		  for(j=0;j<i;j++){
			int boule = rec.value(2+j).toInt();

			// On ne prends pas celle qui a permis la fin de couverture
			if(i !=j)
			{
			  msg = "update " + QString::fromLocal8Bit(CL_TOARR) + ref.nomZone[zn] +
					" set " + QString::fromLocal8Bit(CL_CCOUV) +
					QString::number(id_couv+1) + "=" +QString::number(boule)+
					" where (id="+QString::number(nb_boules+1)+");";
			  status = position.exec(msg);
			  ordr_boule[nb_boules]= boule;
			  nb_boules++;
			  memo_boule[boule-1]++;
			}
		  }
		}
		else
		{
		  lgndeb = lgnfin-1;
		}
		//sauve.bindValue(":depart", lgndeb);
	  }

	}while(query.previous());

	// On a parcouru toute la base
	// indiquer les absents
	for(i = 0; i< ref.limites->max ;i++)
	{
	  if (!memo_boule[i])
	  {
		QBrush macouleur(Qt::green);
		QStandardItem *item1 = dest->item(i);
		item1->setBackground(macouleur);
	  }
	}
  }
}

QString req_msg(int zone, int boule, stTiragesDef *ref)
{
  int max_elm_zone = ref->nbElmZone[zone];
  int col_id = 0;
  //QString *tab = ref->nomZone;
  QString msg = "select id from tirages where (";


  // Suite msg requete
  for(col_id=1;col_id<=max_elm_zone;col_id++)
  {
	msg = msg +
		  ref->nomZone[zone] +
		  QString::number(col_id)+ "=" + QString::number(boule)+
		  " or ";
  }

  if(msg.length() != 0){
	msg.remove(msg.size()-4,4);
	msg = msg + ")";
  }

  return msg;
}

bool GererBase::CreerColonneOrdreArrivee(int id, stTiragesDef *pConf)
{
  QSqlQuery query;
  QString msg = "";
  stTiragesDef ref = *pConf;
  bool status = false;
  int zn = 0;

  // test pour voir si une colonne existe deja
  msg = "select * from " + QString::fromLocal8Bit(CL_TOARR) + ref.nomZone[zn] +
		"." + QString::fromLocal8Bit(CL_CCOUV) +
		QString::number(id);
  status = query.exec(msg);

  if (status == false)
  {
	msg = "alter table " + QString::fromLocal8Bit(CL_TOARR) + ref.nomZone[zn] +
		  " add column " + QString::fromLocal8Bit(CL_CCOUV) +
		  QString::number(id) + " int;";
	status = query.exec(msg);
  }
  else
  {
	status = true;
  }

  return status;
}

void GererBase::PopulateCellMenu(int b_id, int v_id,QMenu *menu,QObject * receiver)
{
  QString msg;
  QSqlQuery query;
  bool status = false;
  int somme = 0;

  // On recupere les valeurs des voisins
  QString Lib[6]={"tot:","r0:","+1:","+2:","-1:","-2:"};
  QString vVoisin[6];

  msg = "select * from r" + QString::number(b_id) +
		" where (id = "+QString::number(v_id) +");";
  status = query.exec(msg);

  if(status)
  {
	status = query.first();
	if(query.isValid())
	{
	  QSqlRecord ligne = query.record();
	  int nb_col = ligne.count();
	  for(int i =1; i< nb_col;i++)
	  {
		int val = ligne.value(i).toInt();
		somme+=val;
		vVoisin[i]= Lib[i]+QString::number(val);
	  }
	  vVoisin[0]= Lib[0]+QString::number(somme);

	  for(int i = 0; i<6;i++)
	  {
		menu->addAction(new QAction(vVoisin[i], receiver));
	  }

	}
  }
}

void GererBase::EffectuerTrieMesPossibles(int tri_id, int col_id,int b_id,QStandardItemModel * vue)
{
  QString msg;
  QSqlQuery query;
  bool status = false;
  QString tblColName[5]={"r0","rp1","rp2","rn1","rn2"};

  //select r30.id,r0 from r30 inner join union_30 on union_30.id=r30.id order by r0 desc;

  msg = "select r"+QString::number(b_id)+".id,"+tblColName[tri_id]+
		" from r"+QString::number(b_id)+
		" inner join union_"+QString::number(b_id)+
		" on union_"+QString::number(b_id)+".id=r"+QString::number(b_id)+
		".id order by "+ tblColName[tri_id]+" desc;";
  status = query.exec(msg);

  if(status)
  {
	status = query.first();
	if(query.isValid())
	{

	  if (col_id >=0 || col_id <5)
	  {

		// Parcourir tout les resultats
		int position = 0;
		do{
		  QSqlRecord ligne = query.record();
		  int val = ligne.value(0).toInt();
		  QStandardItem *tmp_itm = vue->item(position,col_id);
		  tmp_itm->setData(val,Qt::DisplayRole);
		  vue->setItem(position,col_id,tmp_itm);
		  position++;
		}while(query.next());
	  }
	}
  }
}
