#include "filtrecombinaisons.h"

FiltreCombinaisons::FiltreCombinaisons(QWidget *parent) :
    QLineEdit(parent)
{
    //connect(this, SIGNAL(textChanged(QString)), this, SIGNAL(filterChanged()));
}

static inline QRegExp::PatternSyntax patternSyntaxFromAction(const QAction *a)
{
    //return static_cast<QRegExp::PatternSyntax>(a->data().toInt());
}

QRegExp::PatternSyntax FiltreCombinaisons::patternSyntax() const
{
    //return patternSyntaxFromAction(m_patternGroup->checkedAction());
}

void FiltreCombinaisons::setPatternSyntax(QRegExp::PatternSyntax s)
{
}
