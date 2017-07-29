// Matvej Kotov, 2014
// http://fractalworld.xaoc.ru/

#include "JIFS.h"

namespace ru_xaoc_fractalworld
{


Points::Points(const std::vector<std::pair<Point, size_t> > &points)
    : points_(points)
{ }


AffineTransform Points::calcStretchTransform(
        const size_t width, const size_t height, const size_t border) const
{
    double minX = DBL_MAX;
    double maxX = -DBL_MAX;
    double minY = DBL_MAX;
    double maxY = -DBL_MAX;
    for (size_t i = 0; i < points_.size(); ++i) {
        const double x = points_[i].first.x();
        const double y = points_[i].first.y();
        if (x < minX) {
            minX = x;
        }
        if (x > maxX) {
            maxX = x;
        }
        if (y < minY) {
            minY = y;
        }
        if (y > maxY) {
            maxY = y;
        }
    }
    const double scale = std::min((width - 2*border)/ (maxX - minX), (height - 2*border) / (maxY - minY));
    const double deltaX = width / 2 - (minX + maxX) * scale / 2;
    const double deltaY = height / 2 + (minY + maxY) * scale / 2;
    return AffineTransform(scale, 0, 0, -scale, deltaX, deltaY);
}


void Points::draw(QImage &canvas, const AffineTransform &stretchTransform) const
{
    for (size_t i = 0; i < points_.size(); ++i) {
        Point z = stretchTransform.apply(points_[i].first);
        QRgb *line = reinterpret_cast<QRgb *>(canvas.scanLine(int(z.y())));
        line[int(z.x())] = Qt::black;
    }
}


Point Points::getPoint(const size_t i) const
{
    return points_[i].first;
}


size_t Points::getClass(const size_t i) const
{
   return points_[i].second;
}


size_t Points::size() const
{
    return points_.size();
}


Colorer::Colorer(const size_t maxColorNum)
    : maxColorNum_(maxColorNum)
{ }


QColor Colorer::getColor(const size_t n) const
{
    const double x = 6. * (n % maxColorNum_) / maxColorNum_;
    size_t r;
    size_t g;
    size_t b;
    if (x <= 1) {
        b = 255;
        g = 255*x;
        r = 0;
    } else if (x <= 2) {
        b = 255 - 255*(x - 1);
        g = 255;
        r = 0;
    } else if (x <= 3) {
        b = 0;
        g = 255;
        r = 255*(x - 2);
    } else if (x <= 4) {
        b = 0;
        g = 255 - 255*(x - 3);
        r = 255;
    } else if (x <= 5) {
        b = 255*(x - 4);
        g = 0;
        r = 255;
    } else {
        b = 255;
        g = 0;
        r = 255 - 255*(x - 5);
    }
    return QColor(r, g, b);
}


IFS::IFS(const std::vector<AffineTransform> &ts, const std::vector<double> &ps)
    : ts_(ts)
    , ps_(ps)
    , dist_(0, 1)
{
}


Point IFS::apply(const size_t i, const Point &z) const
{
    return ts_[i].apply(z);
}


Point IFS::applyInvert(const size_t i, const Point &z) const
{
    return ts_[i].applyInvert(z);
}


size_t IFS::getRandom() const
{
    //const double x = 1. * rand() / RAND_MAX;
    const double x = dist_(rnd_);
    size_t i = 0;
    double s = ps_[0];
    for (; i < ps_.size() - 1;) {
        if (x < s) {
            break;
        }
        i += 1;
        s += ps_[i];
    }
    return i;
}


Points IFS::apply(const size_t maxIterNum, const size_t skipIterNum) const
{
    Point z(0, 0);
    std::vector<std::pair<Point, size_t> > points;
    for (size_t i = 0; i < maxIterNum; ++i) {
        const size_t cl = this->getRandom();
        z = this->apply(cl, z);
        if (i > skipIterNum) {
            points.push_back(std::make_pair(z, cl));
        }
    }
    return Points(points);
}


void IFS::drawR(
         QImage &canvas,
         const size_t maxIterNum,
         const size_t skipIterNum) const
{
    int width = canvas.width();
    int height = canvas.height();
    const Points points = this->apply(maxIterNum, skipIterNum);
    const AffineTransform stretchTransform = points.calcStretchTransform(width, height);
    points.draw(canvas, stretchTransform);
}


void IFS::drawJ(
        QImage &canvas,
        const Colorer &colorer,
        std::atomic<bool> &abort,
        const size_t maxIterNum,
        const size_t sqrdBound) const
{
    int width = canvas.width();
    int height = canvas.height();
    const Points points = this->apply();
    const AffineTransform stretchTransform = points.calcStretchTransform(width, height);
    const Splitter splitter(points);
    for (int y = 0; y < height; ++y) {
        QRgb *line = reinterpret_cast<QRgb *>(canvas.scanLine(y));
        if ( abort.load() )
          break;
        for (int x = 0; x < width; ++x) {
                Point z(x, y);
                z = stretchTransform.applyInvert(z);
                size_t iter = 0;
                while (iter < maxIterNum && z.sqrdLen() < sqrdBound) {
                     size_t cl = splitter.getClass(z);
                     z = this->applyInvert(cl, z);
                     iter += 1;
                }
                line[x] = colorer.getColor(iter).rgb();
        }
    }
}


void IFS::drawSplitter(
        QImage &canvas,
        const Colorer &colorer) const
{
    //int width = canvas.width();
    //int height = canvas.height();
    const Points points = this->apply();
    //const AffineTransform stretchTransform = points.calcStretchTransform(width, height);
    const Splitter splitter(points);
    splitter.draw(canvas, colorer);
}


NNClassifier::NNClassifier(const Points &points)
{
    queryPt_ = annAllocPt(2);
    dataPts_ = annAllocPts(points.size(), 2);
    nnIdx_ = new ANNidx[1];
    dists_ = new ANNdist[1];
    for (size_t i = 0; i < points.size(); ++i) {
        dataPts_[i][0] = points.getPoint(i).x();
        dataPts_[i][1] = points.getPoint(i).y();
    }
    kdTree_ = new ANNkd_tree(dataPts_, points.size(), 2);
}


size_t NNClassifier::getIndex(const Point &z)
{
    queryPt_[0] = z.x();
    queryPt_[1] = z.y();
    kdTree_->annkSearch(queryPt_, 1, nnIdx_, dists_, 0.0);
    return nnIdx_[0];
}


NNClassifier::~NNClassifier()
{
    delete [] nnIdx_;
    delete [] dists_;
    delete kdTree_;
    annClose();
}


Splitter::Splitter(const Points &points)
    : classifier_(points)
    , points_(points)
{ }


size_t Splitter::getClass(const Point &z) const
{
    return points_.getClass(classifier_.getIndex(z));
}


void Splitter::draw(
        QImage &canvas,
        const Colorer &colorer) const
{
    int width = canvas.width();
    int height = canvas.height();
    const AffineTransform stretchTransform = points_.calcStretchTransform(width, height);
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
                const Point z = stretchTransform.applyInvert(Point(x, y));
                size_t cl = this->getClass(z);
                //canvas.setPixelColor(x, y, colorer.getColor(cl));
                QRgb *line = reinterpret_cast<QRgb *>(canvas.scanLine(y));
                line[x] = colorer.getColor(cl).rgb();
        }
    }
}

} // namespace ru_xaoc_fractalworld
