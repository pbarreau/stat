#include "TTransparentProxyModel.h"

TTransparentProxyModel::TTransparentProxyModel(QObject *parent)
    : QAbstractProxyModel(parent)
{
}

void TTransparentProxyModel::setSourceModel(QAbstractItemModel* newSourceModel)
{
    beginResetModel();

    if (sourceModel()) {
    disconnect(sourceModel(), SIGNAL(rowsAboutToBeInserted(const QModelIndex &, int, int)),
               this, SIGNAL(rowsAboutToBeInserted(const QModelIndex &, int, int)));
    disconnect(sourceModel(), SIGNAL(rowsInserted(const QModelIndex &, int, int)),
               this, SIGNAL(rowsInserted(const QModelIndex &, int, int)));
    disconnect(sourceModel(), SIGNAL(rowsAboutToBeRemoved(const QModelIndex &, int, int)),
               this, SIGNAL(rowsAboutToBeRemoved(const QModelIndex &, int, int)));
    disconnect(sourceModel(), SIGNAL(rowsRemoved(const QModelIndex &, int, int)),
               this, SIGNAL(rowsRemoved(const QModelIndex &, int, int)));
    disconnect(sourceModel(), SIGNAL(rowsAboutToBeMoved(const QModelIndex &, int, int, const QModelIndex &, int)),
               this, SIGNAL(rowsAboutToBeMoved(const QModelIndex &, int, int, const QModelIndex &, int)));
    disconnect(sourceModel(), SIGNAL(rowsMoved(const QModelIndex &, int, int, const QModelIndex &, int)),
               this, SIGNAL(rowsMoved(const QModelIndex &, int, int, const QModelIndex &, int)));
    disconnect(sourceModel(), SIGNAL(columnsAboutToBeInserted(const QModelIndex &, int, int)),
               this, SIGNAL(columnsAboutToBeInserted(const QModelIndex &, int, int)));
    disconnect(sourceModel(), SIGNAL(columnsInserted(const QModelIndex &, int, int)),
               this, SIGNAL(columnsInserted(const QModelIndex &, int, int)));
    disconnect(sourceModel(), SIGNAL(columnsAboutToBeRemoved(const QModelIndex &, int, int)),
               this, SIGNAL(columnsAboutToBeRemoved(const QModelIndex &, int, int)));
    disconnect(sourceModel(), SIGNAL(columnsRemoved(const QModelIndex &, int, int)),
               this, SIGNAL(columnsRemoved(const QModelIndex &, int, int)));
    disconnect(sourceModel(), SIGNAL(columnsAboutToBeMoved(const QModelIndex &, int, int, const QModelIndex &, int)),
               this, SIGNAL(columnsAboutToBeMoved(const QModelIndex &, int, int, const QModelIndex &, int)));
    disconnect(sourceModel(), SIGNAL(columnsMoved(const QModelIndex &, int, int, const QModelIndex &, int)),
               this, SIGNAL(columnsMoved(const QModelIndex &, int, int, const QModelIndex &, int)));
    disconnect(sourceModel(), SIGNAL(modelAboutToBeReset()),
               this, SIGNAL(modelAboutToBeReset()));
    disconnect(sourceModel(), SIGNAL(modelReset()),
               this, SIGNAL(modelReset()));
    disconnect(sourceModel(), SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &)),
               this, SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &)));
    disconnect(sourceModel(), SIGNAL(headerDataChanged(Qt::Orientation,int,int)),
               this, SIGNAL(headerDataChanged(Qt::Orientation,int,int)));
    disconnect(sourceModel(), SIGNAL(layoutAboutToBeChanged()),
               this, SIGNAL(layoutAboutToBeChanged()));
    disconnect(sourceModel(), SIGNAL(layoutChanged()),
               this, SIGNAL(layoutChanged()));
    }

    QAbstractProxyModel::setSourceModel(newSourceModel);

    if (sourceModel()) {
    connect(sourceModel(), SIGNAL(rowsAboutToBeInserted(const QModelIndex &, int, int)),
            this, SIGNAL(rowsAboutToBeInserted(const QModelIndex &, int, int)));
    connect(sourceModel(), SIGNAL(rowsInserted(const QModelIndex &, int, int)),
            this, SIGNAL(rowsInserted(const QModelIndex &, int, int)));
    connect(sourceModel(), SIGNAL(rowsAboutToBeRemoved(const QModelIndex &, int, int)),
            this, SIGNAL(rowsAboutToBeRemoved(const QModelIndex &, int, int)));
    connect(sourceModel(), SIGNAL(rowsRemoved(const QModelIndex &, int, int)),
            this, SIGNAL(rowsRemoved(const QModelIndex &, int, int)));
    connect(sourceModel(), SIGNAL(rowsAboutToBeMoved(const QModelIndex &, int, int, const QModelIndex &, int)),
            this, SIGNAL(rowsAboutToBeMoved(const QModelIndex &, int, int, const QModelIndex &, int)));
    connect(sourceModel(), SIGNAL(rowsMoved(const QModelIndex &, int, int, const QModelIndex &, int)),
            this, SIGNAL(rowsMoved(const QModelIndex &, int, int, const QModelIndex &, int)));
    connect(sourceModel(), SIGNAL(columnsAboutToBeInserted(const QModelIndex &, int, int)),
            this, SIGNAL(columnsAboutToBeInserted(const QModelIndex &, int, int)));
    connect(sourceModel(), SIGNAL(columnsInserted(const QModelIndex &, int, int)),
            this, SIGNAL(columnsInserted(const QModelIndex &, int, int)));
    connect(sourceModel(), SIGNAL(columnsAboutToBeRemoved(const QModelIndex &, int, int)),
            this, SIGNAL(columnsAboutToBeRemoved(const QModelIndex &, int, int)));
    connect(sourceModel(), SIGNAL(columnsRemoved(const QModelIndex &, int, int)),
            this, SIGNAL(columnsRemoved(const QModelIndex &, int, int)));
    connect(sourceModel(), SIGNAL(columnsAboutToBeMoved(const QModelIndex &, int, int, const QModelIndex &, int)),
            this, SIGNAL(columnsAboutToBeMoved(const QModelIndex &, int, int, const QModelIndex &, int)));
    connect(sourceModel(), SIGNAL(columnsMoved(const QModelIndex &, int, int, const QModelIndex &, int)),
            this, SIGNAL(columnsMoved(const QModelIndex &, int, int, const QModelIndex &, int)));
    connect(sourceModel(), SIGNAL(modelAboutToBeReset()),
            this, SIGNAL(modelAboutToBeReset()));
    connect(sourceModel(), SIGNAL(modelReset()),
            this, SIGNAL(modelReset()));
    connect(sourceModel(), SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &)),
            this, SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &)));
    connect(sourceModel(), SIGNAL(headerDataChanged(Qt::Orientation,int,int)),
            this, SIGNAL(headerDataChanged(Qt::Orientation,int,int)));
    connect(sourceModel(), SIGNAL(layoutAboutToBeChanged()),
            this, SIGNAL(layoutAboutToBeChanged()));
    connect(sourceModel(), SIGNAL(layoutChanged()),
            this, SIGNAL(layoutChanged()));
    }
    endResetModel();
}

//virtual int rowCount( const QModelIndex &idx = QModelIndex() ) const;
int TTransparentProxyModel::rowCount(const QModelIndex &parent) const
{
    if(!sourceModel())
    {
    return 0;
    }
    return this->sourceModel()->rowCount(parent);
}

//virtual int columnCount( const QModelIndex &idx ) const;
int TTransparentProxyModel::columnCount(const QModelIndex &parent) const
{
    if(!sourceModel())
    {
    return 0;
    }
    return this->sourceModel()->columnCount(parent);
}

//virtual QModelIndex index( int, int c = 0, const QModelIndex& parent = QModelIndex() ) const;
QModelIndex TTransparentProxyModel::index(int row, int column, const QModelIndex &parent) const
{
    if(!this->sourceModel())
    {
    return QModelIndex();
    }
    return this->sourceModel()->index(row,column,parent);
}

//virtual QModelIndex parent( const QModelIndex &idx ) const;
QModelIndex TTransparentProxyModel::parent(const QModelIndex &child) const
{
//  TODO: check if this is valid.
    QModelIndex mi =  mapFromSource(child);
    if (mi.isValid())
    {
    return mi.parent();
    }
    return QModelIndex();
}

//virtual QModelIndex mapToSource( const QModelIndex &idx ) const;
QModelIndex TTransparentProxyModel::mapToSource(const QModelIndex &index) const
{
    if(!this->sourceModel())
    {
    return QModelIndex();
    }
    return this->sourceModel()->index(index.row(),index.column());
}


//virtual QModelIndex mapFromSource( const QModelIndex &idx ) const;
QModelIndex TTransparentProxyModel::mapFromSource(const QModelIndex &sourceIndex) const
{
    if(sourceIndex.isValid())
    if(!this->sourceModel())
    {
    return QModelIndex();
    }
    return this->sourceModel()->index(sourceIndex.row(),sourceIndex.column());
}
