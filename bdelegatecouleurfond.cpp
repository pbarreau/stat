#include <QToolTip>

#include "bdelegatecouleurfond.h"
#include "db_tools.h"

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
  QPoint c(refx +(ctw/5)*4,refy + (cth/5)*4);

  QPolygon triangle;
  QPoint t1(refx,refy);
  QPoint t2(refx+ctw,refy);
  QPoint t3(refx,refy+cy);
  triangle << t1<<t2<<t3<<t1;


  if(col == Columns::CelInfo){
    int val_f = (index.sibling(
                   index.row(),
                   index.model()->columnCount()-1)
                 ).data().toInt();

    if(val_f & Filtre::isLast){
      painter->save();
      painter->fillRect(option.rect, COULEUR_FOND_DERNIER);
      painter->restore();
    }

    if(val_f & Filtre::isPrevious){
      // montrer un cercle
      painter->save();
      painter->setRenderHint(QPainter::Antialiasing, true);
      painter->setBrush(COULEUR_FOND_AVANTDER);
      painter->drawEllipse(c,cx/2,cy/4);
      painter->restore();
    }

    if(val_f & Filtre::isWanted){
      /// montrer triangle
      painter->save();
      painter->setRenderHint(QPainter::Antialiasing, true);

      painter->setBrush(COULEUR_FOND_FILTRE);
      painter->drawPolygon(triangle);
      painter->restore();
    }

    if(val_f & Filtre::isNever){
      painter->save();
      painter->fillRect(option.rect, COULEUR_FOND_JAMSORTI);
      painter->restore();
    }
  }

  if(col == COL_VISU_ECART ){
    int val_col_2 = (index.sibling(index.row(),COL_VISU_ECART-1)).data().toInt();
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

  if(col >= b_min && col < b_max){
    painter->fillRect(option.rect, COULEUR_FOND_DETAILS);
  }

  if(col >= b_max && col < b_max+len){
    painter->fillRect(option.rect, COULEUR_FOND_ECARTS);
  }

  QItemDelegate::paint(painter, option, index);
}

bool BDelegateCouleurFond::isOnDisk(int centre, int pos)const
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
  QString s_nb = index.model()->index(index.row(),COL_VISU_ECART).data().toString();
  msg1 = QString("Boule %1").arg(s_nb);
  if (col >=b_min && col <=b_max+6)
  {
    QString s_va = index.model()->index(index.row(),col).data().toString();
    QString s_hd = headName;
    msg1 = msg1 + QString("\n%1 = %2").arg(s_hd).arg(s_va);
  }

  QString msg2 = "";
  if(index.column()==COL_VISU_ECART){
    int val_col_2 = (index.sibling(index.row(),COL_VISU_ECART-1)).data().toInt();
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

  QString msg = msg1+QString("\n")+msg2;

#ifndef QT_NO_DEBUG
  qDebug() << "Tooltips :" << msg;
#endif

  if(msg.length())
    QToolTip::showText (QCursor::pos(), msg);

}

BDelegateCouleurFond::BDelegateCouleurFond(st_ColorNeeds param, QTableView *parent)
  :QItemDelegate(parent),b_min(param.b_min),b_max(param.b_max),len(param.len),origine(param.ori)
{
  db_0 = QSqlDatabase::database(param.cnx);
  working_on = param.wko;

  /// Mise en place d'un toolstips
  parent->setMouseTracking(true);
  connect(parent,
          SIGNAL(entered(QModelIndex)),
          this,SLOT(slot_AideToolTip(QModelIndex)));


  /*
  /// Deeinition representation
  starPolygon << QPointF(0.0, 0.0);
  starPolygon << QPointF(1.0, 0.0);
  starPolygon << QPointF(0.0, 0.5);
*/

  starPolygon << QPointF(1.0, 0.5);
  for (int i = 1; i < 5; ++i)
    starPolygon << QPointF(0.5 + 0.5 * std::cos(0.8 * i * 3.14),
                           0.5 + 0.5 * std::sin(0.8 * i * 3.14));

  CreationTableauClefDeCouleurs();

  AffectationCouleurResultat(parent);


}

void BDelegateCouleurFond::AffectationCouleurResultat(QTableView *tbv_cible)
{
  bool isOk = true;
  QSqlQuery query(db_0);

  sqlqmDetails *sqm_tmp= qobject_cast<sqlqmDetails*>((this->origine));

  int nb_row = sqm_tmp->rowCount();
  resu_color = new QColor[nb_row];
  for(int row=0; row < nb_row;row++){

    // Pour le table view
    QModelIndex Ec = sqm_tmp->index(row,0);
    QColor leFond = CalculerCouleur(Ec);
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

  for(int row=0; (row < nb_row) && isOk;row++){
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
    isOk = query.exec(st_update);
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
void BDelegateCouleurFond::CreationTableauClefDeCouleurs(void)
{
  /// creation d'un tableau des couleurs
  /// pour chacun des calculs d'ecart
  ///  + la couche alpha
  nb_colors = 1+(pow(2,len-1)*2);/// Cas case blanche

  int mid_color = nb_colors/2;
  int step_colors = 255/mid_color;
  int alpha_start = 90;
  int step_alpha = (255-alpha_start)/nb_colors;
  val_colors = new QColor[nb_colors];

#ifndef QT_NO_DEBUG
  qDebug() << "Couleur : id (R,V,B,A)\n";
#endif

  QColor tmp_color;
  tmp_color.setRed(255);
  tmp_color.setGreen(255);
  tmp_color.setBlue(255);
  tmp_color.setAlpha(255);
  val_colors[0]=tmp_color;
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
    val_colors[i]=tmp_color;
    //BOrdColor a(tmp_color);

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


  /// Le tableau est cree, le sauver dans la base de donnees
  SauverTableauPriotiteCouleurs();
}

bool BDelegateCouleurFond::SauverTableauPriotiteCouleurs()
{
  bool isOk = true;
  QSqlQuery query(db_0);
  QString tb_name = QString("pCouleurs_")
                    + QString::number(nb_colors);

  QString msg []= {
    {"SELECT name FROM sqlite_master "
     "WHERE type='table' AND name='"+tb_name+"';"},
    {"create table if not exists "
     + tb_name
     +"(id integer primary key, color text)"}
  };

  if((isOk = query.exec(msg[0])))
  {
    query.first();
    if(query.isValid())
    {
      /// La table existe
      isOk = true;
    }
    else{
      /// il faut la creer et la remplir
      if((isOk = query.exec(msg[1]))){
        for(int i =0; (i < nb_colors) && isOk ;i++){
          QString str_insert = QString(" insert into ")
                               + tb_name
                               + QString(" values(NULL,'")
                               + val_colors[i].name(QColor::HexRgb)
                               +QString("')");
          isOk = query.exec(str_insert);
        }
      }
    }
  }

  if(query.lastError().isValid()){
    DB_Tools::DisplayError("BDelegateCouleurFond::",&query," na ");
  }

  if(isOk){
    // emettre le nom du tableau des couleurs
    emit sig_TableDesCouleurs(tb_name);
  }
  return isOk;
}
QColor BDelegateCouleurFond::CalculerCouleur(const QModelIndex &index) const
{
  QColor color;

  int val = 0;
  int r = 2;

  /// Ep,Em,M,Es,Me
  int tab_pri[5]={3,4,1,2,5};
  int nb_pri = sizeof (tab_pri)/sizeof (int);

  int Ec = (index.sibling(index.row(),b_max)).data().toInt();
  for(int i=1;i<=nb_pri;i++){
    int centre = (index.sibling(index.row(),b_max+i)).data().toInt();
    int pri = tab_pri[i-1]-1;

    int item_couleur = isOnDisk(centre,Ec);
    int item_alpha = isOnDisk(r,(centre-Ec));

    if(i==3){
      /// Retirer cas R=Max Ecart
      /// sauf si vraiment tout proche
      item_couleur = item_alpha * item_couleur;
    }

    val = val + pow(2,pri) * (item_couleur*pow(2,item_alpha));

  }

  /// ----------------
#if 0
  if(val == 0){
    color.setRgb(255,255,255,255);
  }
  else
#endif
  {
    if(val<nb_colors){
      color = val_colors[val];
    }
    else{
      /// Erreur de calcul !!
      color.setRed(39);
      color.setGreen(93);
      color.setBlue(219);
      color.setAlpha(255);
    }


  }

  return color;
}

bool BDelegateCouleurFond::setData(const QModelIndex &index, const QVariant &value, int role) const
{
  //sqlqmDetails *sqlqm_tmp = qobject_cast<sqlqmDetails *>origine;

  return(origine->setData(index,value,Qt::BackgroundRole));
}

