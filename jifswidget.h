#ifndef MANDELBROTWIDGET_H
#define MANDELBROTWIDGET_H

#include <QPixmap>
#include <QWidget>
#include "renderthread.h"

class JifsWidget : public QWidget
{
    Q_OBJECT

public:
    JifsWidget(QWidget *parent = 0);

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
/*    void keyPressEvent(QKeyEvent *event) override;
#ifndef QT_NO_WHEELEVENT
    void wheelEvent(QWheelEvent *event) override;
#endif
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
*/
private slots:
    void updatePixmap(const QImage &image);

private:
    //void scroll(int deltaX, int deltaY);

    RenderThread thread;
    QImage image;
};

#endif // MANDELBROTWIDGET_H
