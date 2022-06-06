#include <algorithm>
#include <array>
#include <cmath>
#include <iostream>
#include <map>
#include <memory>
#include <set>
#include <vector>
#ifndef __TRACK_CXX_INCLUDE__
#define __TRACK_CXX_INCLUDE__

#include "HitPoint.h"
#include "HoughGlobal.h"
#include "global.h"
#include "line.h"

class Track {
 private:
    std::vector<HitPoint *> _ptr;
    int _counts{0};
    int _nlayer0{0};
    int _nlayer1{0};
    int _nlayer2{0};
    TrackParameters _params;

 public:
    Track();
    explicit Track(std::vector<HitPoint *> ptr);
    explicit Track(HitPoint *point);
    int Counts() const;
    Track &AddPoint(HitPoint *point);
    bool FitLinear(double *Qmin, double *Qz);
    double Pt() const;
    void Print() const;
    std::vector<HitPoint *> &GetPoints();
    bool operator==(Track &other) const;
    bool operator!=(Track &other) const;
    bool operator>=(Track &other) const;
    bool HitALayers();
    int NumFirstHalfPoints(std::set<int> *events_id) const;
    int NumSecondHalfPoints(std::set<int> *events_id) const;
    bool ContainFirstHalf(std::set<int> *events_id) const;
    bool ContainSecondHalf(std::set<int> *events_id) const;
    int GetEventID(std::set<int> *events_id) const;
    int GetSpin() const;
    auto GetPointIDSet() const;
    std::tuple<int, int, int> GetLayerDistribution();
    Track &LayerDistribution();
    void Clear();
    bool IsEmpty() const;
    int GetNumNoise() const;
    TrackParameters &GetTrackParameters();
    std::vector<Track> Split();
    Track &FilterXY();
};

#endif