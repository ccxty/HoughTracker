#include <cmath>
#ifndef HOUGHGRIDAREA_CXX
#define HOUGHGRIDAREA_CXX 1

#include <iostream>
#include <vector>

#include "HoughPoint.h"
enum class GridMode { POINT, GRID };
class HoughGridArea {
 private:
    double _xMin;
    double _xMax;
    double _yMin;
    double _yMax;
    GridMode _mode;
    double _xMid{};
    std::vector<HoughPoint *> *_pointsHere;
    int _counts = 0;

 public:
    HoughGridArea();
    ~HoughGridArea();
    HoughGridArea(double x_min, double x_max, double y_min, double y_max);
    HoughGridArea(double x_mid, double y_min, double y_max);
    void SetX(double x_min, double x_max);
    void SetX(double x_mid);
    void SetY(double y_min, double y_max);
    inline std::vector<HoughPoint *> *GetPointsHere();
    inline int counts() const;
    inline double xMin() const;
    inline double xMax() const;
    inline double yMin() const;
    inline double yMax() const;
    inline double xMid() const;
    GridMode mode();
    void CountsChange(int counts);
    void CountsAddOne();
    bool CountsZero() const;
    void CountsReset();
    void Print();
};

HoughGridArea::HoughGridArea(double x_min, double x_max, double y_min,
                             double y_max)
    : _xMin(x_min),
      _xMax(x_max),
      _yMin(y_min),
      _yMax(y_max),
      _xMid(NAN),
      _mode(GridMode::GRID),
      _pointsHere(new std::vector<HoughPoint *>) {}
HoughGridArea::HoughGridArea(double x_mid, double y_min, double y_max)
    : _yMin(y_min),
      _yMax(y_max),
      _mode(GridMode::POINT),
      _xMid(x_mid),
      _xMin(NAN),
      _xMax(NAN),
      _pointsHere(new std::vector<HoughPoint *>) {}
HoughGridArea::~HoughGridArea() { delete _pointsHere; }
void HoughGridArea::SetX(double x_min, double x_max) {
    if (_mode == GridMode::GRID) {
        if (_pointsHere == nullptr) {
            std::cout << "pointer null" << std::endl;
            return;
        }
        _xMin = x_min;
        _xMax = x_max;
    }
}
void HoughGridArea::SetX(double x_mid) {
    if (_mode == GridMode::POINT) {
        if (_pointsHere == nullptr) {
            std::cout << "pointer null" << std::endl;
            return;
        }
        _xMid = x_mid;
    }
}
void HoughGridArea::SetY(double y_min, double y_max) {
    if (_pointsHere == nullptr) {
        std::cout << "pointer null" << std::endl;
        return;
    }
    _yMin = y_min;
    _yMax = y_max;
}
inline GridMode HoughGridArea::mode() { return _mode; }
inline std::vector<HoughPoint *> *HoughGridArea::GetPointsHere() {
    return _pointsHere;
}
inline int HoughGridArea::counts() const { return _counts; }
inline double HoughGridArea::xMin() const { return _xMin; }
inline double HoughGridArea::xMax() const { return _xMax; }
inline double HoughGridArea::yMin() const { return _yMin; }
inline double HoughGridArea::yMax() const { return _yMax; }
inline double HoughGridArea::xMid() const { return _xMid; }
void HoughGridArea::CountsChange(int counts) { _counts = counts; }
void HoughGridArea::CountsAddOne() { _counts += 1; }
bool HoughGridArea::CountsZero() const { return _counts > 0; }
void HoughGridArea::CountsReset() { _counts = 0; }
void HoughGridArea::Print() {
    if (_pointsHere == nullptr) {
        std::cout << "pointer null" << std::endl;
        return;
    }
    if (_pointsHere->empty()) {
        std::cout << "no points found" << std::endl;
        return;
    }

    if (_mode == GridMode::GRID) {
        std::cout << "mode: "
                  << "Grid\t"
                  << "counts: " << _counts << std::endl
                  << "x_min: " << _xMin << "\t"
                  << "x_max: " << _xMax << std::endl
                  << "y_min: " << _yMin << "\t"
                  << "y_max: " << _yMax << std::endl;
    }
    if (_mode == GridMode::POINT) {
        std::cout << "mode: "
                  << "Point\t"
                  << "counts: " << _counts << std::endl
                  << "x_mid: " << _xMid << std::endl
                  << "y_min: " << _yMin << "\t"
                  << "y_max: " << _yMax << std::endl;
    }
    for (auto *point : *_pointsHere) {
        point->Print();
    }
}

#endif