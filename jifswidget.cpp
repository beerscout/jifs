#include <QPainter>
#include <QKeyEvent>

#include <math.h>

#include "jifswidget.h"


JifsWidget::JifsWidget(QWidget *parent)
    : QWidget(parent)
{
    connect(&thread, SIGNAL(renderedImage(QImage)), this, SLOT(updatePixmap(QImage)));

    setWindowTitle(tr("Jifs"));
#ifndef QT_NO_CURSOR
    setCursor(Qt::CrossCursor);
#endif
    resize(550, 400);

}

void JifsWidget::paintEvent(QPaintEvent * /* event */)
{
    QPainter painter(this);
    painter.fillRect(rect(), Qt::black);

    if (image.isNull()) {
        painter.setPen(Qt::white);
        painter.drawText(rect(), Qt::AlignCenter, tr("Rendering initial image, please wait..."));
        return;
    }

    painter.drawImage(0, 0, image);
}

void JifsWidget::resizeEvent(QResizeEvent * /* event */)
{
    thread.render(size());
}


void JifsWidget::updatePixmap(const QImage &image)
{
  this->image = image;
  update();
}

