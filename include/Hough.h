#include <array>
#include <cmath>
#include <iostream>
#include <memory>
#include <numeric>

#include "TRandom3.h"
#ifndef __HOUGH_CXX_INCLUDE__
#define __HOUGH_CXX_INCLUDE__

#include "HitPoint.h"
#include "HoughGlobal.h"
#include "HoughGridArea.h"
#include "TMath.h"
#include "Track.h"
#include "global.h"

namespace Hough {

using Tracks = std::vector<Track>;

template <int NAlpha, int NRho>
using GridMatrix =
    std::array<std::array<std::unique_ptr<HoughGridArea>, NRho>, NAlpha>;

template <int NAlpha, int NRho>
auto GridInit() {
    using std::array;
    using std::unique_ptr;
    auto matrix = GridMatrix<NAlpha, NRho>();
    for (int i = 0; i < NAlpha; i++) {
        for (int j = 0; j < NRho; j++) {
            auto ptr = std::make_unique<HoughGridArea>(
                AlphaMin + i * AlphaBinWidth,                           //
                RhoMin + j * RhoBinWidth * (1 - Overlap),               //
                RhoMin + j * RhoBinWidth * (1 - Overlap) + RhoBinWidth  //
            );
            matrix[i][j] = std::move(ptr);
        }
    }
    return std::move(matrix);
}

template <int NAlpha, int NRho>
void FillGrid(GridMatrix<NAlpha, NRho> &matrix, Points &points) {
    for (auto *point : points) {
        for (auto &row : matrix) {
            double alpha = row[0]->xMid();
            double rho = point->xConformal() * cos(alpha) +
                         point->yConformal() * sin(alpha);
            double q = floor((rho - RhoMin) / (RhoBinWidth * (1 - Overlap)));
            int index = floor(q);
            if (index == 0) {
                row[0]->AddPoint(point);
            } else {  // assume Overlap <= 0.5
                row[index]->AddPoint(point);
                if (q - index - 1 / (1 - Overlap) < -1) {
                    row[index - 1]->AddPoint(point);
                }
            }
        }
    }
}

template <int NAlpha, int NRho>
Tracks FindTrack(GridMatrix<NAlpha, NRho> &matrix) {
    using std::make_unique;
    using std::unique_ptr;
    using std::vector;
    auto ptr = Tracks();
    for (int ia = 0; ia < NAlpha; ia++) {
        for (int id = 0; id < NRho; id++) {
            auto &grid = matrix[ia][id];
            int counts22 = grid->counts();
            if (counts22 >= 3) {
                auto points = grid->GetPointsHere();
                if ((ia > 0) && (id > 0) && (ia + 1 < NAlpha) &&
                    (id + 1 < NRho)) {
                    int counts11 = matrix[ia - 1][id - 1]->counts();
                    int counts12 = matrix[ia - 1][id]->counts();
                    int counts13 = matrix[ia - 1][id + 1]->counts();
                    int counts21 = matrix[ia][id - 1]->counts();
                    int counts23 = matrix[ia][id + 1]->counts();
                    int counts31 = matrix[ia + 1][id - 1]->counts();
                    int counts32 = matrix[ia + 1][id]->counts();
                    int counts33 = matrix[ia + 1][id + 1]->counts();
                    int flag = (counts22 >= counts11) + (counts22 >= counts12) +
                               (counts12 >= counts13) + (counts22 >= counts21) +
                               (counts22 >= counts23) + (counts21 >= counts31) +
                               (counts22 >= counts32) + (counts22 >= counts33);
                    // if (  //(counts22 >= counts11) &&  //
                    //     (counts22 >= counts12) &&  //
                    //     //(counts22 >= counts13) &&  //
                    //     (counts22 >= counts21) &&  //
                    //     (counts22 >= counts23) &&  //
                    //     //(counts22 >= counts31) &&  //
                    //     (counts22 >= counts32) &&  //
                    //     //(counts22 >= counts33)     //
                    if (flag >= 5) {
                        auto ptr_temp = Track(points);
                        if (ptr.empty()) {
                            ptr.push_back(std::move(ptr_temp));
                        } else {
                            bool is_equal = false;
                            for (auto &existingTrack : ptr) {
                                if (existingTrack.operator==(ptr_temp)) {
                                    is_equal = true;
                                    break;
                                }
                            }
                            if (!is_equal && (ptr_temp.HitALayers())) {
                                ptr.push_back(std::move(ptr_temp));
                            }
                        }
                    }
                }
            }
        }
    }
    return std::move(ptr);
}

void MergeTracks(Tracks &origin, Tracks &add);

}  // namespace Hough

#endif