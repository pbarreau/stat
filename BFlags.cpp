#ifndef QT_NO_DEBUG
#include <QDebug>
#endif

#include <QMessageBox>

#include <QSqlError>
#include <QSqlQuery>

#include <QLineEdit>

#include <QAbstractTextDocumentLayout>
#include <QTextDocument>
#include <QStyledItemDelegate>
#include <QSortFilterProxyModel>
#include <QApplication>
#include <QTableView>

#include <QPainter>
#include <QStyle>
#include <QStyleOption>
#include <QStylePainter>
#include <QStyleOptionViewItem>
#include <QFontMetrics>
#include <QPalette>
#include <QRect>

#include "BFlags.h"
#include "db_tools.h"

BFlags::BFlags(stPrmDlgt prm) : QStyledItemDelegate(prm.parent)//, QMainWindow(prm.parent)
{
 flt = prm;

 QString cnx=prm.db_cnx;
 db_1 = QSqlDatabase::database(cnx);
 if(db_1.isValid()==false){
  QString str_error = db_1.lastError().text();
  QMessageBox::critical(nullptr, cnx, str_error,QMessageBox::Yes);
  return;
 }

}


void BFlags::paint(QPainter *painter, const QStyleOptionViewItem &option,
                   const QModelIndex &index) const
{
 v3_paint(painter,option,index);
}

void BFlags::v3_paint(QPainter *painter, const QStyleOptionViewItem &option,
                      const QModelIndex &index) const
{
 /// https://openclassrooms.com/forum/sujet/qt-qtableview-qstyleditemdelegate
 /// https://forum.qt.io/topic/41463/solved-qstyleditemdelegate-set-text-color-when-row-is-selected
 /// https://openclassrooms.com/forum/sujet/recuperer-les-ligne-selectionnees-dans-un-qitemdelegate-35100
 /// https://stackoverflow.com/questions/34729858/override-text-in-qstyleditemdelegate-for-qtreeview
 ///
 /// https://code.qt.io/cgit/qt/qt.git/tree/src/gui/styles/qstyle.cpp?h=4.5
 /// ligne 489 , QStyle::drawItemText
 ///
 /// https://code.qt.io/cgit/qt/qtbase.git/tree/src/widgets/itemviews/qstyleditemdelegate.cpp?h=dev

 Q_ASSERT(index.isValid());

 stTbFiltres a;
 a.tbName = "Filtres";
 a.sta = Bp::E_Sta::noSta;
 a.db_total = -1;
 a.b_flt = flt.b_flt;
 a.zne = flt.zne;
 a.typ = flt.typ;
 a.lgn = index.row();
 a.col = index.column();
 a.val = -1;
 a.pri = -1;


 bool ret = getThisFlt(&a, flt.typ, index);
 setVisual(ret, &a, painter, option, index);
 }

 void BFlags::setVisual(const bool isPresent, stTbFiltres *a,QPainter *painter, const QStyleOptionViewItem &option,
                        const QModelIndex &index) const
 {
  QStyleOptionViewItem myOpt = option;
  initStyleOption(&myOpt, index);

	/// Gestion des graphiques
	fltDraw(isPresent, a, painter, myOpt, index);


	/// Gestion du texte
	fltWrite(isPresent, a, painter, myOpt, index);
 }

void BFlags::setWanted(bool state, QPainter *painter, const QStyleOptionViewItem &opt, stTbFiltres *a, const QModelIndex &index) const
{
 /// BUG : il faut cliquer pour voir apparaitrela couleur.
 /// QTableView *tmp = static_cast<QTableView *>(opt.styleObject);
 /// Qt::ItemDataRole f;

 /// Gestion des graphiques
 fltDraw(state, a,painter,opt,index);

 QStyleOptionViewItem myOpt = opt;
 initStyleOption(&myOpt, index);

 QPainter p;

 QRect Cellrect = myOpt.rect;

 Qt::Alignment alg;
 alg = Qt::AlignTop|Qt::AlignLeft;
 QPalette pal;
 QString try_txt = QString::number(myOpt.text.toInt()).rightJustified(2,'0');
 int alignment = static_cast<int>(alg);

 QFont myFont;
 myFont.setFamily("ARIAL");
 myFont.setPointSize(7);
 myFont.setWeight(QFont::Normal);
 myFont.setItalic(true);

 /// Calcul de l'epace pour le texte
 QFontMetrics qfm(myFont);
 QRect space = QApplication::style()->itemTextRect(qfm, Cellrect, alignment, false, try_txt);

 painter->save();
 painter->setFont(myFont);
 //QApplication::style()->drawControl(QStyle::CE_ItemViewItem, &myOpt, painter, nullptr);
 //myOpt.font.setItalic(true);


 QApplication::style()->drawItemText(
  painter,
  space,
  alignment,
  pal,
  false,
  try_txt,
  QPalette::ColorRole::Text);

 painter->restore();
 return;

#if 0
 int size = 0;
 Qt::GlobalColor pen;
 //Qt::Alignment alg;

 if((a->b_flt & Bp::Filtering::isWanted) == Bp::Filtering::isWanted){
  size= 7;
  alg = Qt::AlignTop|Qt::AlignLeft;
 }
 else {
  size = 10;
  alg = Qt::AlignCenter | Qt::AlignVCenter;
 }

 if( (a->b_flt & Bp::Filtering::isWanted) == Bp::Filtering::isWanted){
  pen = Qt::green;
 }
 else if ((a->b_flt & Bp::Filtering::isFiltred) == Bp::Filtering::isFiltred) {
  pen = Qt::red;
 }
 else {
  pen = Qt::black;
 }

 myOpt.palette.setColor(QPalette::Active, QPalette::Text, pen);
 myOpt.displayAlignment = alg;

 myOpt.text = QString::number(myOpt.text.toInt()).rightJustified(2,'0');

 myOpt.font.setFamily("ARIAL");
 myOpt.font.setPointSize(size);

 //opt.font.setItalic(true);
 //opt.font.setBold(true);

 /*
 QApplication::style()->drawItemText(
  painter,
  Cellrect,
  alignment,
  pal,
  false,
  try_txt);
*/

 /// drawFltKey(a,painter,myOpt,index);
 /// reponse incomplete : QApplication::style()->drawControl(QStyle::CE_ItemViewItem, &myOpt, painter, nullptr);
#endif
}

#if 1
bool BFlags::getThisFlt(stTbFiltres *val, const etCount in_typ, const QModelIndex index) const
{
 bool isOk = false;
 val->zne =flt.zne;
 val->typ = in_typ;
 val->b_flt = flt.b_flt;

 if(val->typ >= eCountToSet && val->typ <= eCountEnd){
  switch (val->typ) {
   case eCountElm:
   case eCountCmb:
   case eCountBrc:
    val->lgn = val->typ *10;
    val->col = index.model()->index(index.row(),0).data().toInt();
    val->val = val->col;
    break;
   case eCountGrp:
    val->lgn = index.row();
    val->col = index.column();
    if(index.model()->index(val->lgn,val->col).data().canConvert(QMetaType::Int)){
     val->val = index.model()->index(val->lgn,val->col).data().toInt();
    }
    break;
   case eCountToSet:
   case eCountEnd:
    break;
  }
 }
 else {
  val->typ = eCountToSet;
 }

 isOk = DB_Tools::tbFltGet(val,flt.db_cnx);

 return isOk;
}
#else
bool BFlags::getdbFlt(stTbFiltres *ret, const etCount in_typ, const QModelIndex index) const
{
 bool isOk = false;
#if 0
 etCount typ = in_typ;

 int zn  = flt.zne;
 int lgn = -1;
 int col = -1;
 int val = -1;
 Bp::F_Flts my_flt = Bp::Filtering::isNotSet;


 if(typ >= eCountToSet && typ <= eCountEnd){
  switch (typ) {
   case eCountElm:
   case eCountCmb:
   case eCountBrc:
    lgn = typ *10;
    col = index.model()->index(index.row(),0).data().toInt();
    val = col;
    break;
   case eCountGrp:
    lgn = index.row();
    col = index.column();
    if(index.model()->index(lgn,col).data().canConvert(QMetaType::Int)){
     val = index.model()->index(lgn,col).data().toInt();
    }
    break;
   case eCountToSet:
   case eCountEnd:
    break;
  }
 }
 else {
  typ = eCountToSet;
 }

 QSqlQuery query_2(db_1);
 QString tbFiltre = (*ret).tbName;

 /// Verifier si info presente dans table
 QString msg = "Select *  from "+tbFiltre
               +" where ("
                 "zne="+QString::number(zn)+" and "+
               "typ="+QString::number(typ)+" and "+
               "lgn="+QString::number(lgn)+" and "+
               "col="+QString::number(col)+" and "+
               "val="+QString::number(val)+")";

#ifndef QT_NO_DEBUG
 qDebug() << "mgs_2: "<<msg;
#endif
 isOk = query_2.exec(msg);

 if((isOk = query_2.first()))
 {
  int valeur = query_2.value("flt").toInt();
  int priori = query_2.value("pri").toInt();
  my_flt = static_cast<Bp::F_Flts>(valeur);

  (*ret).pri = priori;
  (*ret).b_flt = my_flt;
  (*ret).flt = Bp::Filtering::isNotSet;//query_2.value("flt").toInt();

 }

 (*ret).sta = isOk;
 (*ret).zne = zn;
 (*ret).typ = typ;
 (*ret).lgn = lgn;
 (*ret).col = col;
 (*ret).val = val;
#endif
 return isOk;
}
#endif

void BFlags::cellWrite(QPainter *painter, QRect curCell, const QString myTxt, Qt::GlobalColor inPen, bool up)const
{
 QFont myFnt;
 QPalette myPal;
 Qt::Alignment myAlg;
 Qt::GlobalColor myPen=inPen;

 QString font_family = "ARIAL";
 int font_weight = QFont::Normal;
 int alignment = 0;
 int size = 0;
 bool b_italic = false;

 if(up==true){
  size= 7;
  myAlg = Qt::AlignTop|Qt::AlignLeft;
  b_italic = true;
 }
 else {
  size = 10;
  myAlg = Qt::AlignCenter | Qt::AlignVCenter;
  b_italic = false;
 }

 myFnt.setPointSize(size);
 myFnt.setWeight(font_weight);
 myFnt.setItalic(b_italic);
 alignment = static_cast<int>(myAlg);


 /// Calcul de l'espace pour le texte
 QFontMetrics qfm(myFnt);
 QRect space = QApplication::style()->itemTextRect(qfm, curCell, alignment, false, myTxt);

 painter->save();

 painter->setFont(myFnt);
 myPal.setColor(QPalette::Active, QPalette::Text, myPen);
 QApplication::style()->drawItemText(painter,space,alignment,myPal,true,myTxt,QPalette::ColorRole::Text);

 painter->restore();
}

void BFlags::fltWrite(bool isPresent, stTbFiltres *a, QPainter *painter, const QStyleOptionViewItem &maModif,
                     const QModelIndex &index) const
{
 Q_UNUSED(isPresent)

 QStyleOptionViewItem myOpt = maModif;
 initStyleOption(&myOpt, index);

 QFont myFnt;
 QPalette myPal;
 Qt::GlobalColor myPen=Qt::black;
 bool set_up = false;

 QString myTxt = myOpt.text;
 QRect curCell = myOpt.rect;
 int cur_col = index.column();

 int painting_col = -1;
 switch (a->typ) {
  case eCountElm:
  case eCountCmb:
  case eCountBrc:
   painting_col = 1;
   if(cur_col != painting_col ){
    cellWrite(painter,curCell,myTxt);
    return;
   }
  break;
  case eCountGrp:
   painting_col = 0;
   break;
  default:
   painting_col = -1;
   break;
 }

 if(cur_col==painting_col){

	if((a->typ == eCountElm)||(a->typ == eCountGrp)){
	 myTxt = QString::number(myOpt.text.toInt()).rightJustified(2,'0');
	}

	if(a->typ == eCountBrc){
	 /// https://stackoverflow.com/questions/7234824/format-a-number-to-a-specific-qstring-format
	 QStringList tmp_lst = myTxt.split(",");
	 if(tmp_lst.size()>1){
		myTxt=tmp_lst[0]+","+tmp_lst[1].leftJustified(2,'0');
	 }
	 else {
		myTxt=myTxt+","+"00";
	 }
	}
 }

 if((a->b_flt & Bp::F_Flt::fltWanted) == Bp::F_Flt::fltWanted){
  set_up = true;
  myPen = Qt::green;
 }

 if (
  (a->b_flt & Bp::F_Flt::fltSelected) == Bp::F_Flt::fltSelected
                                            ||
  (a->b_flt & Bp::F_Flt::fltFiltred) == Bp::F_Flt::fltFiltred

  ) {
  set_up = false;
  myPen = Qt::red;
 }

 cellWrite(painter,curCell,myTxt,myPen,set_up);

}

void BFlags::fltDraw(bool isPresent, stTbFiltres *a, QPainter *painter, const QStyleOptionViewItem &maModif,
                const QModelIndex &index) const
{
 Q_UNUSED(isPresent)

 int col = index.column();

 /// Test faisabilite

 switch (a->typ) {
  case eCountElm:
  case eCountCmb:
  case eCountBrc:
   if(col != flt.start ){
    return;
   }
   break;
  default: ;// Rien
 }

 QColor v[]= {
  Qt::black,
  Qt::red,
  Qt::green,
  QColor(255,216,0,255),
  QColor(255,106,0,255),
  QColor(178,0,255,255),
  QColor(211,255,204,255)
 };
 int nbColors = sizeof (v)/sizeof (QColor);

 QRect Cellrect = maModif.rect;


 int refx = Cellrect.topLeft().x();
 int refy = Cellrect.topLeft().y();
 int ctw = Cellrect.width();  /// largeur cellule
 int cth = Cellrect.height(); /// Hauteur cellule
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

 /// ------------------
 painter->save();
 /// ------------------
 painter->setRenderHint(QPainter::Antialiasing, true);

 if(a->typ==eCountElm){
  if(((a->pri)>0) && ((a->b_flt & Bp::F_Flt::fltFiltred) == (Bp::F_Flt::fltFiltred))){
   painter->fillRect(maModif.rect, COULEUR_FOND_FILTRE);
  }
 }
 else {
  if(
   ((a->b_flt & Bp::F_Flt::fltWanted) == (Bp::F_Flt::fltWanted)) && ((a->b_flt & Bp::F_Flt::fltFiltred) == (Bp::F_Flt::fltFiltred))){
   painter->fillRect(maModif.rect, COULEUR_FOND_FILTRE);
  }
 }

 if( (a->b_flt & Bp::F_Flt::fltTirLast) == (Bp::F_Flt::fltTirLast)){
  painter->fillRect(r2, COULEUR_FOND_DERNIER);
 }

 if((a->b_flt & Bp::F_Flt::fltTirPrev) == (Bp::F_Flt::fltTirPrev)){
  painter->fillRect(r3, COULEUR_FOND_AVANTDER);
 }


 if((a->b_flt & Bp::F_Flt::fltSeenNot) == (Bp::F_Flt::fltSeenNot)){
  painter->fillRect(r2, COULEUR_FOND_JAMSORTI);
 }

 /// Mettre les cercles maintenant car les fonds
 /// snt deja dessinee
 if((a->pri > 0) && (a->pri<nbColors)){

	painter->setBrush(v[a->pri]);
	painter->drawEllipse(c1,cx/2,cy/4);

 }


 /// ------------------
 painter->restore();
 /// ------------------
}
