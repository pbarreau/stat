#include <QMessageBox>

#include <QSqlError>
#include <QSqlQuery>

#include <QPainter>

#include "BFlags.h"

BFlags::BFlags(stPrmDlgt prm) : QItemDelegate(prm.parent)
{
 cur_zn = QString::number(prm.zne);
 cur_tp = QString::number(prm.typ);
 col_show = prm.start;
 eTyp = prm.eTyp;

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

 if(((col == col_show) && ((eTyp>eCountToSet) && (eTyp<eCountEnd))) || (col>0 && (eTyp==eCountGrp))){

  int val_cell = index.sibling(index.row(),0).data().toInt();

	QString flt_grp_key="";
	if(col>0 && (eTyp==eCountGrp)){
	 val_cell = 0;
	 if(index.data().canConvert(QMetaType::Int)){
		val_cell = index.data().toInt();
	 }
	 flt_grp_key = " and lgn="+QString::number(index.row()) +
								 " and col="+QString::number(index.column());
	}

	QString msg = "Select pri,flt from Filtres where("
								"zne="+cur_zn+" and " +
								"typ="+QString::number(eTyp)+" and "+
								"val="+QString::number(val_cell)+flt_grp_key+
								")";
	QSqlQuery q(db_1);
	bool isOk=q.exec(msg);

	int val_f = 0;
	int pri_f = 0;
	if(isOk){
	 q.first();
	 if(q.isValid()==false){
		QItemDelegate::paint(painter, option, index);
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
	painter->save();
	painter->setRenderHint(QPainter::Antialiasing, true);

	if(val_f & Filtre::isFiltred){
	 painter->fillRect(option.rect, COULEUR_FOND_FILTRE);
	}

	if( (val_f & Filtre::isLast)){
	 painter->fillRect(r2, COULEUR_FOND_DERNIER);
	}

	if(val_f & Filtre::isPrevious){
	 painter->fillRect(r3, COULEUR_FOND_AVANTDER);
	}


	if(val_f & Filtre::isNever){
	 painter->fillRect(r2, COULEUR_FOND_JAMSORTI);
	}

	/// Mettre les cercles maintenant car les fonds
	/// snt deja dessinee
	if(pri_f > 0){

	 painter->setBrush(v[pri_f]);
	 painter->drawEllipse(c1,cx/2,cy/4);

	}

  painter->restore();
 }

 QItemDelegate::paint(painter, option, index);
}

