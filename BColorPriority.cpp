#include <QColor>

#include "BColorPriority.h"

BColorPriority::BColorPriority(QObject *parent):QSqlQueryModel(parent)
{

}

QVariant BColorPriority::data(const QModelIndex &index, int role) const
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
