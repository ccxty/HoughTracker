#include <array>
#include <cmath>
#include <numeric>
#include <tuple>
#ifndef HOUGH_CXX_INCLUDE_
#define HOUGH_CXX_INCLUDE_ 1

#include "HoughPoint.h"
#include "TMath.h"

constexpr double AlphaMin = 0;
constexpr double AlphaMax = TMath::Pi();
constexpr double DMin = -0.05;
constexpr double DMax = 0.05;
constexpr int NumAlpha = 100;
constexpr int NumD = 100;
constexpr double AlphaBinWidth = (AlphaMax - AlphaMin) / NumAlpha;
constexpr double DBinWidth = (DMax - DMin) / NumD;

constexpr double MagFeild = 1.0;                        // 1.0 T
constexpr double PtMin = 0.3 * MagFeild * 165.11 / 2.;  // 击中三层动量条件
constexpr double QCut = 1.;
constexpr double QzCut = 2.;

/**
 * @brief Linear fit algorithm with three points, y=a1*x+a0
 *
 * @param point1 point
 * @param point2 point
 * @param point3 point
 * @param Q Q/3
 * @param param tuple<a0, a1>
 *
 */
void TherePointsLinearFit(HoughPoint *point1, HoughPoint *point2,
                          HoughPoint *point3, double *Q,
                          std::tuple<double, double> &param) {
    std::array<double, 3> posX = {point1->xConformal(), point2->xConformal(),
                                  point3->xConformal()};
    std::array<double, 3> posY = {point1->yConformal(), point2->yConformal(),
                                  point3->yConformal()};
    double sum_x = std::accumulate(posX.begin(), posX.end(), 0.0);
    double sum_y = std::accumulate(posY.begin(), posY.end(), 0.0);
    double sum_xx = 0;
    double sum_xy = 0;
    int counts = 3;
    for (int i = 0; i < counts; i++) {
        sum_xx += posX[i] * posX[i];
        sum_xy += posX[i] * posY[i];
    }
    double param_a1 =
        (counts * sum_xy - sum_x * sum_y) / (counts * sum_xx - sum_x * sum_x);
    double param_a0 = (sum_y - param_a1 * sum_x) / counts;
    *Q = 0;
    for (int i = 0; i < counts; i++) {
        *Q += (posY[i] - param_a0 - param_a1 * posX[i]) *
              (posY[i] - param_a0 - param_a1 * posX[i]);
    }
    *Q = *Q / counts;
    std::get<0>(param) = param_a0;
    std::get<1>(param) = param_a1;
}

void FitZLinear(HoughPoint *point1, HoughPoint *point2, HoughPoint *point3,
                double Radius, double *Q_z, std::tuple<double, double> &param) {
    std::array<double, 3> r_xy = {
        sqrt(point1->x * point1->x + point1->y * point1->y),
        sqrt(point2->x * point2->x + point2->y * point2->y),
        sqrt(point3->x * point3->x + point3->y * point3->y)};
    std::array<double, 3> s_xy = {2 * Radius * asin(r_xy[0] / (2 * Radius)),
                                  2 * Radius * asin(r_xy[1] / (2 * Radius)),
                                  2 * Radius * asin(r_xy[2] / (2 * Radius))};
    std::array<double, 3> posZ = {point1->z, point2->z, point3->z};
    double sum_x = std::accumulate(s_xy.begin(), s_xy.end(), 0.0);
    double sum_y = std::accumulate(posZ.begin(), posZ.end(), 0.0);
    double sum_xx = 0;
    double sum_xy = 0;
    int counts = 3;
    for (int i = 0; i < counts; i++) {
        sum_xx += s_xy[i] * s_xy[i];
        sum_xy += s_xy[i] * posZ[i];
    }

    double param_a1 =
        (counts * sum_xy - sum_x * sum_y) / (counts * sum_xx - sum_x * sum_x);
    double param_a0 = (sum_y - param_a1 * sum_x) / counts;
    *Q_z = 0;
    for (int i = 0; i < counts; i++) {
        *Q_z += (posZ[i] - param_a0 - param_a1 * s_xy[i]) *
                (posZ[i] - param_a0 - param_a1 * s_xy[i]);
    }
    *Q_z = *Q_z / counts;
    std::get<0>(param) = param_a0;
    std::get<1>(param) = param_a1;
}
#endif