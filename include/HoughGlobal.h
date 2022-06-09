#include "TMath.h"
#ifndef __HOUGH_GLOBAL_CXX_INCLUDE__
#define __HOUGH_GLOBAL_CXX_INCLUDE__

namespace Hough {

constexpr double AlphaMin = 0;
constexpr double AlphaMax = TMath::Pi();
constexpr double RhoMin = -0.05;
constexpr double RhoMax = 0.05;
constexpr int NAlpha = 100;
constexpr int NRho = 199;
constexpr double Overlap = 0.5;  //重叠区域占网格单元长度的比例
constexpr double AlphaBinWidth = (AlphaMax - AlphaMin) / NAlpha;
constexpr double RhoBinWidth =
    (RhoMax - RhoMin) / ((1 - Overlap) * NRho + Overlap);

}  // namespace Hough

#endif