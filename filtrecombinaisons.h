#ifndef FILTRECOMBINAISONS_H
#define FILTRECOMBINAISONS_H

#include <QLineEdit>

Q_DECLARE_METATYPE(QRegExp::PatternSyntax)

class FiltreCombinaisons : public QLineEdit
{
    Q_OBJECT
    Q_PROPERTY(QRegExp::PatternSyntax patternSyntax READ patternSyntax WRITE setPatternSyntax)

public:
    explicit FiltreCombinaisons(QWidget *parent = 0);
    QRegExp::PatternSyntax patternSyntax() const;
    void setPatternSyntax(QRegExp::PatternSyntax);

signals:
    void filterChanged();

public slots:

};

#endif // FILTRECOMBINAISONS_H


