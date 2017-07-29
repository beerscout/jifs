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

/*void JifsWidget::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_Plus:
        zoom(ZoomInFactor);
        break;
    case Qt::Key_Minus:
        zoom(ZoomOutFactor);
        break;
    case Qt::Key_Left:
        scroll(-ScrollStep, 0);
        break;
    case Qt::Key_Right:
        scroll(+ScrollStep, 0);
        break;
    case Qt::Key_Down:
        scroll(0, -ScrollStep);
        break;
    case Qt::Key_Up:
        scroll(0, +ScrollStep);
        break;
    default:
        QWidget::keyPressEvent(event);
    }
}

#ifndef QT_NO_WHEELEVENT
void JifsWidget::wheelEvent(QWheelEvent *event)
{
    int numDegrees = event->delta() / 8;
    double numSteps = numDegrees / 15.0f;
    zoom(pow(ZoomInFactor, numSteps));
}
#endif

void JifsWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
        lastDragPos = event->pos();
}

void JifsWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton) {
        pixmapOffset += event->pos() - lastDragPos;
        lastDragPos = event->pos();
        update();
    }
}

void JifsWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        pixmapOffset += event->pos() - lastDragPos;
        lastDragPos = QPoint();

        int deltaX = (width() - pixmap.width()) / 2 - pixmapOffset.x();
        int deltaY = (height() - pixmap.height()) / 2 - pixmapOffset.y();
        scroll(deltaX, deltaY);
    }
}*/

void JifsWidget::updatePixmap(const QImage &image)
{
  this->image = image;
  update();
}

/*void JifsWidget::zoom(double zoomFactor)
{
    curScale *= zoomFactor;
    update();
    thread.render(centerX, centerY, curScale, size());
}

void JifsWidget::scroll(int deltaX, int deltaY)
{
    centerX += deltaX * curScale;
    centerY += deltaY * curScale;
    update();
    thread.render(centerX, centerY, curScale, size());
}
*/
