// Matvej Kotov, 2014
// http://fractalworld.xaoc.ru/

#pragma once

#include <ANN/ANN.h> // ANN can be downloaded here http://www.cs.umd.edu/~mount/ANN/
#include <vector>
#include <atomic>
#include <random>
#include <QImage>
#include <QColor>
#include <eigen3/Eigen/Dense>
#include <eigen3/Eigen/Geometry>

namespace ru_xaoc_fractalworld
{


class Point
{
public:
  Point(const double x, const double y)
    : p_(x, y)
  {
  }
  
  Point(const Eigen::Vector2d &v)
    : p_(v)
  {
  }
  
  double x() const
  {
    return p_(0);
  }
  
  double y() const
  {
    return p_(1);
  }
  
  double sqrdLen() const
  {
    return p_.squaredNorm();
  }
  
  const Eigen::Vector2d &operator()() const
  {
    return p_;
  }
  
private:
  Eigen::Vector2d p_;
};


class AffineTransform
{
public:
  AffineTransform(
    const double a,
    const double b,
    const double c,
    const double d,
    const double e,
    const double f)
  {
    t_(0, 0) = a; t_(0, 1) = b; t_(0, 2) = e;
    t_(1, 0) = c; t_(1, 1) = d; t_(1, 2) = f;
    ti_ = t_.inverse(Eigen::Affine);
  }
  
  Point apply(const Point &z) const
  {
    return Point(t_*z());
  }
  
  Point applyInvert(const Point &z) const
  {
    return Point(ti_*z());
  }
  
private:
  Eigen::Transform<double, 2, Eigen::Affine> t_;
  Eigen::Transform<double, 2, Eigen::Affine> ti_;
};


class Points
{
private:
    std::vector<std::pair<Point, size_t> > points_;

public:
    explicit Points(const std::vector<std::pair<Point, size_t> > &points);
    AffineTransform calcStretchTransform(
                const size_t width, const size_t height,
                const size_t border = 40) const;
    void draw(QImage &canvas, const AffineTransform &stretchTransform) const;
    Point getPoint(const size_t i) const;
    size_t getClass(const size_t i) const;
    size_t size() const;
};


class Colorer
{
private:
    size_t maxColorNum_;

public:
    explicit Colorer(const size_t maxColorNum);
    QColor getColor(const size_t n) const;
};


class IFS
{
private:
     std::vector<AffineTransform> ts_;
     std::vector<double> ps_;
     mutable std::mt19937_64 rnd_;
     mutable std::uniform_real_distribution<double> dist_;

public:
    IFS(const std::vector<AffineTransform> &ts, const std::vector<double> &ps);
    Point apply(const size_t i, const Point &z) const;
    Point applyInvert(const size_t i, const Point &z) const;
    size_t getRandom() const;

    Points apply(const size_t maxIterNum = 1000, const size_t skipIterNum = 50) const;
    void drawR(QImage &canvas,
            const size_t maxIterNum = 1000, const size_t skipIterNum = 50) const;
    void drawJ(QImage &canvas,
            const Colorer &colorer,
            std::atomic<bool> &abort,
            const size_t maxIterNum = 16, const size_t sqrdBound = 16) const;
    void drawSplitter(QImage &canvas,
            const Colorer &colorer) const;

};


class NNClassifier
{
private:
    ANNpointArray dataPts_;
    ANNpoint queryPt_;
    ANNidxArray nnIdx_;
    ANNdistArray dists_;
    ANNkd_tree *kdTree_;

    NNClassifier(const NNClassifier &); /* = delete */
    NNClassifier operator=(const NNClassifier &); /* = delete */

public:
    explicit NNClassifier(const Points &points);
    size_t getIndex(const Point &z);
    ~NNClassifier();
};


class Splitter
{
private:
    mutable NNClassifier classifier_;
    Points points_;

public:
    explicit Splitter(const Points &points);
    size_t getClass(const Point &z) const;
    void draw(QImage &canvas,
            const Colorer &colorer) const;
};

} // namespace ru_xaoc_fractalworld
