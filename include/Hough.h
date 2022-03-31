#ifndef HOUGH_CXX_INCLUDE_
#define HOUGH_CXX_INCLUDE_ 1
#include "HoughPoint.h"
#include "TMath.h"

const double AlphaMin = 0;
const double AlphaMax = TMath::Pi();
const double DMin = -0.05;
const double DMax = 0.05;
const int NumAlpha = 100;
const int NumD = 100;
const double AlphaBinWidth = (AlphaMax - AlphaMin) / NumAlpha;
const double DBinWidth = (DMax - DMin) / NumD;

const double MagFeild = 1.0;                        // 1.0 T
const double PtMin = 0.3 * MagFeild * 165.11 / 2.;  // 能击中三层探测器动量条件
const double QCut = 1.;

void TherePointsLinearFit(HoughPoint *point1, HoughPoint *point2,
                          HoughPoint *point3, double *Q, double *param) {
    double posX[3] = {point1->xConformal(), point2->xConformal(),
                      point3->xConformal()};
    double posY[3] = {point1->yConformal(), point2->yConformal(),
                      point3->yConformal()};
    double sum_x = 0;
    double sum_y = 0;
    double sum_xx = 0;
    double sum_xy = 0;
    int counts = 3;
    for (int i = 0; i < counts; i++) {
        sum_x += posX[i];
        sum_y += posY[i];
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
    param[0] = param_a0;
    param[1] = param_a1;
}

void FitZLinear(HoughPoint *point1, HoughPoint *point2, HoughPoint *point3,
                double R, double *Q_z) {
    double l[3] = {sqrt(point1->x() * point1->x() + point1->y() * point1->y()),
                   sqrt(point2->x() * point2->x() + point2->y() * point2->y()),
                   sqrt(point3->x() * point3->x() + point3->y() * point3->y())};
    double s[3] = {2 * R * asin(l[0] / (2 * R)), 2 * R * asin(l[1] / (2 * R)),
                   2 * R * asin(l[2] / (2 * R))};
    double posZ[3] = {point1->z(), point2->z(), point3->z()};
    double sum_x = 0;
    double sum_y = 0;
    double sum_xx = 0;
    double sum_xy = 0;
    int counts = 3;
    for (int i = 0; i < counts; i++) {
        sum_x += s[i];
        sum_y += posZ[i];
        sum_xx += s[i] * s[i];
        sum_xy += s[i] * posZ[i];
    }
    double param_a1 =
        (counts * sum_xy - sum_x * sum_y) / (counts * sum_xx - sum_x * sum_x);
    double param_a0 = (sum_y - param_a1 * sum_x) / counts;
    *Q_z = 0;
    for (int i = 0; i < counts; i++) {
        *Q_z += (posZ[i] - param_a0 - param_a1 * s[i]) *
                (posZ[i] - param_a0 - param_a1 * s[i]);
    }
    *Q_z = *Q_z / counts;
}
#endif