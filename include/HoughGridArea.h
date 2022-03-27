#ifndef HOUGHGRIDAREA_CXX
#define HOUGHGRIDAREA_CXX 1

#include <iostream>
#include <vector>

#include "HoughPoint.h"
enum MODE { POINT, GRID };
class HoughGridArea {
 private:
    double _xMin;
    double _xMax;
    double _yMin;
    double _yMax;
    MODE _mode;
    double _xMid;
    std::vector<HoughPoint *> *_PointsHere;
    int _counts = 0;

 public:
    HoughGridArea();
    ~HoughGridArea();
    HoughGridArea(double xmin, double xmax, double ymin, double ymax);
    HoughGridArea(double xmid, double ymin, double ymax);
    void SetX(double xmin, double xmax);
    void SetX(double xmid);
    void SetY(double ymin, double ymax);
    inline std::vector<HoughPoint *> *GetPointsHere();
    inline int counts();
    inline double xMin();
    inline double xMax();
    inline double yMin();
    inline double yMax();
    inline double xMid();
    MODE mode();
    void CountsChange(int counts);
    void CountsAddOne();
    bool CountsZero();
    void CountsReset();
    void Print();
};

HoughGridArea::HoughGridArea(double xmin, double xmax, double ymin,
                             double ymax) {
    _xMin = xmin;
    _xMax = xmax;
    _yMin = ymin;
    _yMax = ymax;
    _mode = GRID;
    _PointsHere = new std::vector<HoughPoint *>;
}
HoughGridArea::HoughGridArea(double xmid, double ymin, double ymax) {
    _xMid = xmid;
    _yMin = ymin;
    _yMax = ymax;
    _mode = POINT;
    _PointsHere = new std::vector<HoughPoint *>;
}
HoughGridArea::~HoughGridArea() { delete _PointsHere; }
void HoughGridArea::SetX(double xmin, double xmax) {
    if (_mode == GRID) {
        if (_PointsHere == nullptr) {
            std::cout << "pointer null" << std::endl;
            return;
        }
        _xMin = xmin;
        _xMax = xmax;
    }
}
void HoughGridArea::SetX(double xmid) {
    if (_mode == POINT) {
        if (_PointsHere == nullptr) {
            std::cout << "pointer null" << std::endl;
            return;
        }
        _xMid = xmid;
    }
}
void HoughGridArea::SetY(double ymin, double ymax) {
    if (_PointsHere == nullptr) {
        std::cout << "pointer null" << std::endl;
        return;
    }
    _yMin = ymin;
    _yMax = ymax;
}
inline MODE HoughGridArea::mode() { return _mode; }
inline std::vector<HoughPoint *> *HoughGridArea::GetPointsHere() {
    return _PointsHere;
}
inline int HoughGridArea::counts() { return _counts; }
inline double HoughGridArea::xMin() { return _xMin; }
inline double HoughGridArea::xMax() { return _xMax; }
inline double HoughGridArea::yMin() { return _yMin; }
inline double HoughGridArea::yMax() { return _yMax; }
inline double HoughGridArea::xMid() { return _xMid; }
void HoughGridArea::CountsChange(int counts) { _counts = counts; }
void HoughGridArea::CountsAddOne() { _counts += 1; }
bool HoughGridArea::CountsZero() { return (_counts > 0) ? true : false; }
void HoughGridArea::CountsReset() { _counts = 0; }
void HoughGridArea::Print() {
    if (_PointsHere == nullptr) {
        std::cout << "pointer null" << std::endl;
        return;
    }
    if (_PointsHere->size() == 0) {
        std::cout << "no points found" << std::endl;
        return;
    }

    if (_mode == GRID) {
        std::cout << "mode: " << _mode << "counts: " << _counts << std::endl
                  << "xmin: " << _xMin << "\t"
                  << "xmax: " << _xMax << std::endl
                  << "ymin: " << _yMin << "\t"
                  << "ymax: " << _yMax << std::endl;
    }
    if (_mode == POINT) {
        std::cout << "mode: " << _mode << "counts: " << _counts << std::endl
                  << "xmid: " << _xMid << std::endl
                  << "ymin: " << _yMin << "\t"
                  << "ymax: " << _yMax << std::endl;
    }
    for (int i = 0; i < _PointsHere->size(); ++i) {
        _PointsHere->at(i)->Print();
    }
}

#endif