#include <QPainter>
#include <QStyleOptionViewItem>
#include <QModelIndex>
#include <QColor>

#include "BTirDelegate.h"

BTirDelegate::BTirDelegate(const stGameConf *pGame, const Bp::E_Col start_zn, QWidget *parent)
		:
			QStyledItemDelegate(parent), cur_game(pGame), zs(start_zn)
{

}

/// https://stackoverflow.com/questions/9483567/qpainterdrawline-and-qpainterdrawtext-with-different-color-issue-in-qt
void BTirDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
                         const QModelIndex &index) const
{

 QColor u[]= {QColor(201,230,255,255), QColor(255,195,155,255), QColor(170,255,255,255),
               QColor(224,255,147,255),QColor(231,181,255,255),QColor(73,179,255,255),
               Qt::red,Qt::white};

 QStyleOptionViewItem maModif(option);
 initStyleOption(&maModif, index);

 QRect curR = maModif.rect;
 /// Mettre une couleur en fonction du groupe u,dizaine,v,...
 int col_id = index.column();
 int col_zn_0 = zs;
 int end_zn_0 = col_zn_0 +cur_game->limites[0].len;
 if((col_id>=col_zn_0) &&(col_id < end_zn_0))
 {
  int val = index.data().toInt();
  val = val/10;

	if(val < 0 || val >6){
	 val = (sizeof(u)/sizeof(QColor))-2;
	}

	painter->fillRect(maModif.rect, u[val]);
	painter->drawText(maModif.rect,Qt::AlignCenter,maModif.text);

	/// Tracer une ligne si checked
	int col_chk = end_zn_0;
	//QModelIndex ichk = index.model()->index(index.row(),col_chk,QModelIndex());
	if(index.sibling(index.row(),col_chk).data(Qt::CheckStateRole) == Qt::Checked){
#ifdef DBG_LINE
	 int row = index.row();
	 QString txt = maModif.text;
	 int x;/// = maModif.rect.topLeft().x();
	 int y;/// = maModif.rect.bottomRight().x();
	 int w;/// = maModif.rect.height();
	 int h;
	 maModif.rect.getRect(&x,&w,&y,&h);
	 QLine angleline(x,h/2,x+w,h/2);
	 QLine tst[1] = {angleline};
	 painter->drawLines(tst,1);
#else
	 QLine angleline;
#endif
	 painter->save();
	 painter->setPen(Qt::red);
	 angleline.setPoints(maModif.rect.topLeft(), maModif.rect.bottomRight());
	 painter->drawLine(angleline);
	 angleline.setPoints(maModif.rect.topRight(), maModif.rect.bottomLeft());
	 painter->drawLine(angleline);
	 painter->restore();
	}
	return;
 }

 if(cur_game->znCount>1){
  int col_zn_1 = end_zn_0;
  int end_zn_1 = col_zn_1 +cur_game->limites[1].len;
  if((col_id >=col_zn_1) && (col_id <end_zn_1)){
   /// mettre le texte en rouge
   painter->save();
   painter->setPen(Qt::red);
   painter->drawText(maModif.rect,Qt::AlignCenter,maModif.text);
   painter->restore();
   return;
  }
 }

 QStyledItemDelegate::paint(painter, maModif, index);
}
