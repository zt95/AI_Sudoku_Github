#ifndef PAINTLINE_H
#define PAINTLINE_H

#include <QWidget>
#include <QPainter>

class paintLine : public QWidget
{
public:
    paintLine(QWidget *parent = 0);
    void setColor(const QColor& col);
    void setLineSizeDir(int lw, int len, int dir);

protected:
    void paintEvent(QPaintEvent *);

private:
    int lineWidth;
    int lineLength;
    int lineDirection;
    QColor color;
};

#endif // PAINTLINE_H
