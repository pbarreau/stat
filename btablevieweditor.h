#ifndef BTABLEVIEWEDITOR_H
#define BTABLEVIEWEDITOR_H

#include <QMetaType>
#include <QTableView>
#include <QTableWidget>
#include <QAbstractScrollArea>

class BTableViewEditor
{
  public:
    enum EditMode { Editable, ReadOnly };

    explicit BTableViewEditor(int = 0);
#if 0
    BTableViewEditor(BTableViewEditor const& copie);
    ~BTableViewEditor();
#endif

    void paint(QPainter *painter, const QRect &rect,
               const QPalette &palette, EditMode mode) const;
    QSize sizeHint() const;
    Qt::ItemFlags flags(const QModelIndex &index) const;

  private:
    void remplirTableau();

  protected:
    //QSize QAbstractScrollArea::viewportSizeHint() const;

  private:
    QTableWidgetItem *mesInfos;

};
Q_DECLARE_METATYPE(BTableViewEditor)

#endif // BTABLEVIEWEDITOR_H
