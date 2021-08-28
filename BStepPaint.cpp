#include <QApplication>

#include "BStepPaint.h"
#include "colors.h"

BStepPaint::BStepPaint(const stGameConf *pGame, int zone, Bp::ETbvId tbvId, int *cur, int *prev)
{
 pGDef =pGame;
 zn = zone;
 lenTab = pGame->limites[zone].len;
 eTbv = tbvId;
 dig_ball = -1;
 curTir = cur;
 prvTir = prev;
}

void BStepPaint::paint(QPainter *painter, const QStyleOptionViewItem &option,
                       const QModelIndex &index) const
{
 Q_ASSERT(index.isValid());

 int my_col = index.column();
 int my_row = index.row();

 QStyleOptionViewItem myOpt = option;
 initStyleOption(&myOpt, index);

 if(my_col==1 && eTbv==Bp::tbvLeft){
  paintDraw(painter,myOpt);
 }

 if(eTbv==Bp::TbvRight){
  paintDraw(painter,myOpt);
 }

 paintWrite(painter,myOpt);
}


void BStepPaint::paintDraw(QPainter *painter, const QStyleOptionViewItem &myOpt) const
{
 QModelIndex index = myOpt.index;
 QRect cur_rect = myOpt.rect;

 int my_col = index.column();
 int my_row = index.row();

 QColor v[]= {
  COULEUR_FOND_AVANTDER,
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

 int cell_val = -1;
 if(index.data().canConvert(QMetaType::Int)){
  cell_val = index.data().toInt();
 }

 /// ------------------
 painter->save();
 /// ------------------
 painter->setRenderHint(QPainter::Antialiasing, true);

 ///CODE GESTION ICI
 if(dig_ball>0){
  if(cell_val==dig_ball){
   /// Montrer la boule
   painter->fillRect(cur_rect, COULEUR_FOND_FILTRE);
  }
 }

 if(cell_val > 0){
  /// Recherche dans les tableaux des boules

  /// --- Dessin des rectangles
  for(int i = 0 ; i < lenTab; i++){
   if(cell_val==prvTir[i]){
    /// mettre rectangle avant dernier
    painter->fillRect(r3, COULEUR_FOND_AVANTDER);
    break;
   }
  }

  for(int i = 0 ; i < lenTab; i++){
   if(cell_val==curTir[i]){
    /// mettre rectangle dernier
    painter->fillRect(r2, COULEUR_FOND_DERNIER);
    break;
   }
  }

  /// --- Dessin des cercles
  for(int i = 0 ; i < lenTab; i++){
   if(cell_val==curTir[i]){
    /// Rechercher dans les precedents si cette celle est a +- 1
    for(int j = 0 ; j < lenTab; j++){
     if((cell_val-1)==prvTir[j]){
      /// boule -1
      painter->setBrush(Qt::green);
      painter->drawEllipse(c_rd,cx/2,cy/4);
     }

     if((cell_val+1)==prvTir[j]){
      /// boule +1
      painter->setBrush(QColor(0,100,255,255));
      painter->drawEllipse(c_rm,cx/2,cy/4);
     }
    }
   }
  }

 }
 /// ------------------
 painter->restore();
 /// ------------------
}

void BStepPaint::paintWrite(QPainter *painter, const QStyleOptionViewItem &myOpt)const
{
 QString myTxt = myOpt.text;
 QRect cur_rect = myOpt.rect;
 QModelIndex index = myOpt.index;
 QStyle::State state =  myOpt.state;

 QFont myFnt;
 QPalette myPal;
 Qt::GlobalColor myPen=Qt::black;
 bool set_up = false;

 int my_col = index.column();
 int my_row = index.row();

 if(myTxt.size()){
  myTxt = QString::number(myOpt.text.toInt()).rightJustified(2,'0');
 }

 cellWrite(painter,state,cur_rect, myTxt,myPen,set_up);
}

void BStepPaint::cellWrite(QPainter *painter, QStyle::State state, const QRect curCell, const QString myTxt, Qt::GlobalColor inPen, bool up)const
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

void BStepPaint::BSlot_FindBall(BView *tbvTarget, int id)
{
 dig_ball = id;
 tbvTarget->reset();
}
