#ifndef HOUGHTRACK_CXX
#define HOUGHTRACK_CXX 1
#include <iostream>
#include <vector>
#include "HoughPoint.h"
#include "TF1.h"
#include "TLinearFitter.h"
#include <set>
#include "Hough.h"
class HoughTrack
{
private:
    std::vector<HoughPoint *> *_ptr;
    double _pt;
    int _counts = 0;
    bool _fit = false;
    int _nlayer0 = 0;
    int _nlayer1 = 0;
    int _nlayer2 = 0;

public:
    HoughTrack();
    HoughTrack(std::vector<HoughPoint *> *ptr);
    HoughTrack(HoughPoint *point);
    ~HoughTrack();
    int Counts() const;
    void AddPoint(HoughPoint *point);
    bool FitLinear(double *pt, double *Qmin);
    double Pt() const;
    void Print() const;
    std::vector<HoughPoint *> *GetPoints() const;
    bool operator==(HoughTrack *other) const;
    bool operator!=(HoughTrack *other) const;
    bool operator>(HoughTrack *other) const;
    bool HitALayers() const;
    double RatioTrues() const;
    int NumTruePoints() const;
    bool ContainTrueTrack() const;
    int GetSpin() const;
    std::set<int> *GetPointIDSet() const;
    void GetLayerDistribution(int *layer0, int *layer1, int *layer2) const;
};

HoughTrack::HoughTrack()
{
    _ptr = new std::vector<HoughPoint *>;
}

HoughTrack::~HoughTrack()
{
    delete _ptr;
}

HoughTrack::HoughTrack(std::vector<HoughPoint *> *ptr)
{
    _ptr = ptr;
    _counts = ptr->size();
    if (_counts > 0)
    {
        for (int i = 0; i < _counts; i++)
        {
            auto point = _ptr->at(i);
            int layerID = point->layerID();
            if (layerID == 0)
                _nlayer0++;
            else if (layerID == 1)
                _nlayer1++;
            else if (layerID == 2)
                _nlayer2++;
        }
    }
}

HoughTrack::HoughTrack(HoughPoint *point)
{
    _ptr = new std::vector<HoughPoint *>;
    _ptr->push_back(point);
    _counts = 1;
    int layerID = point->layerID();
    if (layerID == 0)
        _nlayer0++;
    else if (layerID == 1)
        _nlayer1++;
    else if (layerID == 2)
        _nlayer2++;
}

void HoughTrack::AddPoint(HoughPoint *point)
{
    if (_ptr == nullptr)
    {
        _ptr = new std::vector<HoughPoint *>;
        _ptr->push_back(point);
        _counts = 1;
    }
    else if (_ptr->size() == 0)
    {
        _ptr->push_back(point);
        _counts = 1;
    }
    else
    {
        int flag = 0;
        for (int i = 0; i < _ptr->size(); i++)
        {
            auto point_existing = _ptr->at(i);
            if (point_existing == point)
            {
                flag = 1;
                break;
            }
        }
        if (flag == 0)
        {
            _ptr->push_back(point);
            _counts += 1;
            int layerID = point->layerID();
            if (layerID == 0)
                _nlayer0++;
            else if (layerID == 1)
                _nlayer1++;
            else if (layerID == 2)
                _nlayer2++;
        }
    }
}

int HoughTrack::Counts() const { return _counts; }

double HoughTrack::Pt() const { return _pt; }

void HoughTrack::Print() const
{
    if (_ptr == nullptr)
    {
        std::cout << "Not Inited correctly" << std::endl;
        return;
    }
    if (_ptr->size() == 0)
    {
        std::cout << "Empty" << std::endl;
        return;
    }
    std::cout << "counts: " << _counts << std::endl;
    for (int i = 0; i < _ptr->size(); i++)
    {
        _ptr->at(i)->Print();
    }
}

std::vector<HoughPoint *> *HoughTrack::GetPoints() const { return _ptr; }

bool HoughTrack::operator!=(HoughTrack *other) const
{
    if (this->operator==(other))
    {
        return false;
    }
    else
        return true;
}

bool HoughTrack::operator==(HoughTrack *other) const
{
    if (this->_counts != other->_counts)
        return false;
    bool is_equal = true;
    if (this->_counts != 0)
    {
        auto ptr1 = this->GetPoints();
        auto ptr2 = other->GetPoints();
        for (int i = 0; i < ptr1->size(); i++)
        {
            auto point1 = ptr1->at(i);
            int equal_counts = 0;
            for (int j = 0; j < ptr2->size(); j++)
            {
                auto point2 = ptr2->at(j);
                if (point1->id() == point2->id())
                    equal_counts++;
            }
            // std::cout << equal_counts << std::endl;
            if (equal_counts != 1)
            {
                is_equal = false;
            }
        }
    }
    return is_equal;
}

bool HoughTrack::operator>(HoughTrack *other) const
{
    return false;
}

// 先调用 HitALayers();
bool HoughTrack::FitLinear(double *pt, double *Qmin)
{
    double x[_counts];
    double y[_counts];
    HoughPoint *layer0[_nlayer0];
    HoughPoint *layer1[_nlayer1];
    HoughPoint *layer2[_nlayer2];
    int i0 = 0;
    int i1 = 0;
    int i2 = 0;
    *Qmin = 1.0;
    for (int i = 0; i < _counts; i++)
    {
        auto point = _ptr->at(i);
        int layer_id = point->layerID();
        if (layer_id == 0)
        {
            layer0[i0] = point;
            i0++;
        }
        else if (layer_id == 1)
        {
            layer1[i1] = point;
            i1++;
        }
        else if (layer_id == 2)
        {
            layer2[i2] = point;
            i2++;
        }
    }

    std::cout << "counts: " << _counts << std::endl;
    std::cout << "number of points in each layer: "
              << _nlayer0 << " "
              << _nlayer1 << " "
              << _nlayer2 << std::endl;
    double param[2];
    double a0, a1;
    double Q_swap;
    for (auto point1 : layer0)
    {
        for (auto point2 : layer1)
        {
            for (auto point3 : layer2)
            {
                TherePointsLinearFit(point1, point2, point3, &Q_swap, param);
                if (Q_swap < *Qmin)
                {
                    *Qmin = Q_swap;
                    a0 = param[0];
                    a1 = param[1];
                }
            }
        }
    }
    std::cout << "a0: " << a0 << "\t"
              << "a1: " << a1 << "\t"
              << "Qmin: " << *Qmin << std::endl;
    double d = abs(a0) / sqrt(1 + a1 * a1);
    std::cout << "d: " << d << std::endl;
    std::cout << "Pt: " << 0.3 / d << std::endl;
    *pt = 0.3 / d;
    _pt = *pt;
    return true;
}

bool HoughTrack::HitALayers() const
{
    return (_nlayer0 > 0) && (_nlayer1 > 0) && (_nlayer2 > 0);
}

int HoughTrack::NumTruePoints() const
{
    int trues = 0;
    for (int i = 0; i < _counts; i++)
    {
        // int id = _ptr->at(i)->id();
        // if ((id == 0) || (id == 1) || (id == 2))
        //     trues++;
        int event_id = _ptr->at(i)->eventID();
        if (event_id != -1)
            trues++;
    }
    std::cout << "NumberTrue: " << trues << std::endl;
    return trues;
}

double HoughTrack::RatioTrues() const
{
    return this->NumTruePoints() / double(_counts);
}

bool HoughTrack::ContainTrueTrack() const
{
    return this->NumTruePoints() == 3;
}

std::set<int> *HoughTrack::GetPointIDSet() const
{
    auto ptr = new std::set<int>;
    if (_counts > 0)
    {
        for (int i = 0; i < _counts; i++)
        {
            ptr->insert(_ptr->at(i)->id());
        }
    }
    return ptr;
}

int HoughTrack::GetSpin() const
{
    if (_counts < 3)
    {
        return 0;
    }
    else
    {
        int i_point_layer0;
        int i_point_layer1;
        int i_point_layer2;
        for (int i = 0; i < _counts; i++)
        {
            int layerID = _ptr->at(i)->layerID();
            if (layerID == 0)
            {
                i_point_layer0 = i;
            }
            else if (layerID == 1)
            {
                i_point_layer1 = i;
            }
            else if (layerID == 2)
            {
                i_point_layer2 = i;
            }
        }
        auto point0 = _ptr->at(i_point_layer0);
        auto point1 = _ptr->at(i_point_layer1);
        auto point2 = _ptr->at(i_point_layer2);
        double theta0 = atan2(point0->y(), point0->x());
        double theta1 = atan2(point1->y(), point1->x());
        double theta2 = atan2(point2->y(), point2->x());
        if (theta0 * theta1 > 0)
        {
            if (theta0 > theta1)
                return 1;
            else
                return -1;
        }
        else
        {
            if (point0->x() * point0->y() > 0)
                return 1;
            else
                return -1;
        }
    }
}

void HoughTrack::GetLayerDistribution(int *layer0, int *layer1, int *layer2) const
{
    *layer0 = _nlayer0;
    *layer1 = _nlayer1;
    *layer2 = _nlayer2;
}
#endif