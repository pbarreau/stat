#include "selectionboules.h"
#include "ui_selectionboules.h"

SelectionBoules::SelectionBoules(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SelectionBoules)
{
    ui->setupUi(this);
}

SelectionBoules::~SelectionBoules()
{
    delete ui;
}
