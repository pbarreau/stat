#ifndef TTRANSPARENTPROXYMODEL_H
#define TTRANSPARENTPROXYMODEL_H

/// https://stackoverflow.com/questions/19835618/how-to-make-transparent-proxy-model-qabstractproxymodel
///
#include <QAbstractProxyModel>

class TTransparentProxyModel :
    public QAbstractProxyModel
{
    Q_OBJECT
public:
    TTransparentProxyModel(QObject *parent = 0);

    void setSourceModel(QAbstractItemModel* newSourceModel);

    /* QAbstractProxyModel methods */
    virtual QModelIndex index( int, int c = 0, const QModelIndex& parent = QModelIndex() ) const;
    virtual QModelIndex parent( const QModelIndex &child ) const;
    virtual int rowCount( const QModelIndex &idx = QModelIndex() ) const;
    virtual int columnCount(const QModelIndex &parent ) const;
    virtual QModelIndex mapToSource( const QModelIndex &index ) const;
    virtual QModelIndex mapFromSource( const QModelIndex &idx ) const;
};
#endif // TTRANSPARENTPROXYMODEL_H
