#include <array>
#include <cmath>
#include <numeric>
#ifndef __LINE_INCLUDE_CXX__
#define __LINE_INCLUDE_CXX__ 1
#include "HoughPoint.h"
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
                      double *Q, Polynomial<2> &line) {
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
inline void TherePointsLinearFit(HoughPoint *point1, HoughPoint *point2,
                                 HoughPoint *point3, double *Q,
                                 Polynomial<2> &line) {
    std::array<double, 3> posX = {point1->xConformal(), point2->xConformal(),
                                  point3->xConformal()};
    std::array<double, 3> posY = {point1->yConformal(), point2->yConformal(),
                                  point3->yConformal()};
    LinearFit<3>(posX, posY, Q, line);
}

inline void FitZLinear(HoughPoint *point1, HoughPoint *point2,
                       HoughPoint *point3, double Radius, double *Qz,
                       Polynomial<2> &line) {
    std::array<double, 3> r_xy = {
        sqrt(point1->x * point1->x + point1->y * point1->y),
        sqrt(point2->x * point2->x + point2->y * point2->y),
        sqrt(point3->x * point3->x + point3->y * point3->y)};
    std::array<double, 3> s_xy = {2 * Radius * asin(r_xy[0] / (2 * Radius)),
                                  2 * Radius * asin(r_xy[1] / (2 * Radius)),
                                  2 * Radius * asin(r_xy[2] / (2 * Radius))};
    std::array<double, 3> posZ = {point1->z, point2->z, point3->z};
    LinearFit<3>(s_xy, posZ, Qz, line);
}
#endif