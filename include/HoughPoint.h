#ifndef HOUGHPOINT_CXX
#define HOUGHPOINT_CXX 1
#include <cmath>
#include <iostream>
#include <vector>

class HoughPoint {
 public:
    double x;
    double y;
    double z;
    int layerID;
    int trackID;
    int eventID;
    double _pt;
    int _id;

    HoughPoint();
    HoughPoint(double posX, double posY, double posZ, int eventID, int trackID,
               int layerID);
    HoughPoint(double posX, double posY, double posZ, int eventID, int trackID,
               int layerID, double pt);
    inline double xConformal() const;
    inline double yConformal() const;
    inline double Pt() const;
    inline void SetPt(double pt);
    inline int id() const;
    inline void SetId(int identity);
    void TrackFlagChange(int flag);
    void TrackFlagAddOne();
    void TrackFlagReset();
    void Print() const;
};
HoughPoint::HoughPoint(double x, double y, double z, int event, int track,
                       int layer)
    : x(x),
      y(y),
      z(z),
      layerID(layer),
      trackID(track),
      eventID(event),
      _pt(0),
      _id(-1) {}
HoughPoint::HoughPoint(double x, double y, double z, int event, int track,
                       int layer, double pt)
    : x(x),
      y(y),
      z(z),
      layerID(layer),
      trackID(track),
      eventID(event),
      _pt(pt),
      _id(-1) {}
inline double HoughPoint::xConformal() const { return 2 * x / (x * x + y * y); }
inline double HoughPoint::yConformal() const { return 2 * y / (y * y + x * x); }
inline int HoughPoint::id() const { return _id; }
inline void HoughPoint::SetId(int identity) { _id = identity; }
inline void HoughPoint::SetPt(double pt) { _pt = pt; }
inline double HoughPoint::Pt() const { return _pt; }
void HoughPoint::Print() const {
    std::cout << "identity: " << _id << " "
              << "Pt: " << _pt << " "
              << "eventID: " << eventID << " "
              << "layerID: " << layerID << " "
              << "trackID: " << trackID << " " << x << " " << y << " " << z
              << std::endl;
}
#endif