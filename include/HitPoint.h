#ifndef __HitPOINT_CXX_INCLUDED__
#define __HitPOINT_CXX_INCLUDED__ 1
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
    inline double xConformal() const;
    inline double yConformal() const;
    inline double Pt() const;
    inline void SetPt(double pt);
    inline int id() const;
    inline void SetId(int identity);
    void Print() const;
};
HitPoint::HitPoint(double x, double y, double z, int event, int track,
                   int layer)
    : x(x),
      y(y),
      z(z),
      layerID(layer),
      trackID(track),
      eventID(event),
      _pt(0),
      _id(-1) {}
HitPoint::HitPoint(double x, double y, double z, int event, int track,
                   int layer, double pt)
    : x(x),
      y(y),
      z(z),
      layerID(layer),
      trackID(track),
      eventID(event),
      _pt(pt),
      _id(-1) {}
inline double HitPoint::xConformal() const { return 2 * x / (x * x + y * y); }
inline double HitPoint::yConformal() const { return 2 * y / (y * y + x * x); }
inline int HitPoint::id() const { return _id; }
inline void HitPoint::SetId(int identity) { _id = identity; }
inline void HitPoint::SetPt(double pt) { _pt = pt; }
inline double HitPoint::Pt() const { return _pt; }
void HitPoint::Print() const {
    std::cout << "identity: " << _id << " "
              << "Pt: " << _pt << " "
              << "eventID: " << eventID << " "
              << "layerID: " << layerID << " "
              << "trackID: " << trackID << " " << x << " " << y << " " << z
              << std::endl;
}
#endif