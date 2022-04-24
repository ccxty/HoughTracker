#include <array>
#include <cmath>
#include <memory>
#include <random>
#include <set>
#include <tuple>

#include "HitPoint.h"
#include "TMath.h"
#include "TRandom3.h"
#include "Track.h"

void GetCircle2P(HitPoint &point1, HitPoint &point2, double Radius,
                 std::array<Point2D, 2> &center) {
    double k = 0, k_vertical = 0;
    double x_mid = 0, y_mid = 0;
    double a = 1;
    double b = 1;
    double c = 1;

    k = (point2.y - point1.y) / (point2.x - point1.x);
    k_vertical = -1 / k;
    x_mid = (point1.x + point2.x) / 2;
    y_mid = (point1.y + point2.y) / 2;
    a = 1 + k_vertical * k_vertical;
    b = -2 * x_mid - k_vertical * k_vertical * 2 * x_mid;
    c = x_mid * x_mid + k_vertical * k_vertical * x_mid * x_mid -
        (Radius * Radius - ((x_mid - point1.x) * (x_mid - point1.x) +
                            (y_mid - point1.y) * (y_mid - point1.y)));
    double x1 = (-b + sqrt(b * b - 4 * a * c)) / (2 * a);
    double x2 = (-b - sqrt(b * b - 4 * a * c)) / (2 * a);
    center[0].x = x1;
    center[1].x = x2;
    center[0].y = (k_vertical * center[0].x - k_vertical * x_mid + y_mid);
    center[1].y = (k_vertical * center[1].x - k_vertical * x_mid + y_mid);
}

void GetCircle3P(HitPoint &p0, HitPoint &p1, HitPoint &p2, Point2D &center) {
    double k1 = -(p1.x - p0.x) / (p1.y - p0.y);
    Point2D mid1{(p1.x + p0.x) / 2, (p1.y + p0.y) / 2};
    double k2 = -(p2.x - p1.x) / (p2.y - p1.y);
    Point2D mid2{(p2.x + p1.x) / 2, (p2.y + p1.y) / 2};
    center.x = -(mid1.y - mid2.y - mid1.x * k1 + mid2.x * k2) / (k1 - k2);
    center.y =
        -(-mid2.y * k1 + mid1.y * k2 + (mid2.x - mid1.x) * k1 * k2) / (k1 - k2);
}

double DeltaPt(double pt) { return 0.0001 * pt * pt; }

void Merge(std::vector<Track *> &tracks, Track *other) {
    for (auto &track : tracks) {
    }
}