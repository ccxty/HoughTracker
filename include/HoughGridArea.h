#include <cmath>
#ifndef __HOUGHGRIDAREA_CXX_INCLUDE__
#define __HOUGHGRIDAREA_CXX_INCLUDE__

#include <iostream>
#include <vector>

#include "HitPoint.h"

namespace Hough {
enum class GridMode { POINT, GRID };
class HoughGridArea {
 private:
    double _xMin;
    double _xMax;
    double _yMin;
    double _yMax;
    GridMode _mode;
    double _xMid{};
    std::vector<HitPoint *> _points;
    int _counts = 0;

 public:
    HoughGridArea();
    HoughGridArea(double x_min, double x_max, double y_min, double y_max);
    HoughGridArea(double x_mid, double y_min, double y_max);
    HoughGridArea &SetX(double x_min, double x_max);
    HoughGridArea &SetX(double x_mid);
    HoughGridArea &SetY(double y_min, double y_max);
    std::vector<HitPoint *> &GetPointsHere();
    int counts() const;
    inline double xMin() const;
    double xMax() const;
    double yMin() const;
    double yMax() const;
    double xMid() const;
    GridMode mode();
    HoughGridArea &CountsChange(int counts);
    HoughGridArea &CountsAddOne();
    HoughGridArea &AddPoint(HitPoint *point);
    bool CountsZero() const;
    HoughGridArea &CountsReset();
    void Print();
};
}  // namespace Hough

#endif