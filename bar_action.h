#ifndef BAR_ACTION_H
#define BAR_ACTION_H

#include <QObject>
#include <QAction>
#include <QModelIndex>

class bar_action:public QAction //Barreau_ActionFromModelIndex
{
    Q_OBJECT
public:
    bar_action(const QModelIndex &index,const QString &label,QObject * parent =0,...)
        :QAction(label,parent), m_index(index)
    {connect(this, SIGNAL(triggered()), this, SLOT(onTriggered()));}

protected Q_SLOTS:
    void onTriggered()
    {
        emit sig_SelectionTirage(m_index);
    }

Q_SIGNALS:
    void sig_SelectionTirage(const QModelIndex &my_index);

private:
    QModelIndex m_index;
};

#endif // BAR_ACTION_H
