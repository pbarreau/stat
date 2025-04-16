#ifndef MONQVIEW_H
#define MONQVIEW_H

#include <QStyledItemDelegate>
#include "SyntheseDetails.h"

typedef struct _pDefPere
{
   SyntheseDetails *pObjet;
   stCurDemande *pParamObj;
   QTabWidget *pOnglet;
   QLineEdit * pDist;

}stObjDetail;

// Test qview dans qView
class MonQtViewDelegate : public QStyledItemDelegate
{
    Q_OBJECT
private:
    stObjDetail *pParent;
    QTabWidget *pereOnglet;
    stCurDemande *pLaConfig;
    QLineEdit * distancePerso;

public:

    MonQtViewDelegate(stObjDetail *pDef, QObject *parent = 0);

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                          const QModelIndex &index) const Q_DECL_OVERRIDE;

    void setEditorData(QWidget *editor, const QModelIndex &index) const Q_DECL_OVERRIDE;
    void setModelData(QWidget *editor, QAbstractItemModel *model,
                      const QModelIndex &index) const Q_DECL_OVERRIDE;

    void updateEditorGeometry(QWidget *editor,
                              const QStyleOptionViewItem &option, const QModelIndex &index) const Q_DECL_OVERRIDE;

    QString SD_Tb2_1(QStringList &boules, int lgn, int dst) const;
    QString SD_Tb2_2(QStringList &boules, int lgn, int dst)const;

};


#endif // MONQVIEW_H
