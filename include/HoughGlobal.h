#include "TMath.h"
#ifndef __HOUGHGLOBAL_CXX_INCLUDE__
#define __HOUGHGLOBAL_CXX_INCLUDE__ 1

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

#endif