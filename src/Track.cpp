#include "Track.h"

Track::Track() = default;

Track::Track(std::vector<HitPoint *> ptr)
    : _ptr(std::move(ptr)), _counts(static_cast<int>(_ptr.size())) {}

Track::Track(HitPoint *point) : _counts(1) {
    _ptr.push_back(point);
}

Track &Track::AddPoint(HitPoint *point) {
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
    return *this;
}

int Track::Counts() const {
    return _counts;
}

double Track::Pt() const {
    return _params.R * 0.3;
}

void Track::Print() const {
    if (_ptr.empty()) {
        std::cout << "Empty\n";
        return;
    }
    std::cout << "counts: " << _counts << "\n";
    for (auto *point : _ptr) {
        point->Print();
    }
}

std::vector<HitPoint *> &Track::GetPoints() {
    return _ptr;
}

bool Track::operator!=(Track &other) const {
    return !this->operator==(other);
}

bool Track::operator==(Track &other) const {
    if (this->_counts == other._counts) {
        if (this->_counts != 0) {
            for (auto *point1 : _ptr) {
                bool find = false;
                for (auto *point2 : other._ptr) {
                    if (point1 == point2) {
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

bool Track::operator>=(Track &other) const {
    if (_counts < other._counts) {
        return false;
    }
    for (auto *point : other._ptr) {
        bool find = false;
        for (auto *exist : _ptr) {
            if (point == exist) {
                find = true;
                break;
            }
        }
        if (!find) {
            return false;
        }
    }
    return true;
}

// ???????????? HitALayers();
bool Track::FitLinear(double *Qmin, double *Qz) {
    this->LayerDistribution();
    std::vector<HitPoint *> layer0;
    std::vector<HitPoint *> layer1;
    std::vector<HitPoint *> layer2;
    *Qmin = 1.0;
    *Qz = 1000;
    bool fit = false;
    for (auto *point : _ptr) {
        int layer_id = point->layerID;
        if (layer_id == 0) {
            layer0.push_back(point);
        } else if (layer_id == 1) {
            layer1.push_back(point);
        } else if (layer_id == 2) {
            layer2.push_back(point);
        }
    }
    StraightLine line_xy;
    StraightLine line_z;
    StraightLine line_xy_swap;
    StraightLine line_z_swap;
    double param_a0 = NAN, param_a1 = NAN, param_R = NAN;
    double Q_swap = NAN, Qz_swap = NAN;
    for (auto *point1 : layer0) {
        for (auto *point2 : layer1) {
            for (auto *point3 : layer2) {
                TherePointsLinearFit(point1,
                                     point2,
                                     point3,
                                     &Q_swap,
                                     line_xy_swap);
                double R = sqrt(line_xy_swap.eff[1] * line_xy_swap.eff[1] + 1) /
                           abs(line_xy_swap.eff[0]);
                FitZLinear(point1, point2, point3, R, &Qz_swap, line_z_swap);
                double center_x = -line_xy_swap.eff[1] / line_xy_swap.eff[0];
                double center_y = 1 / line_xy_swap.eff[0];
                double d =
                    fabs(sqrt(center_x * center_x + center_y * center_y) - R);
                if (d > 100) {
                    continue;
                }
                if ((Qz_swap < QzCut) && (Q_swap < *Qmin)) {
                    fit = true;
                    *Qmin = Q_swap;
                    *Qz = Qz_swap;
                    param_R = R;
                    line_xy.eff[0] = line_xy_swap.eff[0];
                    line_xy.eff[1] = line_xy_swap.eff[1];
                    line_z.eff[0] = line_z_swap.eff[0];
                    line_z.eff[1] = line_z_swap.eff[1];
                }
            }
        }
    }
    if (!fit) {
        return false;
    }
    _params.R = param_R;
    _params.kz = line_z.eff[1];
    _params.center.x = -line_xy.eff[1] / line_xy.eff[0];
    _params.center.y = 1 / line_xy.eff[0];
    return true;
}

bool Track::HitALayers() {
    this->LayerDistribution();
    return (_nlayer0 > 0) && (_nlayer1 > 0) && (_nlayer2 > 0);
}

auto Track::GetPointIDSet() const {
    auto ptr = std::make_unique<std::set<int>>();
    if (_counts > 0) {
        for (auto *point : _ptr) {
            ptr->insert(point->id());
        }
    }
    return ptr;
}

int Track::GetSpin() const {  // TODO(tyxiao): ???????????????????????????
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

bool Track::ContainFirstHalf(std::set<int> *events_id) const {
    return this->NumFirstHalfPoints(events_id) == 3;
}

bool Track::ContainSecondHalf(std::set<int> *events_id) const {
    return this->NumSecondHalfPoints(events_id) == 3;
}

int Track::NumFirstHalfPoints(std::set<int> *events_id) const {
    int max = 0;
    for (int e_id : *events_id) {
        int flag = 0;
        for (auto *point : _ptr) {
            if (point->eventID == e_id) {
                int id = point->id();
                if ((id == 0) || (id == 1) || (id == 2)) {
                    flag++;
                }
            }
        }
        if (flag > max) {
            max = flag;
        }
    }
    return max;
}

int Track::NumSecondHalfPoints(std::set<int> *events_id) const {
    int max = 0;
    for (int e_id : *events_id) {
        int flag = 0;
        for (auto *point : _ptr) {
            if (point->eventID == e_id) {
                int id = point->id();
                if ((id == 4) || (id == 5) || (id == 6)) {
                    flag++;
                }
            }
        }
        if (flag > max) {
            max = flag;
        }
    }
    return max;
}

int Track::GetEventID(std::set<int> *events_id) const {
    for (int e_id : *events_id) {
        int flag = 0;
        for (auto *point : _ptr) {
            if (e_id == point->eventID) {
                int id = point->id();
                if ((id == 0) || (id == 1) || (id == 2)) {
                    flag++;
                }
            }
        }
        if (flag >= 3) {
            return e_id;
        }
    }
    return -1;
}

Track &Track::LayerDistribution() {
    _nlayer0 = 0;
    _nlayer1 = 0;
    _nlayer2 = 0;
    if (_ptr.empty()) {
        return *this;
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
    return *this;
}

void Track::Clear() {
    _counts = 0;
    _nlayer0 = 0;
    _nlayer1 = 0;
    _nlayer2 = 0;
    _ptr.clear();
}

bool Track::IsEmpty() const {
    return _counts == 0;
}

int Track::GetNumNoise() const {
    int num_noise = 0;
    for (auto *point : _ptr) {
        if (point->eventID == -1) {
            num_noise++;
        }
    }
    return num_noise;
}

TrackParameters &Track::GetTrackParameters() {
    return _params;
}

std::vector<Track> Track::Split() {
    auto result = std::vector<Track>();
    for (auto *point0 : _ptr) {
        if (point0->layerID == 0) {
            auto track = Track(point0);
            auto temp_l1 = std::vector<HitPoint *>();
            double k1 = point0->z / InnerTrackerR[0];
            for (auto *other : _ptr) {
                bool z_filter =
                    fabs(other->z - point0->z - k1 * DeltaR01) < DeltaZ01;
                bool xy_filter = point0->xyDistance(other) < DMin01;
                if (other->layerID == 1 && z_filter && xy_filter) {
                    track.AddPoint(other);
                    temp_l1.push_back(other);
                }
            }
            for (auto *point1 : temp_l1) {
                double k2 = (point1->z - point0->z) / DeltaR01;
                for (auto *other : _ptr) {
                    double phi2 = other->Phi();
                    bool z_filter =
                        fabs(other->z - point1->z - k2 * DeltaR12) < DeltaZ12;
                    bool xy_filter = point1->xyDistance(other) < DMin12;
                    if (other->layerID == 2 && z_filter && xy_filter) {
                        track.AddPoint(other);
                    }
                }
            }
            result.push_back(std::move(track));
        }
    }
    return result;
}

Track &Track::FilterXY() {
    if (this->HitALayers()) {
        HitPoint *point0 = nullptr;
        if (_nlayer0 == 1) {
            for (auto *other : _ptr) {
                if (other->layerID == 0) {
                    point0 = other;
                }
            }
            std::vector<HitPoint *> temp_layer1;
            for (auto *other : _ptr) {
                if (other->layerID == 1) {
                    if (point0->xyDistance(other) > DMin01) {
                        auto temp = find(_ptr.begin(), _ptr.end(), other);
                        _ptr.erase(temp);
                    } else {
                        temp_layer1.push_back(other);
                    }
                }
            }
            if (!temp_layer1.empty()) {
                for (auto *other : _ptr) {
                    if (other->layerID == 2) {
                        bool in_area = false;
                        for (auto *exist : temp_layer1) {
                            if (other->xyDistance(exist) < DMin12) {
                                in_area = true;
                            }
                        }
                        if (!in_area) {
                            auto temp = find(_ptr.begin(), _ptr.end(), other);
                            _ptr.erase(temp);
                        }
                    }
                }
            }
        }
    }
    return *this;
}
