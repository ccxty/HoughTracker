#include "TMath.h"
#ifndef __HOUGHGLOBAL_CXX_INCLUDE__
#define __HOUGHGLOBAL_CXX_INCLUDE__

namespace Hough {
constexpr double AlphaMin = 0;
constexpr double AlphaMax = TMath::Pi();
constexpr double RhoMin = -0.05;
constexpr double RhoMax = 0.05;
constexpr int NAlpha = 100;
constexpr int NRho = 100;
constexpr double AlphaBinWidth = (AlphaMax - AlphaMin) / NAlpha;
constexpr double RhoBinWidth = (RhoMax - RhoMin) / NRho;
}  // namespace Hough

#endif