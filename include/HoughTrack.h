#ifndef HOUGHTRACK_CXX
#define HOUGHTRACK_CXX 1
#include <array>
#include <iostream>
#include <set>
#include <vector>

#include "Hough.h"
#include "HoughPoint.h"
#include "TF1.h"
#include "TLinearFitter.h"
class HoughTrack {
 private:
    std::vector<HoughPoint *> *_ptr;
    double _pt{0};
    std::vector<HoughPoint *>::size_type _counts{0};
    int _nlayer1{0};
    int _nlayer0{0};
    int _nlayer2{0};

 public:
    HoughTrack();
    explicit HoughTrack(std::vector<HoughPoint *> *ptr);
    explicit HoughTrack(HoughPoint *point);
    ~HoughTrack();
    // HoughTrack &operator=(const HoughTrack &track);
    // HoughTrack(const HoughTrack &track);
    std::vector<HoughPoint *>::size_type Counts() const;
    void AddPoint(HoughPoint *point);
    bool FitLinear(double *p_t, double *Qmin, double *Q_z);
    double Pt() const;
    void Print() const;
    std::vector<HoughPoint *> *GetPoints() const;
    bool operator==(HoughTrack *other) const;
    bool operator!=(HoughTrack *other) const;
    bool operator>(HoughTrack *other) const;
    bool HitALayers() const;
    double RatioTrues() const;
    int NumTruePoints() const;
    int NumTruePointsMulti(std::set<int> *events_id) const;
    bool ContainTrueTrack() const;
    bool ContainTrueTrackMulti(std::set<int> *events_id) const;
    int GetEventID(std::set<int> *events_id) const;
    int GetSpin() const;
    std::set<int> *GetPointIDSet() const;
    void GetLayerDistribution(int *layer0, int *layer1, int *layer2) const;
};

HoughTrack::HoughTrack() : _ptr(new std::vector<HoughPoint *>) {}

HoughTrack::~HoughTrack() { delete _ptr; }

HoughTrack::HoughTrack(std::vector<HoughPoint *> *ptr)
    : _ptr(ptr), _counts(ptr->size()) {
    if (_counts > 0) {
        for (auto point : *_ptr) {
            int layerID = point->layerID();
            if (layerID == 0) {
                _nlayer0++;
            } else if (layerID == 1) {
                _nlayer1++;
            } else if (layerID == 2) {
                _nlayer2++;
            }
        }
    }
}

HoughTrack::HoughTrack(HoughPoint *point)
    : _ptr(new std::vector<HoughPoint *>), _counts(1) {
    _ptr->push_back(point);
    int layerID = point->layerID();
    if (layerID == 0) {
        _nlayer0++;
    } else if (layerID == 1) {
        _nlayer1++;
    } else if (layerID == 2) {
        _nlayer2++;
    }
}

void HoughTrack::AddPoint(HoughPoint *point) {
    if (_ptr == nullptr) {
        _ptr = new std::vector<HoughPoint *>;
        _ptr->push_back(point);
        _counts = 1;
    } else if (_ptr->size() == 0) {
        _ptr->push_back(point);
        _counts = 1;
    } else {
        bool exist = false;
        for (auto p_exist : *_ptr) {
            if (p_exist == point) {
                exist = true;
                break;
            }
        }
        if (exist == false) {
            _ptr->push_back(point);
            _counts += 1;
            int layerID = point->layerID();
            if (layerID == 0) {
                _nlayer0++;
            } else if (layerID == 1) {
                _nlayer1++;
            } else if (layerID == 2) {
                _nlayer2++;
            }
        }
    }
}

std::vector<HoughPoint *>::size_type HoughTrack::Counts() const {
    return _counts;
}

double HoughTrack::Pt() const { return _pt; }

void HoughTrack::Print() const {
    if (_ptr == nullptr) {
        std::cout << "Not Initialed correctly" << std::endl;
        return;
    }
    if (_ptr->size() == 0) {
        std::cout << "Empty" << std::endl;
        return;
    }
    std::cout << "counts: " << _counts << std::endl;
    for (auto point : *_ptr) {
        point->Print();
    }
}

std::vector<HoughPoint *> *HoughTrack::GetPoints() const { return _ptr; }

bool HoughTrack::operator!=(HoughTrack *other) const {
    if (this->operator==(other)) {
        return false;
    } else {
        return true;
    }
}

bool HoughTrack::operator==(HoughTrack *other) const {
    if (this->_counts != other->_counts) {
        return false;
    }
    if (this->_counts != 0) {
        auto ptr1 = this->GetPoints();
        auto ptr2 = other->GetPoints();
        for (auto point1 : *ptr1) {
            bool find = false;
            for (auto point2 : *ptr2) {
                if (point1->id() == point2->id()) {
                    find = true;
                    break;
                }
            }
            if (find == false) {
                return false;
            }
        }
    }
    return true;
}

bool HoughTrack::operator>(HoughTrack *other) const { return false; }

// 先调用 HitALayers();
bool HoughTrack::FitLinear(double *pt, double *Qmin, double *Qz) {
    HoughPoint *layer0[_nlayer0];
    HoughPoint *layer1[_nlayer1];
    HoughPoint *layer2[_nlayer2];
    int i_0 = 0;
    int i_1 = 0;
    int i_2 = 0;
    *Qmin = 1.0;
    *Qz = 1000;
    bool fit = false;
    for (int i = 0; i < _counts; i++) {
        auto point = _ptr->at(i);
        int layer_id = point->layerID();
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
    std::array<double, 2> param{};
    double param_a0, param_a1, param_R;
    double Q_swap, Qz_swap;
    for (auto point1 : layer0) {
        for (auto point2 : layer1) {
            for (auto point3 : layer2) {
                TherePointsLinearFit(point1, point2, point3, &Q_swap,
                                     param.data());
                double R = sqrt(param[1] * param[1] + 1) / abs(param[0]);
                double a1_z = FitZLinear(point1, point2, point3, R, &Qz_swap);
                if ((Qz_swap < QzCut) &&
                    (Q_swap < *Qmin) /*&& (a1_z > -0.5)*/) {
                    fit = true;
                    *Qmin = Q_swap;
                    *Qz = Qz_swap;
                    // param_a0 = param[0];
                    // param_a1 = param[1];
                    param_R = R;
                }
            }
        }
    }
    if (fit == false) {
        return false;
    }
    // std::cout << "a0: " << a0 << "\t"
    //           << "a1: " << a1 << "\t"
    //           << "Qmin: " << *Qmin
    //           << "Qz: " << *Qz << std::endl;
    // double d_origin = abs(param_a0) / sqrt(1 + param_a1 * param_a1);
    // std::cout << "d: " << d << std::endl;
    // std::cout << "Pt: " << 0.3 / d << std::endl;
    *pt = 0.3 * param_R;
    _pt = *pt;
    return true;
}

bool HoughTrack::HitALayers() const {
    return (_nlayer0 > 0) && (_nlayer1 > 0) && (_nlayer2 > 0);
}

int HoughTrack::NumTruePoints() const {
    int trues = 0;
    for (int i = 0; i < _counts; i++) {
        // int id = _ptr->at(i)->id();
        // if ((id == 0) || (id == 1) || (id == 2))
        //     trues++;
        int event_id = _ptr->at(i)->eventID();
        if (event_id != -1) {
            trues++;
        }
    }
    std::cout << "NumberTrue: " << trues << std::endl;
    return trues;
}

double HoughTrack::RatioTrues() const {
    return this->NumTruePoints() / static_cast<double>(_counts);
}

bool HoughTrack::ContainTrueTrack() const { return this->NumTruePoints() == 3; }

std::set<int> *HoughTrack::GetPointIDSet() const {
    auto ptr = new std::set<int>;
    if (_counts > 0) {
        for (auto point : *_ptr) {
            ptr->insert(point->id());
        }
    }
    return ptr;
}

int HoughTrack::GetSpin() const {  // TODO(tyxiao): 需要改变计算的方式
    if (_counts < 3) {
        return 0;
    } else {
        int i_point_layer0 = 0;
        int i_point_layer1 = 0;
        int i_point_layer2 = 0;
        for (int i = 0; i < _counts; i++) {
            int layerID = _ptr->at(i)->layerID();
            if (layerID == 0) {
                i_point_layer0 = i;
            } else if (layerID == 1) {
                i_point_layer1 = i;
            } else if (layerID == 2) {
                i_point_layer2 = i;
            }
        }
        auto point0 = _ptr->at(i_point_layer0);
        auto point1 = _ptr->at(i_point_layer1);
        auto point2 = _ptr->at(i_point_layer2);
        double theta0 = atan2(point0->y(), point0->x());
        double theta1 = atan2(point1->y(), point1->x());
        double theta2 = atan2(point2->y(), point2->x());
        if (theta0 * theta1 > 0) {
            if (theta0 > theta1) {
                return 1;
            } else {
                return -1;
            }
        } else {
            if (point0->x() * point0->y() > 0) {
                return 1;
            } else {
                return -1;
            }
        }
    }
}

void HoughTrack::GetLayerDistribution(int *layer0, int *layer1,
                                      int *layer2) const {
    *layer0 = _nlayer0;
    *layer1 = _nlayer1;
    *layer2 = _nlayer2;
}

bool HoughTrack::ContainTrueTrackMulti(std::set<int> *events_id) const {
    return this->NumTruePointsMulti(events_id) == 3;
}

int HoughTrack::NumTruePointsMulti(std::set<int> *events_id) const {
    int max = 0;
    for (int eventID : *events_id) {
        int flag = 0;
        for (auto point : *_ptr) {
            if (eventID == point->eventID()) {
                flag++;
            }
        }
        if (flag > max) {
            max = flag;
        }
    }
    return max;
}

int HoughTrack::GetEventID(std::set<int> *event_id) const {
    for (int eventID : *event_id) {
        int flag = 0;
        for (auto point : *_ptr) {
            if (point->eventID() == eventID) {
                flag++;
            }
        }
        if (flag >= 3) {
            return eventID;
        }
    }
    return -1;
}
#endif