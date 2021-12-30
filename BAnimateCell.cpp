#include <QApplication>

#include <QTimer>
#include <QDateTime>
#include <QSqlQueryModel>

#include <QPainter>
#include <QStyle>
#include <QStyleOption>
#include <QStylePainter>
#include <QStyleOptionViewItem>
#include <QFontMetrics>
#include <QPalette>
#include <QRect>

#include "BAnimateCell.h"
#include "BFpm_upl.h"
#include "colors.h"

/// https://stackoverflow.com/questions/54035370/how-to-animate-the-color-of-a-qtableview-cell-in-time-once-its-value-gets-updat
/// https://www.titanwolf.org/Network/q/0f38c86c-dda4-4b9b-9afe-dfb7c6c441f2/y
/// https://stackoverflow.com/questions/12527141/how-can-i-use-a-qfuturewatcher-with-qtconcurrentrun-without-a-race-condition
BAnimateCell::BAnimateCell(BView *view):m_view(view),QStyledItemDelegate(nullptr)
{
 BFpm_upl *m = qobject_cast<BFpm_upl *>(view->model());
 QSqlQueryModel *vl = qobject_cast<QSqlQueryModel *>(m->sourceModel());
 nb_col = vl->columnCount();

 QTimer * timer = new QTimer( this );
 connect( timer, &QTimer::timeout, this, &BAnimateCell::BSlot_animate);


 //timer->start( TIME_RESOLUTION );
}

eUpl_Cal BAnimateCell::addKey(int key)
{
 eUpl_Cal cur_cal = eCalPending;

 setKey(key,Qt::green, cur_cal);

 return cur_cal;
}

eUpl_Cal BAnimateCell::startKey(int key)
{
 eUpl_Cal cur_cal = eCalStarted;

 setKey(key,Qt::red, cur_cal);

 return cur_cal;
}

eUpl_Cal BAnimateCell::delKey(int key)
{
 eUpl_Cal cur_cal = eCalNotSet;

 setKey(key,Qt::gray, cur_cal);

 return cur_cal;
}

bool BAnimateCell::gotKey(int key, eUpl_Cal *curCal)
{
 bool ret_val = false;

 QMap<int, QVariant>::const_iterator it = mapTimeout.find( key );

 if(it != mapTimeout.end()){

  if(curCal != nullptr){
   st_cellData conf;
   QVariant item = it.value();
   conf = item.value<st_cellData>();
   *curCal = conf.e_cal;
  }

  ret_val = true;
 }

 return ret_val;
}

bool BAnimateCell::gotKeyReady(int key)
{
 QMap<int, QVariant>::const_iterator it = mapCal_Ready.find( key );

 return (it == mapCal_Ready.end() ?  false : true);
}

bool BAnimateCell::gotKeyUsr(int key)
{
 QMap<int, QVariant>::const_iterator it = mapUserSelect.find( key );

 return (it == mapUserSelect.end() ?  false : true);
}

bool BAnimateCell::isShowing(int key)
{
 bool ret_val = false;

 if(gotKeyReady(key) == true){

  if(gotKeyShowing(key) == true){
   ret_val = true;
  }
  else{
   QVariant value = mapCal_Ready.value(key);
   mapShowingKey.clear();
   mapShowingKey.insert(key,value);
  }
 }

 return ret_val;
}

int BAnimateCell::countReady()
{
 return mapCal_Ready.size();
}

void BAnimateCell::setCalReady(int key)
{
 st_cellData conf;
 conf.color = Qt::white;
 conf.e_cal = eCalReady;
 QVariant info;
 info.setValue(conf);

#if 0
 eUpl_Cal cur_cal = eCalNotDef;
 if(gotKey(key,&cur_cal) == true){
  if (cur_cal == eCalReady){
   mapTimeout.remove(key);

   /// ----------------
   mapCal_Ready.insert(key,info);
   /// ----------------

   emit BSig_Repaint(m_view);
  }
 }
#endif
 mapTimeout.remove(key);

 /// ----------------
 mapCal_Ready.insert(key,info);
 /// ----------------

 emit BSig_Repaint(m_view);
}

eUpl_Cal BAnimateCell::setUserSelect(int key)
{
 eUpl_Cal new_val = eCalNotDef;
 QVariant item;

 QMap<int, QVariant>::const_iterator it;

 it = mapUserSelect.find( key );

 /// Cette clef est elle deja dans l'ensemble utilisateur
 if(it !=mapUserSelect.end() ){

  /// La mettre dans la file commune comme non selectionnee
  new_val = delKey(key);

  /// La retirer de cette file
  mapUserSelect.remove(key);
 }
 else{
  /// La mettre dans la file commune comme selectionnee
  new_val = addKey(key);

  /// dupliquer la clef
  it = mapTimeout.find( key );
  item = it.value();

  st_cellData conf = item.value<st_cellData>();
  //conf.upl_txt = upl_txt;

  //item.value<st_cellData>().upl_txt = upl_txt;

  mapUserSelect.insert(key, item);
 }

 return new_val;
}

QString BAnimateCell::itemsSelected()
{
 QString ret_val = "";

 if(mapUserSelect.size() != 0){
  QMap<int, QVariant>::const_iterator it;
  for(it=mapUserSelect.begin(); it != mapUserSelect.end(); it++){
   int key = it.key();
   ret_val = ret_val + QString::number(key);
   if(it != mapUserSelect.end() -1){
    ret_val = ret_val + ",";
   }
  }
 }

 return ret_val;
}

///
////// \brief BAnimateCell::setKey
////// \param key
////// \param color
void BAnimateCell::setKey(int key, QColor color, eUpl_Cal eCal)
{
 st_cellData conf;
 conf.color = color;
 conf.e_cal = eCal;
 QVariant info;
 info.setValue(conf);

 mapTimeout.insert(key,info);
 emit BSig_Repaint(m_view);
}

void BAnimateCell::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
 QStyleOptionViewItem myOpt = option;
 initStyleOption(&myOpt, index);

 int col = myOpt.index.column();
 ///int row = myOpt.index.row();

 /// Traitement particulier pour la colonne resultat
 if((col == nb_col-1)){
  int key = index.sibling(index.row(),0).data().toInt();

  FormalizeCell(key, painter, myOpt, index);
 }
 else{
  /// Revenir sur le traitement par defaut pour les autres
  QStyledItemDelegate::paint(painter,myOpt,index);
 }

}

void BAnimateCell::FormalizeCell(int key, QPainter *painter, const QStyleOptionViewItem &myOpt, const QModelIndex &index) const
{
 QRect cur_rect = myOpt.rect;


 /// Mettre la couleur dans la cellule selon l'etat
 QMap<int, QVariant>::const_iterator it = mapTimeout.find( key );
 if(it !=mapTimeout.end() ){
  st_cellData conf;
  QVariant item = it.value();
  conf = item.value<st_cellData>();
  painter->fillRect(cur_rect, conf.color);
 }

 /// Adapter le texte pour Montrer les calculs termines
 it = mapCal_Ready.find( key );
 if(it != mapCal_Ready.end()){
  /// ------
  QString myTxt = myOpt.text;
  QFont myFnt;
  QPalette myPal;
  Qt::GlobalColor myPen = Qt::black;
  Qt::Alignment myAlg = Qt::AlignCenter | Qt::AlignVCenter;
  int alignment = static_cast<int>(myAlg);
  int font_weight = QFont::Normal;
  int size = 10;

  myFnt.setPointSize(size);
  myFnt.setWeight(font_weight);
  myFnt.setItalic(false);
  myFnt.setBold(true);

  /// Calcul de l'espace pour le texte
  QFontMetrics qfm(myFnt);
  QRect space = QApplication::style()->itemTextRect(qfm, cur_rect, alignment, true, myTxt);

  /// -----------
  painter->save();

  painter->setFont(myFnt);
  myPal.setColor(QPalette::Active, QPalette::Text, myPen);
  QApplication::style()->drawItemText(painter,space,alignment,myPal,true,myTxt,QPalette::ColorRole::Text);

  painter->restore();
  /// -----------
 }
 else {
  QStyledItemDelegate::paint(painter,myOpt,index);
 }
}

bool BAnimateCell::gotKeyShowing(int key)
{
 QMap<int, QVariant>::const_iterator it = mapShowingKey.find( key );

 return (it == mapShowingKey.end() ?  false : true);
}

void BAnimateCell::updateNbColumns()
{
 BFpm_upl *m = qobject_cast<BFpm_upl *>(m_view->model());
 QSqlQueryModel *vl = qobject_cast<QSqlQueryModel *>(m->sourceModel());

 if(nb_col == 0){
  nb_col = vl->columnCount();
 }
 ;
}

QVariant BAnimateCell::data(const QModelIndex &idx, int role) const
{
 if( role != ItemModifiedRole );
 //return BView::data( idx, role );

}

void BAnimateCell::BSlot_animate()
{

}

void BAnimateCell::BSlot_over(const QModelIndex &index)
{
 int lgn = index.row();

 mapTimeout.insert(lgn,QDateTime::currentDateTime());

}
