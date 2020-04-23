#include <QToolTip>

#include <QSqlQuery>

#include "bdelegatecouleurfond.h"
#include "db_tools.h"
#include "colors.h"

const int PaintingScaleFactor = 10;

void BDelegateCouleurFond::paint(QPainter *painter, const QStyleOptionViewItem &option,
                                 const QModelIndex &index) const
{
  QStyleOptionViewItem maModif(option);

  int col = index.column();

  QRect Cellrect = maModif.rect;
  int refx = Cellrect.topLeft().x();
  int refy = Cellrect.topLeft().y();
  int ctw = Cellrect.width();
  int cth = Cellrect.height();
  int cx = ctw/4;
  int cy = cth/2;
  QPoint c1(refx +(ctw/5)*4,refy + (cth/6));
  QPoint c2(refx +(ctw/5)*4,refy + (cth*5/6));

  QRect r1; /// priorite
  QRect r2; /// Last
  QRect r3; /// previous
  QRect r4; /// Selected

  QPoint p1(refx,refy);
  QPoint p2(refx +(ctw/3),refy+cth);
  QPoint p3(refx+ctw,refy+(cth*2/3));
  QPoint p4(refx +(ctw/3),refy+(cth/3));
  QPoint p5(refx + ctw,refy);

  /// Priorite
  r1.setTopLeft(p1);
  r1.setBottomRight(p2);

  /// Last
  r2.setBottomLeft(p2);
  r2.setTopRight(p3);

  /// Previous
  r3.setBottomRight(p3);
  r3.setTopLeft(p4);

  ///Selected
  r4.setBottomLeft(p4);
  r4.setTopRight(p5);

  QPolygon triangle;
  QPoint t1(refx,refy);
  QPoint t2(refx+ctw,refy);
  QPoint t3(refx,refy+cy);
  triangle << t1<<t2<<t3<<t1;


  if(col == Columns::vFilters){
    int val_f = (index.sibling(
                   index.row(),
                   index.model()->columnCount()-1)
                 ).data().toInt();

    painter->save();
    painter->setRenderHint(QPainter::Antialiasing, true);

    if( (val_f & Filtre::isLast)){
      painter->fillRect(r2, COULEUR_FOND_DERNIER);
    }

    if(val_f & Filtre::isPrevious){
      painter->fillRect(r3, COULEUR_FOND_AVANTDER);
    }

    if(val_f & Filtre::isWanted){
      painter->fillRect(r4, COULEUR_FOND_FILTRE);
    }

    if(val_f & Filtre::isNever){
      painter->fillRect(r2, COULEUR_FOND_JAMSORTI);
    }

    /// Mettre les cercles maintenant car les fonds
    /// snt deja dessinee
    if(val_f & Filtre::isPlusOne||
       (val_f & Filtre::isMinusOne)){

      if((val_f & Filtre::isPlusOne)){
        painter->setBrush(Qt::green);
        painter->drawEllipse(c1,cx/2,cy/4);
      }

      if((val_f & Filtre::isMinusOne)){
        painter->setBrush(Qt::red);
        painter->drawEllipse(c2,cx/2,cy/4);
      }
    }

    painter->restore();
  }

  if(col == Columns::vEcart ){
    int val_col_2 = (index.sibling(index.row(),Columns::keyColors)).data().toInt();
    QColor leFond = map_FromColor.key(val_col_2);
#ifndef QT_NO_DEBUG
    QString msg = "Lecture couleur : "+ QString::number(val_col_2).rightJustified(2,'0')
                  +" sur "
                  + QString::number(nb_colors).rightJustified(2,'0')
                  + " -> "
                  + "("+QString::number(leFond.red()).rightJustified(3,'0')
                  + ","+QString::number(leFond.green()).rightJustified(3,'0')
                  + ","+QString::number(leFond.blue()).rightJustified(3,'0')
                  + ","+QString::number(leFond.alpha()).rightJustified(3,'0')
                  +")\n";
    qDebug()<< msg;
#endif
    painter->fillRect(option.rect, leFond);
  }

  /// Colonnes fond bleu
  if(col == Columns::TotalElement || col == Columns::EcartCourant){
    painter->fillRect(option.rect, COULEUR_FOND_TOTAL);
  }

  /// Zone distribution des totaux
  if((col > Columns::TotalElement) && (col <= Columns::TotalElement+nbJ)){
    //painter->save();
    painter->fillRect(option.rect, COULEUR_FOND_DETAILS);
    //painter->restore();
  }

  /// Zone analyses
  if(col > Columns::EcartCourant && col < Columns::TotalElement){
   int d = col - (Columns::EcartCourant+1);
   QColor info[]={COULEUR_FOND_Ep,COULEUR_FOND_Em,
    COULEUR_FOND_EM,COULEUR_FOND_Es,COULEUR_FOND_Me};

	 double ecartCourant = (index.sibling(index.row(),Columns::EcartCourant)).data().toDouble();
	 double current = index.data().toDouble();
	 double radix = 2.0;
	 double pos = fabs(current-ecartCourant);

	 if( pos < radix){
		 painter->fillRect(option.rect, info[d]);
	 }

	 if( (pos < 2*radix) && (pos > radix)){
		painter->save();
		painter->setBrush(Qt::green);
		painter->drawEllipse(c1,cx/2,cy/4);
		painter->restore();
	 }
	}

  QItemDelegate::paint(painter, option, index);
}

bool BDelegateCouleurFond::checkValue(int centre, int pos)const
{
  /// On regarde si sur disque de rayon centre
  bool ret_val = false;
  int distance = abs(centre-pos);

  if(distance <= 12){

    if(distance<= abs(centre)){
      // oui
      ret_val = true;
    }
    else{
      // non
      ret_val = false;
    }
  }


  return ret_val;
}

void BDelegateCouleurFond::slot_AideToolTip(const QModelIndex & index)
{
  const QAbstractItemModel * pModel = index.model();
  int col = index.column();

  QVariant vCol = pModel->headerData(col,Qt::Horizontal);
  QString headName = vCol.toString();

  QTableView *view = qobject_cast<QTableView *>(sender());
  QSortFilterProxyModel *m=qobject_cast<QSortFilterProxyModel*>(view->model());
  sqlqmDetails *sqm_tmp= qobject_cast<sqlqmDetails*>(m->sourceModel());
  QVariant item1 = sqm_tmp->data(index,Qt::BackgroundRole);

  QString msg1="";
  QString s_nb = index.model()->index(index.row(),Columns::vEcart).data().toString();
  msg1 = QString("Boule %1").arg(s_nb);
  if (col >=Columns::EcartCourant && col <=Columns::TotalElement+nbJ)
  {
    QString s_va = index.model()->index(index.row(),col).data().toString();
    QString s_hd = headName;
    msg1 = msg1 + QString("\n%1 = %2").arg(s_hd).arg(s_va);
  }

  QString msg2 = "";
  if(index.column()==Columns::vEcart){
    int val_col_2 = (index.sibling(index.row(),Columns::keyColors)).data().toInt();
    QColor leFond = map_FromColor.key(val_col_2);
    int ligne = map_FromColor.value(leFond,-1);
    double p = (ligne*100)/nb_colors;
    if(map_FromColor.contains(leFond)){
      msg2 = "Critere ecart : "+ QString::number(ligne).rightJustified(2,'0')
             +" sur "
             + QString::number(nb_colors).rightJustified(2,'0')
             + " ("+QString::number(p)+"%)\nRVBA"
             + "("+QString::number(leFond.red()).rightJustified(3,'0')
             + ","+QString::number(leFond.green()).rightJustified(3,'0')
             + ","+QString::number(leFond.blue()).rightJustified(3,'0')
             + ","+QString::number(leFond.alpha()).rightJustified(3,'0')
             +")";
    }
    else{
      msg2 = "Error !!";
    }
  }
  if(index.column()==Columns::vFilters){
    msg2="";
    int val_f = (index.sibling(
                   index.row(),
                   index.model()->columnCount()-1)
                 ).data().toInt();

    if( (val_f & Filtre::isLast)){
      msg2 = msg2+ "presente au dernier tirage\n";
    }
    if(val_f & Filtre::isPrevious){
      msg2 = msg2+ "presente a avant dernier tirage\n";
    }
    if(val_f & Filtre::isWanted){
      msg2 = msg2+ "prise comme filtre\n";
    }
    if(val_f & Filtre::isNever){
      msg2 = msg2+ "jamais sortie dans cette couverture\n";
    }
    if((val_f & Filtre::isPlusOne)){
      msg2 = msg2+ "superieure de 1 d'une boule du tirage precedent\n";
    }
    if((val_f & Filtre::isMinusOne)){
      msg2 = msg2+ "inferieure de 1 d'une boule du tirage precedent\n";
    }



  }

  QString msg = msg1+QString("\n")+msg2;

#ifndef QT_NO_DEBUG
  qDebug() << "Tooltips :" << msg;
#endif

  if(msg.length())
    QToolTip::showText (QCursor::pos(), msg);

}

BDelegateCouleurFond::BDelegateCouleurFond(st_ColorNeeds param, QTableView *parent)
  :QItemDelegate(parent),origine(param.ori)
{
  db_0 = QSqlDatabase::database(param.cnx);
  working_on = param.wko;

  nbE = Columns::TotalElement-Columns::EcartCourant-1;;
  nbJ = param.ori->columnCount() -Columns::TotalElement -3;

  nb_colors = 1+(pow(2,nbE)*2);/// Cas case blanche
  QString tbl_def = QString("pCouleurs_")
                    + QString::number(nb_colors);

  if(isTablePresent(tbl_def)){
    /// Chargement
    MapColorRead(tbl_def);
  }
  else
  {
    MapColorCreate();
    MapColorWrite(tbl_def);
  }

  MapColorApply(parent);


}

bool BDelegateCouleurFond::MapColorRead(QString tbl_def)
{
  bool b_retVal = true;
  QSqlQuery query(db_0);
  QString msg = "Select * from "+tbl_def;

  if((b_retVal = query.exec(msg)))
  {
    query.first();
    if(query.isValid())
    {
      int key = -1;
      /// La table existe
      do{
        key = query.value(0).toInt();
        QString color = query.value(1).toString();
        QColor a;
        a.setNamedColor(color);
        if(!(b_retVal=map_FromColor.contains(a))){
          map_FromColor.insert(a,key-1);
        }
      }while(query.next()&& (b_retVal==false));
      nb_colors = map_FromColor.size();
      if(b_retVal){
#ifndef QT_NO_DEBUG
        qDebug()<<"Erreur : presence couleur deja la pour clef "<<key;
#endif
      }
    }
  }

  if(query.lastError().isValid()){
    DB_Tools::DisplayError("BVisuResume::",&query," recupereMapColor ");
  }

#ifndef QT_NO_DEBUG
  QMapIterator<BOrdColor, int>  val (map_FromColor);
  while (val.hasNext()) {
    val.next();
    qDebug() << val.key() << ": " << val.value() << endl;
  }
  ;
#endif

  return b_retVal;
}

bool BDelegateCouleurFond::isTablePresent(QString tb_name)
{
  bool b_retVal = true;
  QSqlQuery query(db_0);
  QString msg = "SELECT name FROM sqlite_master "
                "WHERE type='table' AND name='"+tb_name+"';";

  if((b_retVal = query.exec(msg)))
  {
    if((b_retVal=query.first())){
      b_retVal = query.isValid();
    }
  }
  return b_retVal;
}

void BDelegateCouleurFond::MapColorApply(QTableView *tbv_cible)
{
  Q_UNUSED(tbv_cible);

  bool b_retVal = true;
  QSqlQuery query(db_0);

  sqlqmDetails *sqm_tmp= qobject_cast<sqlqmDetails*>((this->origine));

  int nb_row = sqm_tmp->rowCount();
  resu_color = new QColor[nb_row];
  for(int row=0; row < nb_row;row++){

    // Pour le table view
    QModelIndex Ec = sqm_tmp->index(row,0);
    QColor leFond = getColorForValue(Ec);
    resu_color[row]=leFond;

#ifndef QT_NO_DEBUG
    QString msg = "Memoire couleur : "+ QString::number(row).rightJustified(2,'0')
                  + " -> "
                  + "("+QString::number(leFond.red()).rightJustified(3,'0')
                  + ","+QString::number(leFond.green()).rightJustified(3,'0')
                  + ","+QString::number(leFond.blue()).rightJustified(3,'0')
                  + ","+QString::number(leFond.alpha()).rightJustified(3,'0')
                  +")\n";
    qDebug()<< msg;
#endif

  }

  // Pour la base
  int taille = map_FromColor.count();

  for(int row=0; (row < nb_row) && b_retVal;row++){
    BOrdColor leFond = resu_color[row];
    int prio = map_FromColor.value(leFond,-1);
#ifndef QT_NO_DEBUG
    QString msg = "map check : "+ QString::number(row).rightJustified(2,'0')
                  +" val "
                  + QString::number(prio).rightJustified(2,'0')
                  + " -> "
                  + "("+QString::number(leFond.red()).rightJustified(3,'0')
                  + ","+QString::number(leFond.green()).rightJustified(3,'0')
                  + ","+QString::number(leFond.blue()).rightJustified(3,'0')
                  + ","+QString::number(leFond.alpha()).rightJustified(3,'0')
                  +")\n";
    qDebug()<< msg;
#endif

    /// update tb_name set C=prio where id=row
    QString st_update = "update "
                        +working_on
                        +" set C='"
                        +QString::number(prio)
                        +QString("' where id = '")
                        +QString::number(row+1)+ "'";
    b_retVal = query.exec(st_update);
  }

  if(query.lastError().isValid()){
    DB_Tools::DisplayError("BDelegateCouleurFond::",&query," na ");
  }
  else{
    // refresh query
    QString st_old_query = sqm_tmp->query().executedQuery();
    sqm_tmp->query().clear();
    sqm_tmp->setQuery(st_old_query,db_0);
  }

}
void BDelegateCouleurFond::MapColorCreate(void)
{
  /// creation d'un tableau des couleurs
  /// pour chacun des calculs d'ecart
  ///  + la couche alpha

  int mid_color = nb_colors/2;
  int step_colors = 255/mid_color;
  int alpha_start = 90;
  int step_alpha = (255-alpha_start)/nb_colors;
  //val_colors = new QColor[nb_colors];

#ifndef QT_NO_DEBUG
  qDebug() << "Couleur : id (R,V,B,A)\n";
#endif

  QColor tmp_color;
  tmp_color.setRed(255);
  tmp_color.setGreen(255);
  tmp_color.setBlue(255);
  tmp_color.setAlpha(255);
  //val_colors[0]=tmp_color;
  map_FromColor.insert(tmp_color,0);
#ifndef QT_NO_DEBUG

  qDebug() << "color in : "<< QString::number(0).rightJustified(2,'0')
           << "("<<QString::number(tmp_color.red()).rightJustified(3,'0')
           << ","<<QString::number(tmp_color.green()).rightJustified(3,'0')
           << ","<<QString::number(tmp_color.blue()).rightJustified(3,'0')
           << ","<<QString::number(tmp_color.alpha()).rightJustified(3,'0')
           <<")\n";
#endif

  for(int i = 1; i< nb_colors; i++){

    if(i<=nb_colors/2){
      tmp_color.setGreen(i*step_colors);
      tmp_color.setRed(255);
    }
    else{
      tmp_color.setGreen(255);
      int calc =255-((i%mid_color)*step_colors);
      tmp_color.setRed(calc);
    }
    tmp_color.setBlue(0);
    tmp_color.setAlpha(alpha_start+((i-1)*step_alpha));

#ifndef QT_NO_DEBUG
    if(i == (nb_colors/2)+1){
      qDebug() << "\n\n---------\n\n";
    }

    qDebug() << "color in : "<< QString::number(i).rightJustified(2,'0')
             << "("<<QString::number(tmp_color.red()).rightJustified(3,'0')
             << ","<<QString::number(tmp_color.green()).rightJustified(3,'0')
             << ","<<QString::number(tmp_color.blue()).rightJustified(3,'0')
             << ","<<QString::number(tmp_color.alpha()).rightJustified(3,'0')
             <<")\n";
#endif
    //val_colors[i]=tmp_color;

    /// Rechercher si cette couleur existe
    /// deja comme clef
    if(map_FromColor.contains(tmp_color)){
#ifndef QT_NO_DEBUG
      qDebug() << "Cette couleur comme clef existe deja\n\n";
#endif
      ///QMessageBox::critical(NULL, "Pgm", "Clef Couleur deja presente\n",QMessageBox::Ok);
    }
    else{
      map_FromColor.insert(tmp_color,i);
    }
  }

#ifndef QT_NO_DEBUG
  qDebug() << "\n---------\n";

  QMapIterator<BOrdColor, int>  val (map_FromColor);
  while (val.hasNext()) {
    val.next();
    //qDebug() << val.key() << "\t: " << val.value() << endl;
    qDebug() << "color ou : "<< QString::number(val.value()).rightJustified(2,'0')
             << "("<<QString::number(val.key().red()).rightJustified(3,'0')
             << ","<<QString::number(val.key().green()).rightJustified(3,'0')
             << ","<<QString::number(val.key().blue()).rightJustified(3,'0')
             << ","<<QString::number(val.key().alpha()).rightJustified(3,'0')
             <<")\n";
  }
  ;
#endif
}

bool BDelegateCouleurFond::MapColorWrite(QString tbl_def)
{
  bool b_retVal = true;
  QSqlQuery query(db_0);

  QString msg []= {
    {"SELECT name FROM sqlite_master "
     "WHERE type='table' AND name='"+tbl_def+"';"},
    {"create table if not exists "
     + tbl_def
     +"(id integer primary key, color text)"}
  };

  if((b_retVal = query.exec(msg[0])))
  {
    query.first();
    if(query.isValid())
    {
      /// La table existe
      b_retVal = true;
    }
    else{
      /// il faut la creer et la remplir
      if((b_retVal = query.exec(msg[1]))){
        for(int i =0; (i < nb_colors) && b_retVal ;i++){
          QString str_insert = QString(" insert into ")
                               + tbl_def
                               + QString(" values(NULL,'")
                               + map_FromColor.key(i).name(QColor::HexRgb)
                               +QString("')");
          b_retVal = query.exec(str_insert);
        }
      }
    }
  }

  if(query.lastError().isValid()){
    DB_Tools::DisplayError("BDelegateCouleurFond::",&query," na ");
  }

  return b_retVal;
}

QColor BDelegateCouleurFond::getColorForValue(const QModelIndex &index) const
{
  QColor color;

  int val = 0;
  int r = 2;

  /// Ep,Em,M,Es,Me
  int tab_pri[5]={3,4,1,2,5};
  int nb_pri = sizeof (tab_pri)/sizeof (int);

  int Ec = (index.sibling(index.row(),
                          Columns::EcartCourant)).data().toInt();
  for(int i=1;i<=nb_pri;i++){
    int centre = (index.sibling(index.row(),Columns::EcartCourant+i)).data().toInt();
    int pri = tab_pri[i-1]-1;

    int item_couleur = checkValue(centre,Ec);
    int item_alpha = checkValue(r,(centre-Ec));

    if(i==3){
      /// Retirer cas R=Max Ecart
      /// sauf si vraiment tout proche
      item_couleur = item_alpha * item_couleur;
    }

    val = val + pow(2,pri) * (item_couleur*pow(2,item_alpha));

  }

  /// ----------------
  if(val<nb_colors){
    //color = val_colors[val];
    color = map_FromColor.key(val);
  }
  else{
    /// Erreur de calcul !!
    color.setRed(39);
    color.setGreen(93);
    color.setBlue(219);
    color.setAlpha(255);
  }

  return color;
}

bool BDelegateCouleurFond::setData(const QModelIndex &index, const QVariant &value, int role) const
{
  //sqlqmDetails *sqlqm_tmp = qobject_cast<sqlqmDetails *>origine;

  return(origine->setData(index,value,Qt::BackgroundRole));
}

