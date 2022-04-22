#include <array>
#include <cmath>
#include <iostream>
#include <memory>
#include <set>
#include <vector>
#ifndef HOUGHTRACK_CXX
#define HOUGHTRACK_CXX 1

#include "HitPoint.h"
#include "HoughGlobal.h"
#include "global.h"
#include "line.h"
class Track {
 private:
    std::vector<HitPoint *> _ptr;
    double _pt{0};
    std::vector<HitPoint *>::size_type _counts{0};
    int _nlayer0{0};
    int _nlayer1{0};
    int _nlayer2{0};

 public:
    Track();
    explicit Track(std::vector<HitPoint *> ptr);
    explicit Track(HitPoint *point);
    // HoughTrack &operator=(const HoughTrack &track);
    // HoughTrack(const HoughTrack &track);
    std::vector<HitPoint *>::size_type Counts() const;
    void AddPoint(HitPoint *point);
    bool FitLinear(double *pt, double *Qmin, double *Qz);
    double Pt() const;
    void Print() const;
    std::vector<HitPoint *> &GetPoints();
    bool operator==(Track &other) const;
    bool operator!=(Track &other) const;
    bool operator>(Track &other) const;
    bool HitALayers();
    double RatioTrues() const;
    int NumTruePoints() const;
    int NumTruePointsMulti(std::set<int> *events_id) const;
    bool ContainTrueTrack() const;
    bool ContainTrueTrackMulti(std::set<int> *events_id) const;
    int GetEventID(std::set<int> *events_id) const;
    int GetSpin() const;
    auto GetPointIDSet() const;
    std::tuple<int, int, int> GetLayerDistribution();
    void LayerDistribution();
};

Track::Track() = default;

Track::Track(std::vector<HitPoint *> ptr)
    : _ptr(std::move(ptr)), _counts(_ptr.size()) {}

Track::Track(HitPoint *point) : _counts(1) { _ptr.push_back(point); }

void Track::AddPoint(HitPoint *point) {
    if (_ptr.empty()) {
        _ptr.push_back(point);
        _counts = 1;
    } else {
        bool exist = false;
        for (auto *p_exist : _ptr) {
            if (p_exist == point) {
                exist = true;
                break;
            }
        }
        if (!exist) {
            _ptr.push_back(point);
            _counts += 1;
        }
    }
}

std::vector<HitPoint *>::size_type Track::Counts() const { return _counts; }

double Track::Pt() const { return _pt; }

void Track::Print() const {
    if (_ptr.empty()) {
        std::cout << "Empty" << std::endl;
        return;
    }
    std::cout << "counts: " << _counts << std::endl;
    for (auto *point : _ptr) {
        point->Print();
    }
}

std::vector<HitPoint *> &Track::GetPoints() { return _ptr; }

bool Track::operator!=(Track &other) const { return !this->operator==(other); }

bool Track::operator==(Track &other) const {
    if (this->_counts == other._counts) {
        if (this->_counts != 0) {
            for (auto *point1 : _ptr) {
                bool find = false;
                for (auto *point2 : other._ptr) {
                    if (point1->id() == point2->id()) {
                        find = true;
                        break;
                    }
                }
                if (!find) {
                    return false;
                }
            }
        }
        return true;
    }
    return false;
}

bool Track::operator>(Track &other) const { return false; }

// 需先调用 HitALayers();
bool Track::FitLinear(double *pt, double *Qmin, double *Qz) {
    auto nlayer = this->GetLayerDistribution();
    HitPoint *layer0[std::get<0>(nlayer)];
    HitPoint *layer1[std::get<1>(nlayer)];
    HitPoint *layer2[std::get<2>(nlayer)];
    int i_0 = 0, i_1 = 0, i_2 = 0;
    *Qmin = 1.0;
    *Qz = 1000;
    bool fit = false;
    for (int i = 0; i < _counts; i++) {
        auto *point = _ptr.at(i);
        int layer_id = point->layerID;
        if (layer_id == 0) {
            layer0[i_0] = point;
            i_0++;
        } else if (layer_id == 1) {
            layer1[i_1] = point;
            i_1++;
        } else if (layer_id == 2) {
            layer2[i_2] = point;
            i_2++;
        }
    }
    Polynomial<2> line_xy;
    Polynomial<2> line_z;
    double param_a0 = NAN, param_a1 = NAN, param_R = NAN;
    double Q_swap = NAN, Qz_swap = NAN;
    for (auto *point1 : layer0) {
        for (auto *point2 : layer1) {
            for (auto *point3 : layer2) {
                TherePointsLinearFit(point1, point2, point3, &Q_swap, line_xy);
                double R = sqrt(line_xy.eff[1] * line_xy.eff[1] + 1) /
                           abs(line_xy.eff[0]);
                FitZLinear(point1, point2, point3, R, &Qz_swap, line_z);
                if ((Qz_swap < QzCut) && (Q_swap < *Qmin)) {
                    fit = true;
                    *Qmin = Q_swap;
                    *Qz = Qz_swap;
                    param_R = R;
                }
            }
        }
    }
    if (!fit) {
        return false;
    }
    *pt = 0.3 * param_R;
    _pt = *pt;
    return true;
}

bool Track::HitALayers() {
    this->LayerDistribution();
    return (_nlayer0 > 0) && (_nlayer1 > 0) && (_nlayer2 > 0);
}

int Track::NumTruePoints() const {
    int trues = 0;
    for (auto *point : _ptr) {
        if (point->eventID != -1) {
            trues++;
        }
    }
    return trues;
}

double Track::RatioTrues() const {
    return this->NumTruePoints() / static_cast<double>(_counts);
}

bool Track::ContainTrueTrack() const { return this->NumTruePoints() == 3; }

auto Track::GetPointIDSet() const {
    auto ptr = std::make_unique<std::set<int>>();
    if (_counts > 0) {
        for (auto *point : _ptr) {
            ptr->insert(point->id());
        }
    }
    return ptr;
}

int Track::GetSpin() const {  // TODO(tyxiao): 需要改变计算的方式
    if (_counts < 3) {
        return 0;
    }
    int i_point_layer0 = 0;
    int i_point_layer1 = 0;
    int i_point_layer2 = 0;
    for (int i = 0; i < _counts; i++) {
        int layerID = _ptr.at(i)->layerID;
        if (layerID == 0) {
            i_point_layer0 = i;
        } else if (layerID == 1) {
            i_point_layer1 = i;
        } else if (layerID == 2) {
            i_point_layer2 = i;
        }
    }
    auto *point0 = _ptr.at(i_point_layer0);
    auto *point1 = _ptr.at(i_point_layer1);
    auto *point2 = _ptr.at(i_point_layer2);
    double theta0 = atan2(point0->y, point0->x);
    double theta1 = atan2(point1->y, point1->x);
    double theta2 = atan2(point2->y, point2->x);
    if (theta0 * theta1 > 0) {
        if (theta0 > theta1) {
            return 1;
        }
        return -1;
    }
    if (point0->x * point0->y > 0) {
        return 1;
    }
    return -1;
}

std::tuple<int, int, int> Track::GetLayerDistribution() {
    if ((_nlayer0 + _nlayer1 + _nlayer2) != _counts) {
        this->LayerDistribution();
    }
    return std::make_tuple(_nlayer0, _nlayer1, _nlayer2);
}

bool Track::ContainTrueTrackMulti(std::set<int> *events_id) const {
    return this->NumTruePointsMulti(events_id) == 3;
}

int Track::NumTruePointsMulti(std::set<int> *events_id) const {
    int max = 0;
    for (int eventID : *events_id) {
        int flag = 0;
        for (auto *point : _ptr) {
            if (eventID == point->eventID) {
                flag++;
            }
        }
        if (flag > max) {
            max = flag;
        }
    }
    return max;
}

int Track::GetEventID(std::set<int> *events_id) const {
    for (int eventID : *events_id) {
        int flag = 0;
        for (auto *point : _ptr) {
            if (point->eventID == eventID) {
                flag++;
            }
        }
        if (flag >= 3) {
            return eventID;
        }
    }
    return -1;
}

void Track::LayerDistribution() {
    _nlayer0 = 0;
    _nlayer1 = 0;
    _nlayer2 = 0;
    if (_ptr.empty()) {
        return;
    }
    for (auto *point : _ptr) {
        int layerID = point->layerID;
        if (layerID == 0) {
            _nlayer0++;
        } else if (layerID == 1) {
            _nlayer1++;
        } else if (layerID == 2) {
            _nlayer2++;
        }
    }
}

#endif