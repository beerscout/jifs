#pragma once

#include <atomic>
#include <QMutex>
#include <QSize>
#include <QThread>
#include <QWaitCondition>

class QImage;

class RenderThread : public QThread
{
    Q_OBJECT

public:
    RenderThread(QObject *parent = 0);
    ~RenderThread();

    void render(QSize resultSize);

signals:
    void renderedImage(const QImage &image);

protected:
    void run() override;

private:

    QMutex mutex_;
    QWaitCondition condition_;
    QSize resultSize_;
    std::atomic<bool> restart_;
    std::atomic<bool> abort_;
};
