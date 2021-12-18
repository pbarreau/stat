#ifndef BANIMATECELL_H
#define BANIMATECELL_H

#include <QObject>
#include <QStyledItemDelegate>

#include "BView.h"

/// https://www.titanwolf.org/Network/q/0f38c86c-dda4-4b9b-9afe-dfb7c6c441f2/y
/// https://stackoverflow.com/questions/54035370/how-to-animate-the-color-of-a-qtableview-cell-in-time-once-its-value-gets-updat

#define TIME_RESOLUTION  1000

enum MyDataRole {
 ItemModifiedRole = Qt::UserRole + 1
};

class BAnimateCell: public QStyledItemDelegate
{
  Q_OBJECT

 public:
  explicit  BAnimateCell(BView * view);
  void addKey(int key);
  void delKey(int key);
  bool gotKey(int key);
  void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;

 signals:
  void BSig_Repaint(const BView *tbv);

 private:
  QVariant data(const QModelIndex &idx, int role = Qt::DisplayRole) const;

 private slots:
  void BSlot_animate(void);
  void BSlot_over(const QModelIndex &index);

 private:
  int nb_col;
  //mutable BView * m_view;
  const BView * m_view;
  QMap<int, QVariant > mapTimeout;
};

#endif // BANIMATECELL_H
