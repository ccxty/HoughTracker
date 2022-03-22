#ifndef HOUGH_CXX_INCLUDE_
#define HOUGH_CXX_INCLUDE_ 1
#include "HoughPoint.h"

void TherePointsLinearFit(HoughPoint *point1, HoughPoint *point2, HoughPoint *point3, double *Q, double *param)
{
    double x[3] = {point1->xConformal(), point2->xConformal(), point3->xConformal()};
    double y[3] = {point1->yConformal(), point2->yConformal(), point3->yConformal()};
    double sum_x = 0;
    double sum_y = 0;
    double sum_xx = 0;
    double sum_xy = 0;
    int _counts = 3;
    for (int i = 0; i < _counts; i++)
    {
        sum_x += x[i];
        sum_y += y[i];
        sum_xx += x[i] * x[i];
        sum_xy += x[i] * y[i];
    }
    double a1 = (_counts * sum_xy - sum_x * sum_y) / (_counts * sum_xx - sum_x * sum_x);
    double a0 = (sum_y - a1 * sum_x) / _counts;
    *Q = 0;
    for (int i = 0; i < _counts; i++)
    {
        *Q += (y[i] - a0 - a1 * x[i]) * (y[i] - a0 - a1 * x[i]);
    }
    *Q = *Q / _counts;
    param[0] = a0;
    param[1] = a1;
}
#endif