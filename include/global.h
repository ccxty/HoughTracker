#include <random>
#include <set>

#include "HitPoint.h"
#include "TMath.h"
#include "TRandom3.h"
#ifndef __GLOBAL_CXX_INCLUDE__
#define __GLOBAL_CXX_INCLUDE__

constexpr double MagFeild = 1.0;                        // 1.0 T
constexpr double PtMin = 0.3 * MagFeild * 165.11 / 2.;  // 击中三层动量条件
constexpr double QCut = 0.1;
constexpr double QzCut = 2.;
constexpr std::array<double, 3> InnerTrackerR = {65.115, 115.11, 165.11};
// layer0: (25.6796, 59.8374)
// layer1: (80.2514, 82.5229)
// layer2: (165.11, 0)
// layer0 -> layer1: 59.1
// layer1 -> layer2: 118.368
// layer0 -> layer2: 151.728
constexpr double DMin01 = 59.1;
constexpr double DMin12 = 118.368;
constexpr double DMin02 = 151.728;

constexpr double DeltaPhi01 = 0.1;
constexpr double DeltaPhi12 = 0.1;
constexpr double DeltaZ01 = 10;
constexpr double DeltaZ12 = 10;

constexpr double DeltaR01 = InnerTrackerR[1] - InnerTrackerR[0];
constexpr double DeltaR12 = InnerTrackerR[2] - InnerTrackerR[1];

constexpr double E_Z = 0.4;     // uRWELL:    400um
constexpr double E_RPhi = 0.1;  // uRWELL:    100um

using Points = std::vector<HitPoint *>;

struct TrackParameters {
    double R = 0;
    Point2D center = {0, 0};
    double kz = 0;
};

/**
 * @brief Add noise to existing pointsVector
 *
 * @param n_noise Number of noise points in all 3 layers
 * @param points Vector existing to add the noise points
 */
void ITKAddNoise(int n_noise, Points &points);

/**
 * @brief Get the Random eventID Set to test
 *
 * @param n_tracks_in_event Number of tracks in single test
 * @param base Set of all eventID
 * @return unique_ptr<std::set<int>>
 */
std::unique_ptr<std::set<int>> GetRandomSet(int n_tracks_in_event,
                                            std::set<int> &base);

#endif