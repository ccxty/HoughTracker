#include "TMath.h"
#ifndef __HOUGHGLOBAL_CXX_INCLUDE__
#define __HOUGHGLOBAL_CXX_INCLUDE__ 1

namespace Hough {
constexpr double AlphaMin = 0;
constexpr double AlphaMax = TMath::Pi();
constexpr double RhoMin = -0.05;
constexpr double RhoMax = 0.05;
constexpr int NumAlpha = 100;
constexpr int NumD = 100;
constexpr double AlphaBinWidth = (AlphaMax - AlphaMin) / NumAlpha;
constexpr double RhoBinWidth = (RhoMax - RhoMin) / NumD;
}  // namespace Hough

#endif