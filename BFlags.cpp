#ifndef QT_NO_DEBUG
#include <QDebug>
#endif

#include <math.h>

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

QString BFlags::ViewDetails = "Details";

BFlags::BFlags(const BFlt *conf) : QStyledItemDelegate(nullptr),BFlt(*conf)
{
 db_1 = db_flt;
}


void BFlags::paint(QPainter *painter, const QStyleOptionViewItem &option,
                   const QModelIndex &index) const
{
 displayTbv_cell(painter,option,index);
}

void BFlags::displayTbv_cell(QPainter *painter, const QStyleOptionViewItem &option,
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

 bool b_retVal = false;

 inf_flt->id = -1;
 inf_flt->dbt = -1;
 inf_flt->b_flt = Bp::F_Flt::noFlt;
 inf_flt->pri = -1;
 inf_flt->sta = Bp::E_Sta::noSta;


 QStyleOptionViewItem myOpt = option;
 initStyleOption(&myOpt, index);

 /// Recupere les infos de la table filtre, champ flt
 /// et indique si cette cellule doit etre
 /// traite de facon particuliere
 ///
 b_retVal = chkThatCell(index);

 fltFull(inf_flt,painter,myOpt);

 if(b_retVal){
  /// Gestion des graphiques
  fltDraw(inf_flt,painter,myOpt);
 }

 /// Il faut mettre notre texte
 fltWrite(inf_flt, painter, myOpt);
}

void BFlags::setVisual(const bool isPresent, stTbFiltres *a,QPainter *painter, const QStyleOptionViewItem &option,
                       const QModelIndex &index) const
{
 QStyleOptionViewItem myOpt = option;
 initStyleOption(&myOpt, index);

 /// Gestion des graphiques
 ///fltDraw(isPresent, a, painter, myOpt, index);


 /// Gestion du texte
 fltWrite(a, painter, myOpt);
}

void BFlags::setWanted(bool state, QPainter *painter, const QStyleOptionViewItem &opt, stTbFiltres *a, const QModelIndex &index) const
{
 /// BUG : il faut cliquer pour voir apparaitrela couleur.
 /// QTableView *tmp = static_cast<QTableView *>(opt.styleObject);
 /// Qt::ItemDataRole f;

 /// Gestion des graphiques
 fltDraw(a,painter,opt);

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

bool BFlags::getThisFlt(stTbFiltres *val, const etCount in_typ, const QModelIndex index) const
{
 bool b_retVal = false;

 int cur_col = index.column();
 int cur_row = index.row();

 val->typ = in_typ;
 val->zne = flt.zne;

 if(val->typ >= E_CountToSet && val->typ <= E_CountEnd){
  switch (val->typ) {
   case E_CountElm:
   case E_CountCmb:
   case E_CountBrc:
    if(index.column()==1){
     val->lgn = val->typ *10;
     val->col = index.model()->index(index.row(),0).data().toInt();
     val->val = val->col;
    }
    else {
     return b_retVal;
    }
    break;

	 case E_CountGrp:
		if(((index.column())>0) &&
				(!index.data().isNull()) &&
				(index.data().isValid()))
		{
		 val->lgn = cur_row;
		 val->col = cur_col;
		 if(index.model()->index(val->lgn,val->col).data().canConvert(QMetaType::Int)){
			val->val = index.model()->index(val->lgn,val->col).data().toInt();
		 }
		 else {
			return b_retVal;
		 }
		}
		else {
		 return b_retVal;
		}

		break;
	 case E_CountToSet:
	 case E_CountEnd:
		break;
	}
 }
 else {
  val->typ = E_CountToSet;
 }

 val->zne =flt.zne;
 val->b_flt = flt.b_flt;

 b_retVal = DB_Tools::tbFltGet(val,flt.db_cnx);

 if(b_retVal == false){
  if(val->b_flt != Bp::noFlt){
   b_retVal = true;
  }
 }

 return b_retVal;
}

//void BFlags::cellWrite(QPainter *painter, const QStyleOptionViewItem &myOpt, Qt::GlobalColor inPen, bool up)const
void BFlags::cellWrite(QPainter *painter, QStyle::State state, const QRect curCell, const QString myTxt, Qt::GlobalColor inPen, bool up)const
{
 bool selected = state & QStyle::State_Selected;

 QFont myFnt;
 QPalette myPal;
 Qt::Alignment myAlg;
 Qt::GlobalColor myPen=inPen;

 if (selected)
 {
  /// https://www.qtcentre.org/threads/53498-QFontMetrics-boundingRect()-and-QPainter-draw()-differences
  // Whitee pen while selection
  //painter->setPen(Qt::white);
  painter->setBrush(myPal.highlightedText());
  painter->fillRect(curCell, COULEUR_FOND_FILTRE);
  painter->setPen(selected
                   ? myPal.highlightedText().color()
                   : myPal.text().color());
 }

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
 QRect space = QApplication::style()->itemTextRect(qfm, curCell, alignment, true, myTxt);

 painter->save();


 painter->setFont(myFnt);
 myPal.setColor(QPalette::Active, QPalette::Text, myPen);



 QApplication::style()->drawItemText(painter,space,alignment,myPal,true,myTxt,QPalette::ColorRole::Text);

 painter->restore();
}

void BFlags::fltWrite(stTbFiltres *a, QPainter *painter, const QStyleOptionViewItem &myOpt) const
{

 //QStyleOptionViewItem myOpt = maModif;
 //initStyleOption(&myOpt, index);

 QString myTxt = myOpt.text;
 QRect cur_rect = myOpt.rect;
 QModelIndex index = myOpt.index;
 QStyle::State state =  myOpt.state;

 QFont myFnt;
 QPalette myPal;
 Qt::GlobalColor myPen=Qt::black;
 bool set_up = false;

 int cur_col = index.column();

 int painting_col = Bp::noCol;
 switch (a->typ) {
  case E_CountElm:
  case E_CountCmb:
  case E_CountBrc:
   painting_col = Bp::colTxt;
   break;

	case E_CountGrp:
	 painting_col = Bp::colId;
	 break;

	default:
	 painting_col = Bp::noCol;
	 break;
 }

 if(cur_col==painting_col){

	if((a->typ == E_CountElm)||(a->typ == E_CountGrp)){
	 myTxt = QString::number(myOpt.text.toInt()).rightJustified(2,'0');
	}

	/*
	 *
	 * code corrige dans select sql par printf("%7.2f", avg())
	 * le champ a 7 carateres :
	 * 4 avant la virgule, la virgule puis 2 apres la virgule
	 *
	if(a->typ == E_CountBrc){
	 /// https://stackoverflow.com/questions/7234824/format-a-number-to-a-specific-qstring-format
	 QStringList tmp_lst = myTxt.split(",");
	 if(tmp_lst.size()>1){
		myTxt=tmp_lst[0]+","+tmp_lst[1].leftJustified(2,'0');
	 }
	 else {
		myTxt=myTxt+","+"00";
	 }
	}
 */
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
	if(a->typ == E_CountGrp){
	 myPen = Qt::black;
	}
	else{
	 myPen = Qt::red;
	}
 }

 cellWrite(painter,state,cur_rect, myTxt,myPen,set_up);

}

void BFlags::fltFull(stTbFiltres *a, QPainter *painter, const QStyleOptionViewItem &myOpt) const
{

 if(inf_flt->tb_ref.compare("B_fdj")!=0 ||
     ((inf_flt->typ==E_CountGrp) && (lview->objectName().compare(ViewDetails) != 0))){
  return;
 }


 int col = myOpt.index.column();
 int row = myOpt.index.row();

 if((col == colTotal) || (col == colEc)){
  painter->fillRect(myOpt.rect, COULEUR_FOND_TOTAL);
 }

 int ec= myOpt.index.sibling(row,Bp::colEc).data().toInt();

 if((col > Bp::colEc) && (col <Bp::colTotalv1))
 {
  const QColor tab[]={COULEUR_FOND_R0,COULEUR_FOND_R1,COULEUR_FOND_R2};
  const QColor mul[]={COULEUR_FOND_R3,COULEUR_FOND_R4,COULEUR_FOND_R5};
  QColor color=Qt::white;

  double val = myOpt.index.data().toDouble();

	int r = -1;
	bool multiple = false;
	if((val > 1.0) && (ec>(2*val))){
	 r = static_cast<int>(fmod(ec,val));
	 multiple = true;
	}
	else {
	 r = static_cast<int>(fabs(val-ec));
	}

	if(r>=0 & r<=2){
	 if(multiple == false){
		color = tab[r];
	 }
	 else {
		color = mul[r];
	 }
	}

  painter->fillRect(myOpt.rect, color);
 }

}

void BFlags::fltDraw(stTbFiltres *a, QPainter *painter, const QStyleOptionViewItem &myOpt) const
{

 if(inf_flt->tb_ref.compare("B_fdj")!=0){
  return;
 }
 else if((inf_flt->typ !=E_CountGrp) && (myOpt.index.column() != Bp::colVisual)){
  return;
 }

 QRect cur_rect = myOpt.rect;

 QColor v[]= {
  COULEUR_FOND_AVANTDER, //QColor(255,106,0,255),
  Qt::red,
  Qt::green,
  QColor(255,216,0,255),
  Qt::black,
  QColor(178,0,255,255),
  QColor(211,255,204,255)
 };
 int nbColors = sizeof (v)/sizeof (QColor);

 int refx = cur_rect.topLeft().x();
 int refy = cur_rect.topLeft().y();
 int ctw = cur_rect.width();  /// largeur cellule
 int cth = cur_rect.height(); /// Hauteur cellule
 int cx = ctw/4;
 int cy = cth/2;

 QPoint c_ru(refx +(ctw/5)*4,refy + (cth/6));   /// Center Right up

 /*
 QPoint c_rm(refx +(ctw/5)*4,refy + (cth*3/6)); /// Center Right middle
 QPoint c_rd(refx +(ctw/5)*4,refy + (cth*5/6)); /// Center Right down
 */

 QPoint c_rm(refx +(ctw/5)*1,refy + (cth*3/6)); /// Center Right middle
 QPoint c_rd(refx +(ctw/5)*1,refy + (cth*5/6)); /// Center Right down

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

 if(((a->b_flt & Bp::F_Flt::fltFiltred) == (Bp::F_Flt::fltFiltred))){
  painter->fillRect(cur_rect, COULEUR_FOND_FILTRE);
 }

 /*
 if(a->typ==E_CountElm){

  if(((a->b_flt & Bp::F_Flt::fltFiltred) == (Bp::F_Flt::fltFiltred))){
   painter->fillRect(cur_rect, COULEUR_FOND_FILTRE);
  }

  if(((a->b_flt & Bp::F_Flt::fltFiltred) == (Bp::F_Flt::fltFiltred))){
   painter->fillRect(cur_rect, COULEUR_FOND_FILTRE);
  }
 }
 else {
  if(
   ((a->b_flt & Bp::F_Flt::fltWanted) == (Bp::F_Flt::fltWanted)) && ((a->b_flt & Bp::F_Flt::fltFiltred) == (Bp::F_Flt::fltFiltred))){
   painter->fillRect(cur_rect, COULEUR_FOND_FILTRE);
  }
 }
 */

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
	painter->drawEllipse(c_ru,cx/2,cy/4);
 }

 if((a->b_flt & Bp::F_Flt::fltSeenBfr) == (Bp::F_Flt::fltSeenBfr)){
  painter->setBrush(Qt::green);
  painter->drawEllipse(c_rd,cx/2,cy/4);
 }

 if((a->b_flt & Bp::F_Flt::fltSeenAft) == (Bp::F_Flt::fltSeenAft)){
  painter->setBrush(QColor(0,100,255,255));
  painter->drawEllipse(c_rm,cx/2,cy/4);
 }


 /// ------------------
 painter->restore();
 /// ------------------
}
