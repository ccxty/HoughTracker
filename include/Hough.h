#include <array>
#include <cmath>
#include <memory>
#include <numeric>

#include "TRandom3.h"
#ifndef HOUGH_CXX_INCLUDE_
#define HOUGH_CXX_INCLUDE_ 1

#include "HitPoint.h"
#include "HoughGlobal.h"
#include "HoughGridArea.h"
#include "TMath.h"
#include "Track.h"
#include "global.h"

namespace Hough {
using HoughGrid =
    std::vector<std::unique_ptr<std::vector<std::unique_ptr<HoughGridArea>>>>;
using Tracks = std::vector<std::unique_ptr<Track>>;

template <int NAlpha, int NRho>
using GridMatrix =
    std::array<std::array<std::unique_ptr<HoughGridArea>, NRho>, NAlpha>;

template <int NAlpha, int NRho>
auto GridInit(double shift) {  // shift == 0.25 is good for single gridmatrix
    using std::array;
    using std::unique_ptr;
    auto martix = GridMatrix<NAlpha, NRho>();
    for (int i = 0; i < NAlpha; i++) {
        for (int j = 0; j < NRho; j++) {
            auto ptr = std::make_unique<HoughGridArea>(
                AlphaMin + i * AlphaBinWidth,
                RhoMin + (j - shift) * RhoBinWidth,       // shift
                RhoMin + (j + shift + 1) * RhoBinWidth);  // shift
            martix[i][j] = std::move(ptr);
        }
    }
    return std::move(martix);
}

template <int NAlpha, int NRho>
void FillGrid(GridMatrix<NAlpha, NRho> &matrix, Points &points) {
    for (auto *point : points) {
        for (auto &row : matrix) {
            for (auto &grid : row) {
                double alpha = grid->xMid();
                double rho = point->xConformal() * cos(alpha) +
                             point->yConformal() * sin(alpha);

                if ((rho >= grid->yMin()) && (rho < grid->yMax())) {
                    grid->CountsAddOne();
                    grid->GetPointsHere().push_back(point);
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
            if (int counts22 = grid->counts() >= 3) {
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
                    if ((counts22 >= counts11) && (counts22 >= counts12) &&
                        (counts22 >= counts13) && (counts22 >= counts21) &&
                        (counts22 >= counts23) && (counts22 >= counts31) &&
                        (counts22 >= counts32) && (counts22 >= counts33)) {
                        auto ptr_temp = make_unique<Track>(points);
                        if (ptr.empty()) {
                            ptr.push_back(move(ptr_temp));
                        } else {
                            bool is_equal = false;
                            for (auto &existingTrack : ptr) {
                                if (existingTrack->operator==(*ptr_temp)) {
                                    is_equal = true;
                                    break;
                                }
                            }
                            if (!is_equal && (ptr_temp->HitALayers())) {
                                ptr.push_back(move(ptr_temp));
                            }
                        }
                    }
                }
            }
        }
    }
    return std::move(ptr);
}

Tracks MergeTracks(Tracks &tracks1, Tracks &tracks2) {
    for (auto &track : tracks2) {
        bool contain = false;
        for (auto &existingTrack : tracks1) {
            if (existingTrack->operator>=(*track)) {
                contain = true;
                break;
            }
        }
        if (!contain) {
            tracks1.push_back(move(track));
        }
    }
    return std::move(tracks1);
}

}  // namespace Hough

#endif