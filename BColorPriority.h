#ifndef BCOLORPRIORITY_H
#define BCOLORPRIORITY_H

#include <QSqlQueryModel>

class BColorPriority:public QSqlQueryModel
{
 Q_OBJECT

 public: ///
 explicit BColorPriority(QObject *parent = nullptr);
 QVariant data(const QModelIndex &index, int role) const;
};

#endif // BCOLORPRIORITY_H
