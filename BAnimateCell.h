#ifndef BANIMATECELL_H
#define BANIMATECELL_H

#include <QObject>
#include <QStyledItemDelegate>
#include <QMetaType>

#include "BView.h"
#include "ns_upl.h"

/// https://www.titanwolf.org/Network/q/0f38c86c-dda4-4b9b-9afe-dfb7c6c441f2/y
/// https://stackoverflow.com/questions/54035370/how-to-animate-the-color-of-a-qtableview-cell-in-time-once-its-value-gets-updat

#define TIME_RESOLUTION  1000

enum MyDataRole {
 ItemModifiedRole = Qt::UserRole + 1
};

//typedef enum _eCalcul eUpl_Cal;
typedef struct _cellData
{
  QColor color;
  eUpl_Cal e_cal;
  QString upl_txt;
  QVariant timer;
}st_cellData;
Q_DECLARE_METATYPE(st_cellData)

class BAnimateCell: public QStyledItemDelegate
{
  Q_OBJECT

 public:
  explicit  BAnimateCell(BView * view);
  eUpl_Cal addKey(int key);
  eUpl_Cal startKey(int key);
  eUpl_Cal delKey(int key);
  void setCalReady(int key);
  eUpl_Cal setUserSelect(int key);
  QString itemsSelected();
  bool gotKey(int key, eUpl_Cal *curCal = nullptr);
  bool gotKeyReady(int key);
  bool gotKeyUsr(int key);
  bool isShowing(int key);
  int countReady();
  void updateNbColumns();
  void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;

 signals:
  void BSig_Repaint(const BView *tbv);

 private:
  void setKey(int key, QColor color, eUpl_Cal eCal = eCalNotDef);
  void FormalizeCell(int key, QPainter *painter, const QStyleOptionViewItem &myOpt, const QModelIndex &index) const;
  bool gotKeyShowing(int key);

  QVariant data(const QModelIndex &idx, int role = Qt::DisplayRole) const;

 private slots:
  void BSlot_animate(void);
  void BSlot_over(const QModelIndex &index);

 private:
  int nb_col;
  const BView * m_view;
  QMap<int, QVariant > mapTimeout;
  QMap<int, QVariant > mapUserSelect;
  QMap<int, QVariant > mapCal_Ready;
  QMap<int, QVariant > mapShowingKey;
};

#endif // BANIMATECELL_H
