#ifndef LABELCLICKABLE_H
#define LABELCLICKABLE_H

#include <QLabel>

class LabelClickable : public QLabel
{
    Q_OBJECT
public:
		explicit LabelClickable(const QString& text ="", QWidget  *parent = 0);
		LabelClickable *getLabel(void);
    ~LabelClickable();

signals:
    void clicked(const QString& text);

protected:
 void mousePressEvent ( QMouseEvent * event ) ;

public slots:

};

#endif // LABELCLICKABLE_H
