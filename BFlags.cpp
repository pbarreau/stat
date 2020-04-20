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

BFlags::BFlags(stPrmDlgt prm) : QStyledItemDelegate(prm.parent)
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
 int col = index.column();

 if((col == flt.start) && (flt.typ==eCountElm))
 {
  v3_paint(painter,option,index);
 }
 else {
  QStyledItemDelegate::paint(painter, option, index);
 }

 //v1_paint(painter,option,index);
}

void BFlags::v2_paint(QPainter *painter, const QStyleOptionViewItem &option,
                      const QModelIndex &index) const
{
 /// https://openclassrooms.com/forum/sujet/qt-qtableview-qstyleditemdelegate
 /// https://forum.qt.io/topic/41463/solved-qstyleditemdelegate-set-text-color-when-row-is-selected
 /// https://openclassrooms.com/forum/sujet/recuperer-les-ligne-selectionnees-dans-un-qitemdelegate-35100
 /// https://stackoverflow.com/questions/34729858/override-text-in-qstyleditemdelegate-for-qtreeview
 ///
 /// https://code.qt.io/cgit/qt/qt.git/tree/src/gui/styles/qstyle.cpp?h=4.5
 /// ligne 489 , QStyle::drawItemText


 stTbFiltres a;
 a.tbName = "Filtres";
 a.zne = flt.zne;
 a.typ = flt.typ;
 a.lgn = -1;
 a.col = -1;
 a.val = -1;
 a.pri = -1;
 a.flt = Bp::Filtering::isNotSet;


 if(getdbFlt(&a, flt.typ, index)){
  setWanted(true, painter,option, &a, index);
 }
 else {
  setWanted(false, painter,option, &a, index);
 }
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
 a.zne = flt.zne;
 a.typ = flt.typ;
 a.lgn = -1;
 a.col = -1;
 a.val = -1;
 a.pri = -1;
 a.flt = Bp::Filtering::isNotSet;

 bool ret = getdbFlt(&a, flt.typ, index);
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
 /*
 QStyle::State val_state = myOpt.state;
 */

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
}

bool BFlags::getdbFlt(stTbFiltres *ret, const etCount in_typ, const QModelIndex index) const
{
 bool isOk = false;
 etCount typ = in_typ;

 int zn  = flt.zne;
 int lgn = -1;
 int col = -1;
 int val = -1;
 Bp::Filterings my_flt = Bp::Filtering::isNotSet;


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
  my_flt = static_cast<Bp::Filterings>(valeur);

  (*ret).pri = priori;
  (*ret).b_flt = my_flt;
  (*ret).flt = query_2.value("flt").toInt();

 }

 (*ret).isPresent = isOk;
 (*ret).zne = zn;
 (*ret).typ = typ;
 (*ret).lgn = lgn;
 (*ret).col = col;
 (*ret).val = val;

 return isOk;
}

void BFlags::fltWrite(bool isPresent, stTbFiltres *a, QPainter *painter, const QStyleOptionViewItem &maModif,
                     const QModelIndex &index) const
{
 QStyleOptionViewItem myOpt = maModif;
 initStyleOption(&myOpt, index);

 QFont myFnt;
 QPalette myPal;
 Qt::Alignment myAlg;
 Qt::GlobalColor myPen;
 QRect curCell = myOpt.rect;

 int alignment = 0;
 int size = 0;
 bool b_italic = false;

 myFnt.setFamily("ARIAL");
 myFnt.setWeight(QFont::Normal);

 if((a->b_flt & Bp::Filtering::isWanted) == Bp::Filtering::isWanted){
  myPen = Qt::green;
  size= 7;
  myAlg = Qt::AlignTop|Qt::AlignLeft;
  b_italic = true;
 }
 else {
  myPen = Qt::black;
  size = 10;
  myAlg = Qt::AlignCenter | Qt::AlignVCenter;
  b_italic = false;
 }

 if ((a->b_flt & Bp::Filtering::isFiltred) == Bp::Filtering::isFiltred) {
  myPen = Qt::red;
 }

 myFnt.setPointSize(size);
 myFnt.setItalic(b_italic);
 alignment = static_cast<int>(myAlg);

 QString myTxt = QString::number(myOpt.text.toInt()).rightJustified(2,'0');

 /// Calcul de l'epace pour le texte
 QFontMetrics qfm(myFnt);
 QRect space = QApplication::style()->itemTextRect(qfm, curCell, alignment, false, myTxt);

 painter->save();

 painter->setFont(myFnt);
 myPal.setColor(QPalette::Active, QPalette::Text, myPen);
 QApplication::style()->drawItemText(painter,space,alignment,myPal,true,myTxt,QPalette::ColorRole::Text);

 painter->restore();
}

void BFlags::fltDraw(bool isPresent, stTbFiltres *a, QPainter *painter, const QStyleOptionViewItem &maModif,
                const QModelIndex &index) const
{
 int col = index.column();

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
//#if 0

 if(((a->pri)>0) && ((a->b_flt & Bp::Filtering::isFiltred) == (Bp::Filtering::isFiltred))){
  painter->fillRect(maModif.rect, COULEUR_FOND_FILTRE);
 }

 if( (a->b_flt & Bp::Filtering::isLastTir) == (Bp::Filtering::isLastTir)){
  painter->fillRect(r2, COULEUR_FOND_DERNIER);
 }

 if((a->b_flt & Bp::Filtering::isPrevTir) == (Bp::Filtering::isPrevTir)){
  painter->fillRect(r3, COULEUR_FOND_AVANTDER);
 }


 if((a->b_flt & Bp::Filtering::isNotSeen) == (Bp::Filtering::isNotSeen)){
  painter->fillRect(r2, COULEUR_FOND_JAMSORTI);
 }
//#endif
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

void BFlags::v1_paint(QPainter *painter, const QStyleOptionViewItem &option,
                      const QModelIndex &index) const
{
 QStyleOptionViewItem maModif(option);

 int col = index.column();

 QColor v[]= {
  Qt::black,
  Qt::red,
  Qt::green,
  QColor(255,216,0,255),
  QColor(255,106,0,255),
  QColor(178,0,255,255),
  QColor(211,255,204,255)
 };

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

 QPolygon triangle;
 QPoint t1(refx,refy);
 QPoint t2(refx+ctw,refy);
 QPoint t3(refx,refy+cy);
 triangle << t1<<t2<<t3<<t1;

 if(((col == flt.start) && ((flt.typ>eCountToSet) && (flt.typ<eCountEnd))) || (col>0 && (flt.typ==eCountGrp))){


	int val_cell = index.sibling(index.row(),0).data().toInt();

	QString flt_grp_key="";
	if(col>0 && (flt.typ==eCountGrp)){
	 val_cell = 0;
	 if(index.data().canConvert(QMetaType::Int)){
		val_cell = index.data().toInt();
	 }
	 flt_grp_key = " and lgn="+QString::number(index.row()) +
								 " and col="+QString::number(index.column());
	}

	QString msg = "Select pri,flt from Filtres where("
								"zne="+QString::number(flt.zne)+" and " +
								"typ="+QString::number(flt.typ)+" and "+
								"val="+QString::number(val_cell)+flt_grp_key+
								")";
	QSqlQuery q(db_1);
	bool isOk=q.exec(msg);

	int val_f = 0;
	int pri_f = 0;
	if(isOk){
	 q.first();
	 if(q.isValid()==false){
		//QItemDelegate::paint(painter, option, index);
		QStyledItemDelegate::paint(painter, option, index);
		return;
	 }
	 /// Info priorite
	 if(q.value(0).canConvert(QMetaType::Int)){
		pri_f = q.value(0).toInt();
		if(pri_f<0){pri_f=0;}
	 }

	 /// Info filtre
	 if(q.value(1).canConvert(QMetaType::Int)){
		val_f = q.value(1).toInt();
		if(val_f<0){val_f=0;}
	 }
	}

	//----------------
	//QAbstractItemView::

	painter->save();

	painter->setCompositionMode(QPainter::CompositionMode::CompositionMode_SourceOver);

	if((col == flt.start) && (flt.typ==eCountElm)){
	 painter->setBackground(QBrush(QColor(Qt::white)));
	 painter->fillRect(option.rect, QColor(Qt::white));
	}

	painter->setRenderHint(QPainter::Antialiasing, true);

	if(val_f & Filtre::isFiltred){
	 painter->fillRect(option.rect, COULEUR_FOND_FILTRE);
	}

	if( (val_f & Filtre::isLastTir)){
	 painter->fillRect(r2, COULEUR_FOND_DERNIER);
	}

	if(val_f & Filtre::isPrevTir){
	 painter->fillRect(r3, COULEUR_FOND_AVANTDER);
	}


	if(val_f & Filtre::isNotSeen){
	 painter->fillRect(r2, COULEUR_FOND_JAMSORTI);
	}

	/// Mettre les cercles maintenant car les fonds
	/// snt deja dessinee
	if(pri_f > 0){

	 painter->setBrush(v[pri_f]);
	 painter->drawEllipse(c1,cx/2,cy/4);

	}

	if(val_f & Filtre::isWanted){

	 if((col == flt.start) && (flt.typ==eCountElm)){

		painter->setBrush(Qt::BrushStyle::SolidPattern);
		painter->fillRect(option.rect, QColor(Qt::white));

		static int toto = 0;
		QStyle::State value = option.state;

		//QString text = "0"+ option.text;
		//text = 		QString::number(toto).rightJustified(2,'0');
		toto++;
		int aa_cel = index.sibling(index.row(),0).data().toInt();
		int aa_col = index.column();
		int aa_row = index.row();

		const QSortFilterProxyModel *m= static_cast<const QSortFilterProxyModel *>(index.model());
		QSqlQueryModel *s= static_cast<QSqlQueryModel *>(m->sourceModel());
		int aa_len = s->columnCount();


		QModelIndex aa_index = s->index(index.row(), index.column());
		aa_cel = aa_index.sibling(index.row()-1,0).data().toInt();
		//QString aa_text = QString::number(aa_cel).rightJustified(2,'0');
		//painter->setPen(QPen(Qt::green));
		//QString text = "0"+ option.text;AlignCenter
		//painter->drawText(option.rect, Qt::AlignCenter | Qt::AlignVCenter, text );

		QStyleOptionViewItem unStyleOption = option;
		//unStyleOption.text = aa_text;

		unStyleOption.palette.setColor(QPalette::HighlightedText, Qt::green);

		unStyleOption.font.setFamily("ARIAL");
		unStyleOption.font.setPointSize(14);
		//painter->setFont(QFont("ARIAL",12,1));

		unStyleOption.font.setItalic(true);
		//unStyleOption.font.setBold(true);

		//painter->setPen(QPen(Qt::red));
		unStyleOption.palette.setColor(QPalette::Active, QPalette::Text, Qt::red);

		//painter->setBrush(Qt::BrushStyle::SolidPattern);
		//unStyleOption.palette.setBrush(QPalette::Text, Qt::red);

		//painter->drawText(option.rect, Qt::AlignCenter | Qt::AlignVCenter, aa_text );

		QStyledItemDelegate::paint(painter, unStyleOption, index);

	 }
	}

  painter->restore();
 }

#if 0
 if((col == col_show) && (eTyp==eCountElm)){
  painter->save();
  if(option.text.compare("1")){
   QString v0 = option.text;
   int v1 = option.type;
   QFont v2 = option.font;
   QModelIndex v3 = option.index;
   QLocale v4 = option.locale;
   //ViewItemFeatures v5 = option.features;
   QStyle::State v5 = option.state;
   int v6 = option.version;
   QObject *v7 = option.styleObject;
   QStyleOptionViewItem s = option;
   QString t = s.text;
   painter->setPen(QPen(Qt::red));
   s.palette.setColor(QPalette::Active, QPalette::Text, Qt::red);
   s.font.setBold(true);
   QStyledItemDelegate::paint(painter, s, index);
  }
  painter->restore();
 }
#endif

 //QItemDelegate::paint(painter, option, index);
 QStyledItemDelegate::paint(painter, option, index);

 }

