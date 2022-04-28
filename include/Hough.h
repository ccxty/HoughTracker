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

template <int NAlpha, int NRho>
using GridMatrix =
    std::array<std::array<std::unique_ptr<HoughGridArea>, NRho>, NAlpha>;

std::vector<std::unique_ptr<Track>> FindTrack(HoughGrid &gridMatrix) {
    using std::make_unique;
    using std::unique_ptr;
    using std::vector;
    auto ptr = std::vector<unique_ptr<Track>>();
    for (int ia = 0; ia < gridMatrix.size(); ia++) {
        auto &row = gridMatrix.at(ia);
        for (int id = 0; id < row->size(); id++) {
            auto &grid = row->at(id);
            if (grid->counts() >= 3) {
                auto points = grid->GetPointsHere();
                if ((ia > 0) && (id > 0) && (ia + 1 < gridMatrix.size()) &&
                    (id + 1 < row->size())) {
                    int counts1 = gridMatrix.at(ia - 1)->at(id - 1)->counts();
                    int counts2 = gridMatrix.at(ia - 1)->at(id)->counts();
                    int counts3 = gridMatrix.at(ia - 1)->at(id + 1)->counts();
                    int counts4 = gridMatrix.at(ia)->at(id - 1)->counts();
                    int counts5 = gridMatrix.at(ia)->at(id + 1)->counts();
                    int counts6 = gridMatrix.at(ia + 1)->at(id - 1)->counts();
                    int counts7 = gridMatrix.at(ia + 1)->at(id)->counts();
                    int counts8 = gridMatrix.at(ia + 1)->at(id + 1)->counts();
                    if ((grid->counts() >= counts1) &&
                        (grid->counts() >= counts2) &&
                        (grid->counts() >= counts3) &&
                        (grid->counts() >= counts4) &&
                        (grid->counts() >= counts5) &&
                        (grid->counts() >= counts6) &&
                        (grid->counts() >= counts7) &&
                        (grid->counts() >= counts8)) {
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
    // std::cout << " Num of tracks: " << ptr->size() << std::endl;
    return move(ptr);
}

/**
 * @brief Initialize the grids
 *
 * @param NAlpha Grid number in the alpha direction, default NumAlpha
 * @param NRho Grid number in the rho direction, default NumD
 * @return vector<vector<HoughGridArea *> *>
 */
auto GridInit(const int NAlpha = NumAlpha, const int NRho = NumD) {
    using std::make_unique;
    using std::unique_ptr;
    using std::vector;
    auto ptr1 = vector<unique_ptr<vector<unique_ptr<HoughGridArea>>>>();
    for (int i = 0; i < NAlpha; i++) {
        auto ptr2 = make_unique<vector<unique_ptr<HoughGridArea>>>();
        for (int j = 0; j < NRho; j++) {
            auto ptr3 = make_unique<HoughGridArea>(
                AlphaMin + i * AlphaBinWidth,
                RhoMin + (j - 0.25) * DBinWidth,   // shift
                RhoMin + (j + 0.75) * DBinWidth);  // shift
            ptr2->push_back(move(ptr3));
        }
        ptr1.push_back(move(ptr2));
    }
    // std::cout << "Init completed" << std::endl;
    return move(ptr1);
}

/**
 * @brief Fill the grids (Hough transformation)
 *
 * @param gridMatrix the grids
 * @param pointsList all points, including true points and noise
 */
void FillGrid(HoughGrid &gridMatrix, Points &points) {
    for (auto *point : points) {
        for (auto &row : gridMatrix) {
            for (auto &grid : *row) {
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
auto GridInit() {
    using std::array;
    using std::unique_ptr;
    auto martix = GridMatrix<NAlpha, NRho>();
    for (int i = 0; i < NAlpha; i++) {
        for (int j = 0; j < NRho; j++) {
            auto ptr = std::make_unique<HoughGridArea>(
                AlphaMin + i * AlphaBinWidth,
                RhoMin + (j - 0.25) * DBinWidth,   // shift
                RhoMin + (j + 0.75) * DBinWidth);  // shift
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
std::vector<std::unique_ptr<Track>> FindTrack(
    GridMatrix<NAlpha, NRho> &matrix) {
    using std::make_unique;
    using std::unique_ptr;
    using std::vector;
    auto ptr = std::vector<unique_ptr<Track>>();
    for (int ia = 0; ia < NAlpha; ia++) {
        for (int id = 0; id < NRho; id++) {
            auto &grid = matrix[ia][id];
            if (grid->counts() >= 3) {
                auto points = grid->GetPointsHere();
                if ((ia > 0) && (id > 0) && (ia + 1 < NAlpha) &&
                    (id + 1 < NRho)) {
                    int counts1 = matrix[ia - 1][id - 1]->counts();
                    int counts2 = matrix[ia - 1][id]->counts();
                    int counts3 = matrix[ia - 1][id + 1]->counts();
                    int counts4 = matrix[ia][id - 1]->counts();
                    int counts5 = matrix[ia][id + 1]->counts();
                    int counts6 = matrix[ia + 1][id - 1]->counts();
                    int counts7 = matrix[ia + 1][id]->counts();
                    int counts8 = matrix[ia + 1][id + 1]->counts();
                    if ((grid->counts() >= counts1) &&
                        (grid->counts() >= counts2) &&
                        (grid->counts() >= counts3) &&
                        (grid->counts() >= counts4) &&
                        (grid->counts() >= counts5) &&
                        (grid->counts() >= counts6) &&
                        (grid->counts() >= counts7) &&
                        (grid->counts() >= counts8)) {
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
}  // namespace Hough

#endif