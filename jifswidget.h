#pragma once

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

private slots:
    void updatePixmap(const QImage &image);

private:
    //void scroll(int deltaX, int deltaY);

    RenderThread thread;
    QImage image;
};
