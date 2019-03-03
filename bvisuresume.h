#ifndef BVISURESUME_H
#define BVISURESUME_H


#include <QTableView>
#include <QTableWidget>
#include <QTableWidgetItem>

#include <QItemDelegate>
#include <QSqlDatabase>

#include "sqlqmdetails.h"
#include "bordcolor.h"

#define COL_VISU_RESUME 2
#define COL_VISU_COMBO  5

class BVisuResume:public QStyledItemDelegate
{
    Q_OBJECT
public:
    struct prmBVisuResume{
        QString cnx;
        QString tb_rsm;
        QString tb_tot;
        QString tb_cld; /// Table colors def

    } ;

    explicit BVisuResume(prmBVisuResume param, QTableView *parent = 0);
    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const override;

    QSize sizeHint(const QStyleOptionViewItem &option,
                   const QModelIndex &index) const override;

    /// gestion d'un table view dans un table view
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                          const QModelIndex &index) const override;

    void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    void setModelData(QWidget *editor, QAbstractItemModel *model,
                      const QModelIndex &index) const override;

    void updateEditorGeometry(QWidget *editor,
        const QStyleOptionViewItem &option, const QModelIndex &index) const override;

private:
    bool recupereMapColor(QString tbl_def);

private slots:
    void slot_AideToolTip(const QModelIndex & index);

private:
    QSqlDatabase db_0;
    QString tColorDefs;
    int nb_colors;
    QString tb_rsm_src;
    QString tb_tot_src;
    QTableWidget *a;
    QMap<BOrdColor,int> map_colors;

};


class myCombo:public QComboBox{
    Q_OBJECT

public:
    explicit myCombo(QString cnx, QWidget *parent = Q_NULLPTR):QComboBox (parent)
    {db_0=QSqlDatabase::database(cnx);}
    void showPopup();

  private:
      QSqlDatabase db_0;

};
#endif // BVISURESUME_H
