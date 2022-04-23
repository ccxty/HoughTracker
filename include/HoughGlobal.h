#include "TMath.h"
#ifndef __HOUGHGLOBAL_CXX_INCLUDE__
#define __HOUGHGLOBAL_CXX_INCLUDE__ 1

namespace Hough {
constexpr double AlphaMin = 0;
constexpr double AlphaMax = TMath::Pi();
constexpr double DMin = -0.05;
constexpr double DMax = 0.05;
constexpr int NumAlpha = 100;
constexpr int NumD = 100;
constexpr double AlphaBinWidth = (AlphaMax - AlphaMin) / NumAlpha;
constexpr double DBinWidth = (DMax - DMin) / NumD;
}  // namespace Hough

#endif