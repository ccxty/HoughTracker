#include "HoughGridArea.h"
namespace Hough {
HoughGridArea::HoughGridArea(double x_min, double x_max, double y_min,
                             double y_max)
    : _xMin(x_min),
      _xMax(x_max),
      _yMin(y_min),
      _yMax(y_max),
      _xMid(NAN),
      _mode(GridMode::GRID) {}
HoughGridArea::HoughGridArea(double x_mid, double y_min, double y_max)
    : _yMin(y_min),
      _yMax(y_max),
      _mode(GridMode::POINT),
      _xMid(x_mid),
      _xMin(NAN),
      _xMax(NAN) {}
HoughGridArea &HoughGridArea::SetX(double x_min, double x_max) {
    if (_mode == GridMode::GRID) {
        _xMin = x_min;
        _xMax = x_max;
    }
    return *this;
}
HoughGridArea &HoughGridArea::SetX(double x_mid) {
    if (_mode == GridMode::POINT) {
        _xMid = x_mid;
    }
    return *this;
}
HoughGridArea &HoughGridArea::SetY(double y_min, double y_max) {
    _yMin = y_min;
    _yMax = y_max;
    return *this;
}
GridMode HoughGridArea::mode() { return _mode; }
std::vector<HitPoint *> &HoughGridArea::GetPointsHere() { return _points; }
int HoughGridArea::counts() const { return _counts; }
double HoughGridArea::xMin() const { return _xMin; }
double HoughGridArea::xMax() const { return _xMax; }
double HoughGridArea::yMin() const { return _yMin; }
double HoughGridArea::yMax() const { return _yMax; }
double HoughGridArea::xMid() const { return _xMid; }
HoughGridArea &HoughGridArea::CountsChange(int counts) {
    _counts = counts;
    return *this;
}
HoughGridArea &HoughGridArea::CountsAddOne() {
    _counts += 1;
    return *this;
}
bool HoughGridArea::CountsZero() const { return _counts > 0; }
HoughGridArea &HoughGridArea::CountsReset() {
    _counts = 0;
    return *this;
}
void HoughGridArea::Print() {
    if (_points.empty()) {
        std::cout << "no points found\n";
        return;
    }

    if (_mode == GridMode::GRID) {
        std::cout << "mode: "
                  << "Grid\t"
                  << "counts: " << _counts << "\n"
                  << "x_min: " << _xMin << "\t"
                  << "x_max: " << _xMax << "\n"
                  << "y_min: " << _yMin << "\t"
                  << "y_max: " << _yMax << "\n";
    }
    if (_mode == GridMode::POINT) {
        std::cout << "mode: "
                  << "Point\t"
                  << "counts: " << _counts << "\n"
                  << "x_mid: " << _xMid << "\n"
                  << "y_min: " << _yMin << "\t"
                  << "y_max: " << _yMax << "\n";
    }
    for (auto *point : _points) {
        point->Print();
    }
}
}  // namespace Hough