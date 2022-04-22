#include <array>
#include <cmath>
#include <memory>
#include <numeric>
#include <random>
#include <set>
#include <tuple>

#include "TRandom3.h"
#ifndef HOUGH_CXX_INCLUDE_
#define HOUGH_CXX_INCLUDE_ 1

#include "HoughGlobal.h"
#include "HoughGridArea.h"
#include "HoughPoint.h"
#include "HoughTrack.h"
#include "TMath.h"

using HoughGrid =
    std::vector<std::unique_ptr<std::vector<std::unique_ptr<HoughGridArea>>>>;

// not to use
void find_peak(HoughGrid &gridMatrix) {
    int flag = 0;
    for (auto &row : gridMatrix) {
        for (auto &grid : *row) {
            if (grid->counts() >= 3) {
                auto points = grid->GetPointsHere();
                for (auto *point : points) {
                    // std::cout << flag << " " << grid->counts() << " "
                    //           << point->eventID() << " " << point->layerID()
                    //           << " " << ia << " " << id << endl;
                }
                flag++;
            }
        }
    }
}

std::vector<std::unique_ptr<HoughTrack>> find_track(HoughGrid &gridMatrix) {
    using std::make_unique;
    using std::unique_ptr;
    using std::vector;
    auto ptr = std::vector<unique_ptr<HoughTrack>>();
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
                        auto ptr_temp = make_unique<HoughTrack>(points);
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
                DMin + (j - 0.25) * DBinWidth,   // shift
                DMin + (j + 0.75) * DBinWidth);  // shift
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
void FillGrid(HoughGrid &gridMatrix, std::vector<HoughPoint *> &pointsList) {
    for (auto *point : pointsList) {
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

/**
 * @brief Add noise to existing pointsVector
 *
 * @param n_noise Number of noise points in all 3 layers
 * @param points Vector existing to add the noise points
 */
void AddNoise(int n_noise, std::vector<HoughPoint *> &points) {
    if (n_noise <= 0) {
        return;
    }
    auto rdm = TRandom3();
    auto rdm_layer = TRandom3();
    auto rdm_z = TRandom3();
    std::random_device rd_device;
    std::array<double, 3> radius = {65.115, 115.11, 165.11};
    auto len = points.size();
    rdm.SetSeed(rd_device() % kMaxULong);
    rdm_layer.SetSeed(rd_device() % kMaxULong);
    rdm_z.SetSeed(rd_device() % kMaxULong);

    for (int i = 0; i < n_noise; i++) {
        int layerID = static_cast<int>(rdm_layer.Integer(3));
        double posX = NAN, posY = NAN, posZ = NAN;
        rdm.Circle(posX, posY, radius[layerID]);
        posZ = (radius[layerID] / tan(20 * TMath::Pi() / 180.)) *
               (-1 + 2 * rdm_z.Rndm());
        auto *point = new HoughPoint(posX, posY, posZ, -1, 1, layerID, 0);
        point->SetId(static_cast<int>(len) + i);
        points.push_back(point);
    }
}

/**
 * @brief Get the Random eventID Set to test
 *
 * @param n_tracks_in_event Number of tracks in single test
 * @param base Set of all eventID
 * @return unique_ptr<std::set<int>>
 */
auto GetRandomSet(int n_tracks_in_event, std::set<int> &base) {
    auto set_test = std::make_unique<std::set<int>>();
    while (set_test->size() < n_tracks_in_event) {
        auto iter(base.begin());
        advance(iter, rand() % base.size());
        set_test->insert(*iter);
    }
    return set_test;
}

#endif