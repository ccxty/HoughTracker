#include <array>
#include <cmath>
#include <numeric>
#ifndef __LINE_CXX_INCLUDED__
#define __LINE_CXX_INCLUDED__
#include "HitPoint.h"
#include "global.h"
template <int N>
struct Line {
    std::array<double, N> x;
    std::array<double, N> y;
};

// y=a0+a1*x+...+a_{n-1}*x^{n-1}
template <int N>
struct Polynomial {
    int degree = N;
    std::array<double, N> eff;

    Polynomial();
    double value(double x);
};

using StraightLine = Polynomial<2>;
using Quadratic = Polynomial<3>;

template <int N>
Polynomial<N>::Polynomial() {
    for (int i = 0; i < N; i++) {
        eff[i] = 0.0;
    }
}

template <int N>
double Polynomial<N>::value(double x) {
    double result = 0;
    for (int i = 0; i < N; i++) {
        result += eff[i] * pow(x, i);
    }
    return result;
}

/**
 * @brief LinearFit with N points
 *
 * @tparam N number of points
 * @param x x coordinate
 * @param y y coordinate
 * @param Q sqrt(Q/(N - 2))
 * @param line fit result
 */
template <int N>
inline void LinearFit(std::array<double, N> &x, std::array<double, N> &y,
                      double *Q, StraightLine &line) {
    double sum_x = std::accumulate(x.begin(), x.end(), 0.0);
    double sum_y = std::accumulate(y.begin(), y.end(), 0.0);
    double sum_xx = 0;
    double sum_xy = 0;
    for (int i = 0; i < N; i++) {
        sum_xx += x[i] * x[i];
        sum_xy += x[i] * y[i];
    }
    double a1 = (N * sum_xy - sum_x * sum_y) / (N * sum_xx - sum_x * sum_x);
    double a0 = (sum_y - a1 * sum_x) / N;
    *Q = 0;
    for (int i = 0; i < N; i++) {
        *Q += (y[i] - a0 - a1 * x[i]) * (y[i] - a0 - a1 * x[i]);
    }
    *Q = sqrt(*Q / (N - 2));
    line.eff[0] = a0;
    line.eff[1] = a1;
}

/**
 * @brief Linear fit algorithm with three points, y=a1*x+a0
 *
 * @param point1 point
 * @param point2 point
 * @param point3 point
 * @param Q sqrt(Q/(N-2))
 * @param param Polynomial<2>
 *
 */
inline void TherePointsLinearFit(HitPoint *point1, HitPoint *point2,
                                 HitPoint *point3, double *Q,
                                 StraightLine &line) {
    std::array<double, 3> posX = {point1->xConformal(), point2->xConformal(),
                                  point3->xConformal()};
    std::array<double, 3> posY = {point1->yConformal(), point2->yConformal(),
                                  point3->yConformal()};
    LinearFit<3>(posX, posY, Q, line);
}

inline void FitZLinear(HitPoint *point1, HitPoint *point2, HitPoint *point3,
                       double Radius, double *Qz, StraightLine &line) {
    std::array<double, 3> r_xy = {InnerDetectorR[point1->layerID],
                                  InnerDetectorR[point2->layerID],
                                  InnerDetectorR[point3->layerID]};
    std::array<double, 3> s_xy = {2 * Radius * asin(r_xy[0] / (2 * Radius)),
                                  2 * Radius * asin(r_xy[1] / (2 * Radius)),
                                  2 * Radius * asin(r_xy[2] / (2 * Radius))};
    std::array<double, 3> posZ = {point1->z, point2->z, point3->z};
    LinearFit<3>(s_xy, posZ, Qz, line);
}

inline void FitZLinear(HitPoint *point1, HitPoint *point2, HitPoint *point3,
                       double *Qz, StraightLine &line) {
    std::array<double, 3> r_xy = {InnerDetectorR[point1->layerID],
                                  InnerDetectorR[point2->layerID],
                                  InnerDetectorR[point3->layerID]};
    std::array<double, 3> posZ = {point1->z, point2->z, point3->z};
    LinearFit<3>(r_xy, posZ, Qz, line);
}

void GetParams2P(HitPoint &point1, HitPoint &point2, double Radius,
                 std::array<TrackParameters, 2> &params) {
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
    params[0].center.x = x1;
    params[1].center.x = x2;
    params[0].center.y =
        (k_vertical * params[0].center.x - k_vertical * x_mid + y_mid);
    params[1].center.y =
        (k_vertical * params[1].center.x - k_vertical * x_mid + y_mid);
}

void GetParams3P(HitPoint &p0, HitPoint &p1, HitPoint &p2,
                 TrackParameters &param) {
    double k1 = -(p1.x - p0.x) / (p1.y - p0.y);
    Point2D mid1{(p1.x + p0.x) / 2, (p1.y + p0.y) / 2};
    double k2 = -(p2.x - p1.x) / (p2.y - p1.y);
    Point2D mid2{(p2.x + p1.x) / 2, (p2.y + p1.y) / 2};
    param.center.x = -(mid1.y - mid2.y - mid1.x * k1 + mid2.x * k2) / (k1 - k2);
    param.center.y =
        -(-mid2.y * k1 + mid1.y * k2 + (mid2.x - mid1.x) * k1 * k2) / (k1 - k2);
}

#endif