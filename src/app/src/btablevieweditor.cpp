#include <QPainter>
#include <QPalette>

#include "btablevieweditor.h"

BTableViewEditor::BTableViewEditor(int value)
{
 mesInfos = new QTableWidgetItem;
}

#if 0
BTableViewEditor::BTableViewEditor(BTableViewEditor const& copie)
{

}

BTableViewEditor::~BTableViewEditor()
{

}
#endif

void BTableViewEditor::remplirTableau(void)
{

}

Qt::ItemFlags BTableViewEditor::flags(const QModelIndex &index) const
{
    if (index.column()==index.model()->columnCount()-2)
        return Qt::ItemIsEditable;

    return Qt::ItemIsEnabled;
}

void BTableViewEditor::paint(QPainter *painter, const QRect &rect,
                       const QPalette &palette, EditMode mode) const
{
    painter->save();

    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->setPen(Qt::NoPen);

    if (mode == Editable) {
        painter->setBrush(palette.highlight());
    } else {
        painter->setBrush(palette.foreground());
    }

    /// Travail a faire
    ///

    painter->restore();
}

QSize BTableViewEditor::sizeHint() const
{
    //return QTableView::viewportSizeHint();
    return mesInfos->sizeHint();
}
