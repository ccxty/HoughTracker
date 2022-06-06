#include "HitPoint.h"

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
double HitPoint::xConformal() const { return 2 * x / (x * x + y * y); }
double HitPoint::yConformal() const { return 2 * y / (y * y + x * x); }
double HitPoint::xyDistance(const HitPoint* other) const {
    return sqrt((x - other->x) * (x - other->x) +
                (y - other->y) * (y - other->y));
}
double HitPoint::Phi() const {
    double phi = atan2(z, sqrt(x * x + y * y));
    if (phi < 0) {
        phi += 2 * M_PI;
    }
    return phi;
}
int HitPoint::id() const { return _id; }
HitPoint& HitPoint::SetId(int identity) {
    _id = identity;
    return *this;
}
HitPoint& HitPoint::SetPt(double pt) {
    _pt = pt;
    return *this;
}
double HitPoint::Pt() const { return _pt; }
void HitPoint::Print() const {
    std::cout << "identity: " << _id << " "
              << "Pt: " << _pt << " "
              << "eventID: " << eventID << " "
              << "layerID: " << layerID << " "
              << "trackID: " << trackID << " " << x << " " << y << " " << z
              << "\n";
}
