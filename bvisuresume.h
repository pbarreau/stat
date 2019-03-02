#ifndef BVISURESUME_H
#define BVISURESUME_H


#include <QTableView>
#include <QTableWidget>
#include <QTableWidgetItem>

#include <QItemDelegate>
#include <QSqlDatabase>

#include "sqlqmdetails.h"

#define COL_VISU_RESUME 2
#define COL_VISU_COMBO  5

typedef struct _prmBVisuResume{
    QString cnx;
    QString tb_rsm;
    QString tb_tot;
    QString tb_cld; /// Table colors def

} prmBVisuResume;

class BVisuResume:public QStyledItemDelegate
{
    Q_OBJECT
public:
    BVisuResume(prmBVisuResume param, QTableView *parent = 0);
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

typedef struct _stBVisuResume_sql
{
    QString cnx;
    QString sql;
    QString tb_rsm; /// Working on Table
    QString tb_tot; /// Working on Table
    QTableView *view;
    SyntheseGenerale *ori; /// origine
    int *b_min;
    int *b_max;
}stBVisuResume_sql;

class BVisuResume_sql : public QSqlQueryModel
{
    Q_OBJECT
public:
    explicit BVisuResume_sql(stBVisuResume_sql param,QObject *parent = 0);

    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    //int rowCount(const QModelIndex &parent = QModelIndex()) const;
    //int columnCount(const QModelIndex &parent = QModelIndex()) const;

    //bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);




private:
    QSqlDatabase db_0;
    QString tb_tot_src;
    QString tb_rsm_src;
};

class myCombo:public QComboBox{
    Q_OBJECT
public:
    explicit myCombo(QWidget *parent = Q_NULLPTR):QComboBox (parent) {}
    void showPopup();
};
#endif // BVISURESUME_H
