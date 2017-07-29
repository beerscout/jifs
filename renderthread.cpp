#include "renderthread.h"
#include "JIFS.h"

#include <QtWidgets>
#include <cmath>

using namespace ru_xaoc_fractalworld;

RenderThread::RenderThread(QObject *parent)
    : QThread(parent)
    , restart_(false)
    , abort_(false)
{
}

RenderThread::~RenderThread()
{
    mutex_.lock();
    abort_.store(true);
    condition_.wakeOne();
    mutex_.unlock();

    wait();
}

void RenderThread::render(QSize resultSize)
{
    QMutexLocker locker(&mutex_);

    resultSize_ = resultSize;

    if (!isRunning()) {
        start(LowPriority);
    } else {
        restart_.store(true);
        condition_.wakeOne();
    }
}

void RenderThread::run()
{
    forever {
        mutex_.lock();
        QSize resultSize = resultSize_;
        mutex_.unlock();

        QImage image(resultSize, QImage::Format_RGB32);
        
        // Lattice
        std::vector<AffineTransform> ts;
        ts.push_back(AffineTransform(0.3, -0.3, 0.3, 0.3, 1, 1));
        ts.push_back(AffineTransform(0.3, -0.3, 0.3, 0.3, 1, -1));
        ts.push_back(AffineTransform(0.3, -0.3, 0.3, 0.3, -1, 1));
        ts.push_back(AffineTransform(0.3, -0.3, 0.3, 0.3, -1, -1));
        std::vector<double> ps(4, 0.25);
        const IFS ifs(ts, ps);
        const Colorer colorer(50);
        ifs.drawJ(image, colorer, abort_);
        
/*
        // Triangle
        std::vector<AffineTransform> ts;
        ts.push_back(AffineTransform(-0.4, 0, 0, -0.4, 0.24, 0.37));
        ts.push_back(AffineTransform(0.5, 0, 0, 0.5, -1.37, 0.25));
        ts.push_back(AffineTransform(0.21, 0, 0, 0.21, 1, 1.47));
        ts.push_back(AffineTransform(0.5, 0, 0, 0.5, 0.76, -1.16));
        std::vector<double> ps;
        ps.push_back(0.23);
        ps.push_back(0.36);
        ps.push_back(0.06);
        ps.push_back(0.35);
        const IFS ifs(ts, ps);
        const Colorer colorer(50);
        ifs.drawJ(image, colorer);
*/

/*        // Star
        std::vector<AffineTransform> ts;
        ts.push_back(AffineTransform(0.255, 0, 0, 0.255, 0.006, 7.307));
        ts.push_back(AffineTransform(0.255, 0, 0, 0.255, -3.09, 1.931));
        ts.push_back(AffineTransform(0.255, 0, 0, 0.255, 3.102, 1.931));
        ts.push_back(AffineTransform(0.268485, -0.737658, 0.737658, 0.268485, 3.705256, 3.658438));
        std::vector<double> ps;
        ps.push_back(0.080149);
        ps.push_back(0.080149);
        ps.push_back(0.080149);
        ps.push_back(0.759552);
        const IFS ifs(ts, ps);
        const Colorer colorer(50);
        ifs.drawJ(image, colorer, 15, 100); */

        
 /*       std::vector<AffineTransform> ts;
        ts.push_back(AffineTransform(0, 0.5, -0.5, 0, 0, 0.5));
        ts.push_back(AffineTransform(0.5, 0, 0, 0.5, 0.5, 0));
        ts.push_back(AffineTransform(0, -0.5, 0.5, 0, 1.0, 0.5));
        std::vector<double> ps(3, 0.333333);
        const IFS ifs(ts, ps);
        const Colorer colorer(50);
        ifs.drawJ(image, colorer, 16, 4); */
 
        if ( abort_.load() )
          break;

        emit renderedImage(image);

        mutex_.lock();
        if (!restart_.load())
            condition_.wait(&mutex_);
        restart_.store(false);
        mutex_.unlock();
    }
}
