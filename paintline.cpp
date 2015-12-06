#include "paintline.h"

paintLine::paintLine(QWidget *parent) : QWidget(parent)
{

}

void paintLine::setColor(const QColor& col)
{
    this->color = col;
}

void paintLine::setLineSizeDir(int lw, int len, int dir)
{
    this->lineWidth = lw;
    this->lineDirection = dir;
    this->lineLength = len;
    if (dir == 0) {
        setMinimumSize(len, lw);
        setMaximumSize(len, lw);
    }
    else {
        setMinimumSize(lw, len);
        setMaximumSize(lw, len);
    }
    this->update();
}

void paintLine::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    QPen pen;
    pen.setWidth(this->lineWidth);
    pen.setColor(this->color);
    painter.setPen(pen);
    int x1, x2, y1, y2;
    if (this->lineDirection == 0) {
        x1 = 0;
        y1 = this->lineWidth/2;
        x2 = this->lineLength;
        y2 = y1;
    }
    else {
        x1 = this->lineWidth/2;
        y1 = 0;
        x2 = x1;
        y2 = this->lineLength;
    }
    painter.drawLine(x1, y1, x2, y2);
}
