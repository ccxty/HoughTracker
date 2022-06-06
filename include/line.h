#include <array>
#include <cmath>
#include <numeric>

#include "HitPoint.h"
#include "global.h"
#ifndef __LINE_CXX_INCLUDED__
#define __LINE_CXX_INCLUDED__

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
void LinearFit(std::array<double, N> &x, std::array<double, N> &y, double *Q,
               StraightLine &line) {
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
void TherePointsLinearFit(HitPoint *point1, HitPoint *point2, HitPoint *point3,
                          double *Q, StraightLine &line);

void FitZLinear(HitPoint *point1, HitPoint *point2, HitPoint *point3,
                double Radius, double *Qz, StraightLine &line);

void FitZLinear(HitPoint *point1, HitPoint *point2, HitPoint *point3,
                double *Qz, StraightLine &line);

void GetParams2P(HitPoint &point1, HitPoint &point2, double Radius,
                 std::array<TrackParameters, 2> &params);

void GetParams3P(HitPoint &p0, HitPoint &p1, HitPoint &p2,
                 TrackParameters &param);

#endif