#ifndef HOUGH_CXX_INCLUDE_
#define HOUGH_CXX_INCLUDE_ 1
#include "HoughPoint.h"

const double AlphaMin = 0;
const double AlphaMax = TMath::Pi();
const double DMin = -0.05;
const double DMax = 0.05;
const int NAlpha = 100;
const int ND = 100;
const double AlphaBinWidth = (AlphaMax - AlphaMin) / NAlpha;
const double DBinWidth = (DMax - DMin) / ND;

const double MagFeild = 1.0;                       // 1.0 T
const double PtMin = 0.3 * MagFeild * 165.11 / 2.; // 能击中三层探测器动量条件
const double QCut = 1.;

void TherePointsLinearFit(HoughPoint *point1, HoughPoint *point2, HoughPoint *point3, double *Q, double *param)
{
    double x[3] = {point1->xConformal(), point2->xConformal(), point3->xConformal()};
    double y[3] = {point1->yConformal(), point2->yConformal(), point3->yConformal()};
    double sum_x = 0;
    double sum_y = 0;
    double sum_xx = 0;
    double sum_xy = 0;
    int counts = 3;
    for (int i = 0; i < counts; i++)
    {
        sum_x += x[i];
        sum_y += y[i];
        sum_xx += x[i] * x[i];
        sum_xy += x[i] * y[i];
    }
    double a1 = (counts * sum_xy - sum_x * sum_y) / (counts * sum_xx - sum_x * sum_x);
    double a0 = (sum_y - a1 * sum_x) / counts;
    *Q = 0;
    for (int i = 0; i < counts; i++)
    {
        *Q += (y[i] - a0 - a1 * x[i]) * (y[i] - a0 - a1 * x[i]);
    }
    *Q = *Q / counts;
    param[0] = a0;
    param[1] = a1;
}

void FitZLinear(HoughPoint *point1, HoughPoint *point2, HoughPoint *point3, double *Qz)
{
    double r[3] = {sqrt(point1->x() * point1->x() + point1->y() * point1->y()),
                   sqrt(point2->x() * point2->x() + point2->y() * point2->y()),
                   sqrt(point3->x() * point3->x() + point3->y() * point3->y())};
    double z[3] = {point1->z(), point2->z(), point3->z()};
    double sum_x = 0;
    double sum_y = 0;
    double sum_xx = 0;
    double sum_xy = 0;
    int counts = 3;
    for (int i = 0; i < counts; i++)
    {
        sum_x += r[i];
        sum_y += z[i];
        sum_xx += r[i] * r[i];
        sum_xy += r[i] * z[i];
    }
    double a1 = (counts * sum_xy - sum_x * sum_y) / (counts * sum_xx - sum_x * sum_x);
    double a0 = (sum_y - a1 * sum_x) / counts;
    *Qz = 0;
    for (int i = 0; i < counts; i++)
    {
        *Qz += (z[i] - a0 - a1 * r[i]) * (z[i] - a0 - a1 * r[i]);
    }
    *Qz = *Qz / counts;
}
#endif