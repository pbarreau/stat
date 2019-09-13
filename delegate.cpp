#include <QItemDelegate>
#include <QPainter>
#include <QModelIndex>
#include <QStyleOptionViewItem>
#include <QTextDocument>
#include <QAbstractTextDocumentLayout>
#include <QLineF>

#include <QSqlQuery>
#include <QSqlQueryModel>

#include "delegate.h"


//---------------------------------------------------
void BDelegateStepper::paint(QPainter *painter, const QStyleOptionViewItem &option,
                             const QModelIndex &index) const
{
    QStyleOptionViewItem maModif(option);
    //QLineF angleline;
#if 0
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

#endif
		QRect Cellrect = maModif.rect;
		int refx = Cellrect.topLeft().x();
		int refy = Cellrect.topLeft().y();
		int ctw = Cellrect.width();
		int cth = Cellrect.height();
		int cx = ctw/4;
		int cy = cth/2;
		QPoint c1(refx +(ctw/8)*7,refy + (cth/6));
		QPoint c2(refx +(ctw/8)*7,refy + (cth*3/6));
		QPoint c3(refx +(ctw/8)*7,refy + (cth*5/6));

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

    int val = 0;
    int bg = 0;

    QColor u[]= {QColor(201,230,255,255), QColor(255,195,155,255), QColor(170,255,255,255),
                 QColor(224,255,147,255),QColor(231,181,255,255),QColor(73,179,255,255),
                 Qt::red,Qt::white};
    QColor p[]= {Qt::gray,Qt::red,Qt::black,Qt::black};
    QPalette t(u[1]);

    /*
    int cx = maModif.rect.width()/5;
    int cy = maModif.rect.height()/4;
    int refx = maModif.rect.topLeft().x();
    QPoint c1(refx +cx*4,maModif.rect.topLeft().y()+cy);
    QPoint c2(refx +cx*4,maModif.rect.topLeft().y()+2*cy);
    QPoint c3(refx +cx*4,maModif.rect.topLeft().y()+3*cy);
*/
    /// Mettre une couleur en fonction du groupe u,dizaine,v,...
    if(index.model()->index(index.row(),0).data().canConvert(QMetaType::Int))
    {
        val =  index.model()->index(index.row(),0).data().toInt();
        val = (int)floor(val/10);

        if(val < 0 || val >6)
            val = (sizeof(u)/sizeof(QColor))-2;

        // Faut il changer le fond de la case
        if(index.model()->index(index.row(),2).data().canConvert(QMetaType::Int))
        {
            // Ok info de background
            bg = index.model()->index(index.row(),2).data().toInt();
            if(bg !=0)
            {
                val = (sizeof(u)/sizeof(QColor))-1;
            }
        }


        painter->fillRect(option.rect, u[val]);
    }


    // Selection d'une cellule
    if (option.state & QStyle::State_Selected)
    {
        ////painter->fillRect(option.rect, t.highlight());

        /// test
        /*        gradient.setCenter(0, 0);
        gradient.setFocalPoint(3, 3);
        gradient.setColorAt(1, QColor(Qt::red).light(120));
        gradient.setColorAt(0, QColor(Qt::darkYellow).light(120));
*/
        //QRadialGradient gradient(cx, cy, maModif.rect.bottomRight().rx());

        //painter->setBrush(gradient);
        //painter->setPen(QPen(Qt::black, 0));
        painter->save();
        painter->setBrush(Qt::white);
        //painter->drawEllipse(c,cx/2,cy/2);
        painter->restore();
    }

    // Recuperer le chiffre en colonne 1
    // 1 : cette boule sort au tirage suivant
    // 3 : cette boule est sortie au tirage precedent.
    // Pour ecrire pas en gras : QItemDelegate::paint(painter, maModif, index);

    QString lab = "";
    QTextDocument doc;
    QAbstractTextDocumentLayout::PaintContext ctx;
    if(index.model()->index(index.row(),1).data().canConvert(QMetaType::Int))
    {

        int pen = index.model()->index(index.row(),1).data().toInt();

        // speudo gras
        lab = index.model()->data(index,Qt::DisplayRole).toString();
        doc.setHtml(QString("<html><strong>%1</strong></html>").arg(lab));

				// cette boule pas encore sortie
				if(pen == BDelegateStepper::JourBoule::pasImportant)
        {
            // mettre numero boule en gras gris

            painter->save();
            painter->translate(maModif.rect.left(), maModif.rect.top());
            QRect clip(0, 0, maModif.rect.width(), maModif.rect.height());
            painter->setClipRect(clip);

            ctx.palette.setColor(QPalette::Text, Qt::gray);
            ctx.clip = clip;
            doc.documentLayout()->draw(painter, ctx);
            painter->restore();
        }


				// cette boule est sortie hier
				if(pen & BDelegateStepper::JourBoule::hier)
				{
				 /// mettre numero boule en gras gris
				 painter->save();
				 painter->translate(maModif.rect.left(), maModif.rect.top());
				 QRect clip(0, 0, maModif.rect.width(), maModif.rect.height());
				 painter->setClipRect(clip);

				 ctx.palette.setColor(QPalette::Text, Qt::gray);
				 ctx.clip = clip;
				 doc.documentLayout()->draw(painter, ctx);
				 painter->restore();


				 /// montrer un cercle
				 painter->save();
				 painter->setBrush(Qt::yellow);
				 painter->drawEllipse(c3,cx/2,cy/4);
				 painter->restore();

				}

				// cette boule est sortie haujourd'hui
				if(pen & BDelegateStepper::JourBoule::aujourdhui)
				{
				 /// Gras Rouge
				 painter->save();
				 painter->translate(maModif.rect.left(), maModif.rect.top());
				 QRect clip(0, 0, maModif.rect.width(), maModif.rect.height());
				 painter->setClipRect(clip);

				 ctx.palette.setColor(QPalette::Text, Qt::red);
				 ctx.clip = clip;
				 doc.documentLayout()->draw(painter, ctx);
				 painter->restore();

				 /// montrer un cercle
				 painter->save();
				 painter->setBrush(Qt::green);
				 painter->drawEllipse(c2,cx/2,cy/4);
				 painter->restore();

				}

				// cette boule va sortir demain
				if(pen & BDelegateStepper::JourBoule::demain)
				{
				 painter->save();
				 /// Gras Noir
				 painter->translate(maModif.rect.left(), maModif.rect.top());
				 QRect clip(0, 0, maModif.rect.width(), maModif.rect.height());
				 painter->setClipRect(clip);

				 ctx.palette.setColor(QPalette::Text, Qt::black);
				 ctx.clip = clip;
				 doc.documentLayout()->draw(painter, ctx);
				 painter->restore();

				 /// montrer un cercle
				 painter->save();
				 painter->setBrush(Qt::red);
				 painter->drawEllipse(c1,cx/2,cy/4);
				 painter->restore();

				}
#if 0
        // boule sortie entre -1 + n <n< n+1
            // dernier tirage ?
        if (pen & 0x1){
                // oui mettre numero boule en gras rouge
                painter->save();
                painter->translate(maModif.rect.left(), maModif.rect.top());
                QRect clip(0, 0, maModif.rect.width(), maModif.rect.height());
                painter->setClipRect(clip);

                ctx.palette.setColor(QPalette::Text, Qt::red);
                ctx.clip = clip;
                doc.documentLayout()->draw(painter, ctx);
                painter->restore();

                // montrer un cercle
                painter->save();
                painter->setBrush(Qt::green);
                painter->drawEllipse(c,cx/2,cy/2);
                painter->restore();
            }

            // boule va sortir au prochain tirage
            if ((pen & 0x2)||(pen & 0x4)){
                painter->save();
                painter->translate(maModif.rect.left(), maModif.rect.top());
                QRect clip(0, 0, maModif.rect.width(), maModif.rect.height());
                painter->setClipRect(clip);

                // mais est elle actuellement sortie ?
                if(pen & 0x1){
                    //oui
                    ctx.palette.setColor(QPalette::Text, Qt::red);
                }
                else
                {
                    //non
                    ctx.palette.setColor(QPalette::Text, Qt::black);
                }
                ctx.clip = clip;
                doc.documentLayout()->draw(painter, ctx);
                painter->restore();

                // montrer un cercle
                painter->save();
                if((pen &0x1) && (pen & 0x2) &&(pen & 0x4)){
                    painter->setBrush(Qt::black);
                }
                else if((pen & 0x2) &&(pen & 0x4)){
                    painter->setBrush(Qt::magenta);
                }
                else if(pen & 0x2){
                    painter->setBrush(Qt::red);
                }
                else if(pen & 0x4)
                {
                    painter->setBrush(Qt::yellow);
                }
                painter->drawEllipse(c,cx/2,cy/4);
                painter->restore();
            }

            // la boule etait deja sortie

#endif
#if 0
        if (pen >=0 && pen < 4)
        {
            switch(pen)
            {

            case 1:
                //test diagonale
                //angleline.setPoints(maModif.rect.topLeft(), maModif.rect.bottomRight());
                //painter->drawLine(angleline);
                painter->save();
                painter->setBrush(Qt::green);
                painter->drawEllipse(c,cx/2,cy/2);
                painter->restore();


            case 2:
            {

                lab = index.model()->data(index,Qt::DisplayRole).toString();
                doc.setHtml(QString("<html><strong>%1</strong></html>").arg(lab));
                //doc.setTextWidth(maModif.rect.width());

                painter->save();
                painter->translate(maModif.rect.left(), maModif.rect.top());
                QRect clip(0, 0, maModif.rect.width(), maModif.rect.height());
                painter->setClipRect(clip);

                ctx.palette.setColor(QPalette::Text, Qt::black);
                ctx.clip = clip;
                doc.documentLayout()->draw(painter, ctx);
                painter->restore();

                painter->save();
                painter->setBrush(Qt::yellow);
                painter->drawEllipse(c,cx/2,cy/2);
                painter->restore();
            }
                break;

            case 3:{

                //test diagonale
                //angleline.setPoints(maModif.rect.topLeft(), maModif.rect.bottomRight());
                //painter->drawLine(angleline);

                // test mise en gras + couleur jaune
                lab = index.model()->data(index,Qt::DisplayRole).toString();
                doc.setHtml(QString("<html><strong>%1</strong></html>").arg(lab));
                //doc.setTextWidth(maModif.rect.width());

                painter->save();
                painter->translate(maModif.rect.left(), maModif.rect.top());
                QRect clip(0, 0, maModif.rect.width(), maModif.rect.height());
                painter->setClipRect(clip);

                //QAbstractTextDocumentLayout::PaintContext ctx;
                ctx.palette.setColor(QPalette::Text, Qt::yellow);
                ctx.clip = clip;
                doc.documentLayout()->draw(painter, ctx);
                painter->restore();

                // test modif coleur texte
                //maModif.palette.setColor(QPalette::Text,p[pen]);
                //maModif.palette.setColor(QPalette::Text,Qt::yellow);
                //QItemDelegate::paint(painter, maModif, index);

                // test cercle magenta
                painter->save();
                painter->setBrush(Qt::magenta);
                painter->drawEllipse(c,cx/2,cy/2);
                painter->restore();
            }
                break;

            default:
                //test diagonale
                angleline.setPoints(maModif.rect.topRight(), maModif.rect.bottomLeft());
                //angleline.setLine();
                painter->drawLine(angleline);

                maModif.palette.setColor(QPalette::Text,Qt::magenta);
                QItemDelegate::paint(painter, maModif, index);

                painter->save();
                painter->setBrush(Qt::blue);
                painter->drawEllipse(c,cx/2,cy/2);
                painter->restore();

                break;
            }
        }
#endif
    }

    //maModif.text = "";
    //maModif.widget->style()->drawControl(QStyle::CE_ItemViewItem, &maModif, painter);

    //painter->translate(maModif.rect.left(), maModif.rect.top());
    //QRect clip(0, 0, maModif.rect.width(), maModif.rect.height());
    //doc.drawContents(painter, clip);
    //painter->restore();
    //QColor ItemForegroundColor = index.data(Qt::ForegroundRole).value<QColor>();

    //QItemDelegate::paint(painter, maModif, index);
}

BDelegateElmOrCmb::BDelegateElmOrCmb(stPrmDlgt prm) : QItemDelegate(prm.parent)
{
 cur_zn = QString::number(prm.zne);
 cur_tp = QString::number(prm.typ);
 dbToUse = QSqlDatabase::database(prm.db_cnx);
}

void BDelegateElmOrCmb::paint(QPainter *painter, const QStyleOptionViewItem &option,
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


 if(((col == 0) && (cur_tp.toInt()<3)) || (col>0 && (cur_tp.toInt()==3))){

	QString msg = "Select pri,flt from Filtres where("
								"zne="+cur_zn+" and " +
								"typ="+cur_tp+" and "+
								"lgn="+QString::number(index.row())+" and "+
								"col="+QString::number(index.column())+
								")";
	QSqlQuery q(dbToUse);
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

	if(val_f & Filtre::isWanted){
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

#if 0
void BDelegateElmOrCmb::paint(QPainter *painter, const QStyleOptionViewItem &option,
                              const QModelIndex &index) const
{
    int col = index.column();
    int pri = -1;
    int nbCol = index.model()->columnCount();
    int val = 0;
    QStyleOptionViewItem maModif(option);
    int cx = maModif.rect.width()/4;
    int cy = maModif.rect.height()/2;
    int refx = maModif.rect.topLeft().x();
    QPoint c(refx +cx*3,maModif.rect.topLeft().y()+cy);

    QColor u[]= {QColor(201,230,255,255),QColor(200,170,100,140)};
    QColor v[]= {
        Qt::black,
        Qt::red,
        Qt::green,
        QColor(255,216,0,255),
        QColor(255,106,0,255),
        QColor(178,0,255,255),
        QColor(211,255,204,255)
    };


    /// Elle indique que mettre comme couleur
    QString msg = "Select pri,flt from Filtres where("
                  "zne="+cur_zn+" and " +
                  "typ="+cur_tp+" and "+
                  "lgn="+QString::number(index.row())+" and "+
                  "col="+QString::number(index.column())+
                  ")";
    QSqlQuery q(dbToUse);
    bool isOk=q.exec(msg);

		if(isOk){
		 q.first();
		 /// Info priorite
		 if(q.value(0).canConvert(QMetaType::Int)){
			pri = q.value(0).toInt();
			if(pri<0){pri=0;}
		 }

		 /// Info filtre
		 if(q.value(1).canConvert(QMetaType::Int)){
			val = q.value(1).toInt();
			if(val<0){val=0;}
		 }
		}

#if 0
    /// Regarder la valeur de l'avant derniere colonne pour activer info priorite
    /// Elle indique que mettre comme couleur
    if(index.model()->index(index.row(),nbCol-2).data().canConvert(QMetaType::Int))
    {
        pri =  index.model()->index(index.row(),nbCol-2).data().toInt();
    }
#endif

    switch(col)
    {
    case 0:
    {
        /// Mettre un cercle colore
        /// Filtre active sur le cas
        if (val & 0x2)
        {
         painter->fillRect(option.rect, u[0]);
        }

        if (pri > 0){
            painter->save();
            painter->setBrush(v[pri]);
            painter->drawEllipse(c,cx/2,cy/2);
            painter->restore();
        }

    }
        break;
    case 1:
    {
        /// Marquer le dernier tirage
        if (val & 0x1)
        {
            painter->fillRect(option.rect, u[1]);
        }

        /// Marquer tirages non sorti
        if (val & 0x4)
        {
            painter->fillRect(option.rect, v[2]);
        }
    }
        break;
    default:
        break;
    }


    QItemDelegate::paint(painter, maModif, index);
}
#endif


void BDelegateFilterGrp::paint(QPainter *painter, const QStyleOptionViewItem &option,
                               const QModelIndex &index) const
{
    QStyleOptionViewItem maModif(option);
#if 0
    QColor u[]= {QColor(201,230,255,255),QColor(200,170,100,140)};
    int col = index.column();
    int row = index.row();
    int nbCol = index.model()->columnCount();
    int val = 0;


    /// Regarder la valeur de la derniere colonne
    /// Elle indique que mettre comme couleur
    if(index.model()->index(index.row(),nbCol-1).data().canConvert(QMetaType::Int))
    {
        val =  index.model()->index(index.row(),nbCol-1).data().toInt();
    }


    if(col>0 && col<(nbCol-1)){
        /// bit actif
        if(val & (1<<col-1)){
            painter->fillRect(option.rect, u[0]);
        }
    }
#endif
    QItemDelegate::paint(painter, maModif, index);
}

BSqmColorizePriority::BSqmColorizePriority(QObject *parent):QSqlQueryModel(parent)
{

}

QVariant BSqmColorizePriority::data(const QModelIndex &index, int role) const
{

    QColor u[]= {
        Qt::black,
        Qt::red,
        Qt::green,
        QColor(255,216,0,255),
        QColor(255,106,0,255),
        QColor(178,0,255,255),
        QColor(211,255,204,255)
    };

    if(index.column()== 0 )
    {
        int nbCol=index.model()->columnCount();

        /// recuperation de l'info donnant la couleur
        QModelIndex priority = index.sibling(index.row(),nbCol-2);


        /// Choix de la couleur a appliquer
        if(priority.data().canConvert(QMetaType::Int)){
            int val = priority.data().toInt();
            if (role == Qt::TextColorRole){
                if(val) val = 1; // On garde une seule couleur
                return (u[val]);
            }
        }
    }

    return QSqlQueryModel::data(index,role);
}

