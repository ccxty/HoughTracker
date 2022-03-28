#ifndef HOUGHPOINT_CXX
#define HOUGHPOINT_CXX 1
#include <cmath>
#include <iostream>
#include <vector>

class HoughPoint {
 private:
    double _x;
    double _y;
    double _z;
    int _layerID;
    int _trackID;
    int _eventID;
    double _xConformal;
    double _yConformal;
    double _pt;
    int _id;

 public:
    HoughPoint();
    HoughPoint(double x, double y, double z, int eventID, int trackID,
               int layerID);
    HoughPoint(double x, double y, double z, int eventID, int trackID,
               int layerID, double Pt);
    inline double x() const;
    inline double y() const;
    inline double z() const;
    inline double xConformal() const;
    inline double yConformal() const;
    inline int layerID() const;
    inline int trackID() const;
    inline int eventID() const;
    inline double Pt() const;
    inline void SetPt(double pt);
    inline int id() const;
    inline void SetId(int id);
    void TrackFlagChange(int flag);
    void TrackFlagAddOne();
    void TrackFlagReset();
    void Print() const;
};
HoughPoint::HoughPoint(double x, double y, double z, int event, int track,
                       int layer)
    : _x(x),
      _y(y),
      _z(z),
      _layerID(layer),
      _trackID(track),
      _eventID(event),
      _xConformal(2 * x / (x * x + y * y)),
      _yConformal(2 * y / (y * y + x * x)),
      _pt(0),
      _id(-1) {}
HoughPoint::HoughPoint(double x, double y, double z, int event, int track,
                       int layer, double pt)
    : _x(x),
      _y(y),
      _z(z),
      _layerID(layer),
      _trackID(track),
      _eventID(event),
      _xConformal(2 * x / (x * x + y * y)),
      _yConformal(2 * y / (y * y + x * x)),
      _pt(pt),
      _id(-1) {}
inline double HoughPoint::x() const { return _x; }
inline double HoughPoint::y() const { return _y; }
inline double HoughPoint::z() const { return _z; }
inline double HoughPoint::xConformal() const { return _xConformal; }
inline double HoughPoint::yConformal() const { return _yConformal; }
inline int HoughPoint::eventID() const { return _eventID; }
inline int HoughPoint::trackID() const { return _trackID; }
inline int HoughPoint::layerID() const { return _layerID; }
inline int HoughPoint::id() const { return _id; }
inline void HoughPoint::SetId(int id) { _id = id; }
inline void HoughPoint::SetPt(double pt) { _pt = pt; }
inline double HoughPoint::Pt() const { return _pt; }
void HoughPoint::Print() const {
    std::cout << "identity: " << _id << " "
              << "Pt: " << _pt << " "
              << "eventID: " << _eventID << " "
              << "layerID: " << _layerID << " "
              << "trackID: " << _trackID << " " << _x << " " << _y << " " << _z
              << " " << _xConformal << " " << _yConformal << std::endl;
}
#endif