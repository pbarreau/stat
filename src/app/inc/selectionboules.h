#ifndef SELECTIONBOULES_H
#define SELECTIONBOULES_H

#include <QDialog>

namespace Ui {
class SelectionBoules;
}

class SelectionBoules : public QDialog
{
    Q_OBJECT
    
public:
    explicit SelectionBoules(QWidget *parent = 0);
    ~SelectionBoules();
    
private:
    Ui::SelectionBoules *ui;
};

#endif // SELECTIONBOULES_H
