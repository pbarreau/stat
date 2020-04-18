#include <QMessageBox>

#include <QSqlError>
#include <QSqlQuery>

#include <QLineEdit>

#include <QPainter>
#include <QAbstractTextDocumentLayout>
#include <QTextDocument>
#include <QStyledItemDelegate>
#include <QSortFilterProxyModel>
#include <QApplication>
#include <QTableView>

#include "BFlags.h"
//BFlags::BFlags(stPrmDlgt prm) : QItemDelegate(prm.parent)

BFlags::BFlags(stPrmDlgt prm) : QStyledItemDelegate(prm.parent)
{
 cur_zn = QString::number(prm.zne);
 cur_tp = QString::number(prm.typ);
 col_show = prm.start;
 eTyp = prm.eTyp;
 model = prm.mod;
 eflt = prm.b_flt;

 QString cnx=prm.db_cnx;
 db_1 = QSqlDatabase::database(cnx);
 if(db_1.isValid()==false){
  QString str_error = db_1.lastError().text();
  QMessageBox::critical(nullptr, cnx, str_error,QMessageBox::Yes);
  return;
 }

}


QWidget * BFlags::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
 QLineEdit *tmp = new QLineEdit;
 return tmp;
 /*
 if (index.data().canConvert<QLabel>()) {
  QLabel *tmp = new QLabel;
  return tmp;
 }
*/
 //return QStyledItemDelegate::createEditor(parent, option, index);
}

void BFlags::setEditorData(QWidget *editor, const QModelIndex &index) const
{
 QString data = index.data().toString();
 QLineEdit *in = qobject_cast<QLineEdit *>(editor);
 in->setText(data);
 /*
 if (index.data().canConvert<QLineEdit>()) {
  QString data = index.data().toString();
  QLineEdit *in = qobject_cast<QLineEdit *>(editor);
  in->setText(data);
 } else {
  QStyledItemDelegate::setEditorData(editor, index);
 }
*/
}

BFlags_sql::BFlags_sql(etCount eIn, int col,
											 QObject *parent)
		:
			QSqlQueryModel(parent),
			col_show(col),
			eTyp(eIn)

{

}

QVariant	BFlags_sql::data(const QModelIndex &index, int role) const
{

 Qt::ItemDataRole d = static_cast<Qt::ItemDataRole>(role);


 if(index.column()==col_show)
 {

	if((role == Qt::DisplayRole) && (eTyp == eCountElm) )
	{
	 //return "";
	}
 }
 return QSqlQueryModel::data(index,role);

}

void BFlags::paint(QPainter *painter, const QStyleOptionViewItem &option,
                   const QModelIndex &index) const
{
 int col = index.column();

 if((col == col_show) && (eTyp==eCountElm))
 {
  v2_paint(painter,option,index);
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

 QStyleOptionViewItem monOption = option;

 painter->save();

 initStyleOption(&monOption, index);

 item_Wanted(monOption);

#if 0
 monOption.palette.setColor(QPalette::Active, QPalette::Text, Qt::red);
 monOption.displayAlignment = Qt::AlignCenter | Qt::AlignVCenter;

 monOption.text = QString::number(monOption.text.toInt()).rightJustified(2,'0');

 monOption.font.setFamily("ARIAL");
 monOption.font.setPointSize(10);

 monOption.font.setItalic(true);
 //monOption.font.setBold(true);
#endif

 QApplication::style()->drawControl(QStyle::CE_ItemViewItem, &monOption, painter, nullptr);
 //QStyledItemDelegate::paint(painter, monOption, index);
 painter->restore();
}

void BFlags::item_Wanted(QStyleOptionViewItem &opt) const
{
 /// BUG : il faut cliquer pour voir apparaitrela couleur.
 /// QTableView *tmp = static_cast<QTableView *>(opt.styleObject);
 /// Qt::ItemDataRole f;

 opt.palette.setColor(QPalette::Active, QPalette::Text, Qt::red);
 opt.displayAlignment = Qt::AlignTop|Qt::AlignLeft;//Qt::AlignCenter | Qt::AlignVCenter;

 opt.text = QString::number(opt.text.toInt()).rightJustified(2,'0');

 opt.font.setFamily("ARIAL");
 opt.font.setPointSize(7);

 //opt.font.setItalic(true);
 //opt.font.setBold(true);

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

	if((col == col_show) && (eTyp==eCountElm)){
	 painter->setBackground(QBrush(QColor(Qt::white)));
	 painter->fillRect(option.rect, QColor(Qt::white));
	}

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

	if(val_f & Filtre::isWanted){

	 if((col == col_show) && (eTyp==eCountElm)){

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
		BFlags_sql *s= static_cast<BFlags_sql *>(m->sourceModel());
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

