#ifndef MONQVIEW_H
#define MONQVIEW_H

#include <QStyledItemDelegate>

// Test qview dans qView
class MonQtViewDelegate : public QStyledItemDelegate
{
    Q_OBJECT
private:
    QTabWidget *pereOnglet;
    QStringList lesBoules;
    QLineEdit * distancePerso;

public:
    MonQtViewDelegate(QLineEdit *pDist,QStringList &lstChoix, QTabWidget *memo =0, QObject *parent = 0);

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                          const QModelIndex &index) const Q_DECL_OVERRIDE;

    void setEditorData(QWidget *editor, const QModelIndex &index) const Q_DECL_OVERRIDE;
    void setModelData(QWidget *editor, QAbstractItemModel *model,
                      const QModelIndex &index) const Q_DECL_OVERRIDE;

    void updateEditorGeometry(QWidget *editor,
                              const QStyleOptionViewItem &option, const QModelIndex &index) const Q_DECL_OVERRIDE;
};


#endif // MONQVIEW_H