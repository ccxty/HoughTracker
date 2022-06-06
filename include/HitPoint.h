#ifndef __HitPOINT_CXX_INCLUDED__
#define __HitPOINT_CXX_INCLUDED__
#include <cmath>
#include <iostream>
#include <vector>

struct Point2D {
    double x;
    double y;
};

struct Point3D {
    double x;
    double y;
    double z;
};

class HitPoint {
 public:
    double x;
    double y;
    double z;
    int layerID;
    int trackID;
    int eventID;
    double _pt;
    int _id;

    HitPoint();
    HitPoint(double posX, double posY, double posZ, int eventID, int trackID,
             int layerID);
    HitPoint(double posX, double posY, double posZ, int eventID, int trackID,
             int layerID, double pt);
    double xConformal() const;
    double yConformal() const;
    double xyDistance(const HitPoint* other) const;
    double Phi() const;  // [0,2*pi]
    double Pt() const;
    HitPoint& SetPt(double pt);
    int id() const;
    HitPoint& SetId(int identity);
    void Print() const;
};
#endif