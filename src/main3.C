#include <TVector3.h>
#include <unistd.h>

#include <algorithm>
#include <array>
#include <cmath>
#include <fstream>
#include <iostream>
#include <memory>
#include <random>
#include <set>
#include <vector>

#include "Hough.h"
#include "HoughGridArea.h"
#include "HoughPoint.h"
#include "HoughTrack.h"
#include "TFile.h"
#include "TMath.h"
#include "TRandom3.h"
#include "TTree.h"
#include "clipp.h"

using std::array;
using std::cerr;
using std::cout;
using std::endl;
using std::string;
using std::unique_ptr;
using std::vector;
using namespace clipp;

// not to use
void find_peak(vector<vector<HoughGridArea *> *> *gridMatrix) {
    int flag = 0;
    for (auto row : *gridMatrix) {
        for (auto grid : *row) {
            if (grid->counts() >= 3) {
                auto points = grid->GetPointsHere();
                for (auto point : *points) {
                    // std::cout << flag << " " << grid->counts() << " "
                    //           << point->eventID() << " " << point->layerID()
                    //           << " " << ia << " " << id << endl;
                }
                flag++;
            }
        }
    }
}

vector<HoughTrack *> *find_track(
    std::vector<std::vector<HoughGridArea *> *> *gridMatrix) {
    auto ptr = new std::vector<HoughTrack *>;
    for (int ia = 0; ia < gridMatrix->size(); ia++) {
        auto row = gridMatrix->at(ia);
        for (int id = 0; id < row->size(); id++) {
            auto grid = row->at(id);
            if (grid->counts() >= 3) {
                auto points = grid->GetPointsHere();
                // for (int ip = 0; ip < points->size(); ip++) {
                //     auto point = points->at(ip);
                //     // cout << flag1 << " " << grid->counts() << " "
                //     //      << point->eventID() << " " << point->layerID()
                //     //      << " " << ia << " " << id << endl;
                // }
                if ((ia > 0) && (id > 0) && (ia + 1 < gridMatrix->size()) &&
                    (id + 1 < row->size())) {
                    int counts1 = gridMatrix->at(ia - 1)->at(id - 1)->counts();
                    int counts2 = gridMatrix->at(ia - 1)->at(id)->counts();
                    int counts3 = gridMatrix->at(ia - 1)->at(id + 1)->counts();
                    int counts4 = gridMatrix->at(ia)->at(id - 1)->counts();
                    int counts5 = gridMatrix->at(ia)->at(id + 1)->counts();
                    int counts6 = gridMatrix->at(ia + 1)->at(id - 1)->counts();
                    int counts7 = gridMatrix->at(ia + 1)->at(id)->counts();
                    int counts8 = gridMatrix->at(ia + 1)->at(id + 1)->counts();
                    if ((grid->counts() >= counts1) &&
                        (grid->counts() >= counts2) &&
                        (grid->counts() >= counts3) &&
                        (grid->counts() >= counts4) &&
                        (grid->counts() >= counts5) &&
                        (grid->counts() >= counts6) &&
                        (grid->counts() >= counts7) &&
                        (grid->counts() >= counts8)) {
                        auto ptr_temp = new HoughTrack(points);
                        if (ptr->size() == 0) {
                            ptr->push_back(ptr_temp);
                            // ptr_temp->Print();
                        } else {
                            bool is_equal = false;
                            for (auto existingTrack : *ptr) {
                                if (existingTrack->operator==(ptr_temp)) {
                                    is_equal = true;
                                    break;
                                }
                            }
                            if (!is_equal && (ptr_temp->HitALayers())) {
                                ptr->push_back(ptr_temp);
                                // ptr_temp->Print();
                            }
                        }
                    }
                }
            }
        }
    }
    // std::cout << " Num of tracks: " << ptr->size() << std::endl;
    return ptr;
}

/**
 * @brief Initialize the grids
 *
 * @param NAlpha Grid number in the alpha direction, default NumAlpha
 * @param NRho Grid number in the rho direction, default NumD
 * @return vector<vector<HoughGridArea *> *>
 */
auto GridInit(const int NAlpha = NumAlpha, const int NRho = NumD) {
    auto ptr1 = vector<vector<HoughGridArea *> *>();
    for (int i = 0; i < NAlpha; i++) {
        auto ptr2 = new vector<HoughGridArea *>;
        for (int j = 0; j < NRho; j++) {
            auto ptr3 =
                new HoughGridArea(AlphaMin + i * AlphaBinWidth,
                                  DMin + (j - 0.25) * DBinWidth,   // shift
                                  DMin + (j + 0.75) * DBinWidth);  // shift
            ptr2->push_back(ptr3);
        }
        ptr1.push_back(ptr2);
    }
    // std::cout << "Init completed" << std::endl;
    return ptr1;
}

/**
 * @brief Fill the grids (Hough transformation)
 *
 * @param gridMatrix the grids
 * @param pointsList all points, including true points and noise
 */
void FillGrid(std::vector<std::vector<HoughGridArea *> *> *gridMatrix,
              vector<HoughPoint *> &pointsList) {
    for (auto point : pointsList) {
        for (auto row : *gridMatrix) {
            for (auto grid : *row) {
                double alpha = grid->xMid();
                double rho = point->xConformal() * cos(alpha) +
                             point->yConformal() * sin(alpha);

                if ((rho >= grid->yMin()) && (rho < grid->yMax())) {
                    // cout << d << endl;
                    grid->CountsAddOne();
                    grid->GetPointsHere()->push_back(point);
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
    } else {
        auto rdm = TRandom3();
        auto rdm_layer = TRandom3();
        auto rdm_z = TRandom3();
        std::random_device rd_device;
        array<double, 3> radius = {65.115, 115.11, 165.11};
        auto len = points.size();
        rdm.SetSeed(rd_device() % kMaxULong);
        rdm_layer.SetSeed(rd_device() % kMaxULong);
        rdm_z.SetSeed(rd_device() % kMaxULong);

        for (int i = 0; i < n_noise; i++) {
            auto layerID = rdm_layer.Integer(3);
            double posX = NAN, posY = NAN, posZ = NAN;
            rdm.Circle(posX, posY, radius[layerID]);
            posZ = (radius[layerID] / tan(20 * TMath::Pi() / 180.)) *
                   (-1 + 2 * rdm_z.Rndm());
            auto point = new HoughPoint(posX, posY, posZ, -1, 1,
                                        static_cast<int>(layerID), 0);
            point->SetId(static_cast<int>(len) + i);
            points.push_back(point);
        }
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
    auto set_test = unique_ptr<std::set<int>>(new std::set<int>);
    while (set_test->size() < n_tracks_in_event) {
        auto iter(base.begin());
        advance(iter, rand() % base.size());
        set_test->insert(*iter);
    }
    return set_test;
}

int main(int argc, char **argv) {
    //
    // Commandline Arguments Parser with include/clipp.h
    //
    enum class mode { all, single, help };
    mode selected = mode::help;

    string pt_str, noise_str, particle, n_track_str;

    auto pt_arg = required("-pt") & value("pt", pt_str);
    auto noise_arg = required("-noise") & value("noise", noise_str);
    auto particle_arg = required("-particle") & value("particle", particle);
    auto n_track_arg = required("-multi") & value("num-tracks", n_track_str);
    auto all_mode =
        (command("all").set(selected, mode::all),
         pt_arg % "Pt of the data file", noise_arg % "number of noise points",
         particle_arg % "the particle stored in the data file",
         n_track_arg % "number of tracks in single event");
    auto single_mode =
        (command("single").set(selected, mode::single),
         pt_arg % "Pt of the data file", noise_arg % "number of noise points",
         particle_arg % "the particle stored in the data file",
         n_track_arg % "number of tracks in single event");
    auto cli =
        ((all_mode | single_mode | command("help").set(selected, mode::help)),
         option("-v", "--version")
             .call([] { cout << "version 1.0\n\n"; })
             .doc("show version"));
    if (parse(argc, argv, cli)) {
        switch (selected) {
            case mode::all:
                cout << "all mode" << endl;
                break;
            case mode::single:
                cout << "single mode" << endl;
                break;
            case mode::help:
                cout << make_man_page(cli, "HoughTracker");
                return 0;
        }
    } else {
        cout << usage_lines(cli, "HoughTracker") << '\n';
        return 0;
    }

    // string path =
    //     "/home/txiao/STCF_Oscar2.0.0/share/pi+/test2/root_data_source/";
    string path =
        "/home/ubuntu-tyxiao/work/STCF_Oscar2.0.0/HoughTracker/"
        "root_data_source/";
    path += particle + "/posPt";
    path += pt_str;
    path += ".root";

    double Pt_data = atof(pt_str.c_str());
    int n_noise = atoi(noise_str.c_str());
    int n_tracks_in_event = atoi(n_track_str.c_str());

    auto *file = new TFile(path.c_str());
    if (!file->IsOpen()) {
        cerr << "File not found" << endl;
        return 0;
    }
    TTree *tree = dynamic_cast<TTree *>(gDirectory->Get("tree1"));
    const Long64_t nevents = tree->GetEntries();

    unique_ptr<std::set<int>> eventIDs_toTest;
    std::set<int> eventIDs_all;
    for (auto i = 0; i < nevents; i++) {
        eventIDs_all.insert(i);
    }
    if (selected == mode::single) {
        eventIDs_toTest = GetRandomSet(n_tracks_in_event, eventIDs_all);
    } else if (selected == mode::all) {
        eventIDs_toTest.reset(&eventIDs_all);
    }

    std::vector<double> *posX = nullptr;
    std::vector<double> *posY = nullptr;
    std::vector<double> *posZ = nullptr;
    std::vector<int> *trackID = nullptr;
    std::vector<int> *layerID = nullptr;
    std::vector<double> *P_t = nullptr;
    TBranch *b_posX = nullptr;
    TBranch *b_posY = nullptr;
    TBranch *b_posZ = nullptr;
    TBranch *b_trackID = nullptr;
    TBranch *b_Pt = nullptr;
    TBranch *b_layerID = nullptr;
    int eventID = 0;
    int npoints = 0, nhits = 0;
    tree->SetBranchAddress("posX", &posX, &b_posX);
    tree->SetBranchAddress("posY", &posY, &b_posY);
    tree->SetBranchAddress("posZ", &posZ, &b_posZ);
    tree->SetBranchAddress("eventID", &eventID);
    tree->SetBranchAddress("layerID", &layerID, &b_layerID);
    tree->SetBranchAddress("trackID", &trackID, &b_trackID);
    tree->SetBranchAddress("Pt", &P_t, &b_Pt);
    tree->SetBranchAddress("nhits", &nhits);

    string savepath = "./data/" + particle + "/trackdata_Pt" + pt_str +
                      "_noise" + string(noise_str);
    if (n_tracks_in_event != 0) {
        savepath += "_multi" + n_track_str;
    }
    savepath += ".root";
    auto savefile = new TFile(savepath.c_str(), "RECREATE");
    auto savetree = new TTree("tree1", "tree1");
    int event_id = 0, track_id = 0, num_true = 0, num_total = 0, Q_e = 0;
    double Q_min, pt, Qz;
    bool true_track = false;
    savetree->Branch("event_id", &event_id);
    savetree->Branch("track_id", &track_id);
    savetree->Branch("true_track", &true_track);
    savetree->Branch("Pt", &pt);
    savetree->Branch("Q", &Q_min);
    savetree->Branch("Qz", &Qz);
    savetree->Branch("num_true", &num_true);
    savetree->Branch("num_total", &num_total);
    savetree->Branch("Qe", &Q_e);
    int counts_useful_events = 0;
    std::cout << path << endl;
    // std::cout << Pt_data << endl;
    while (eventIDs_toTest->size() >= n_tracks_in_event) {
        std::cout << "There are " << eventIDs_toTest->size()
                  << " events left\n";
        // int eventIDTest;
        auto test_set = GetRandomSet(n_tracks_in_event, *eventIDs_toTest);
        for (auto event : *test_set) {
            eventIDs_toTest->erase(event);
        }
        std::vector<HoughPoint *> pointsList;
        int eventID_temp = -1;
        int eventID_skip = -1;
        for (auto i_event : *test_set) {
            tree->GetEntry(i_event);
            if (nhits < 3) {
                continue;
            }
            int read_count = 0;
            counts_useful_events++;
            for (int ip = 0; ip < nhits; ip++) {
                if ((trackID->at(ip) == 1) && (eventID != eventID_skip)) {
                    auto *ptr = new HoughPoint(
                        posX->at(ip), posY->at(ip), posZ->at(ip), eventID,
                        trackID->at(ip), layerID->at(ip), P_t->at(ip));
                    ptr->SetId(static_cast<int>(pointsList.size()));
                    pointsList.push_back(ptr);
                    read_count++;
                }
                if (read_count >= 3) {
                    eventID_skip = eventID;
                }
            }
        }
        if (pointsList.size() < 3) {
            continue;
        }
        AddNoise(n_noise, pointsList);
        npoints = static_cast<int>(pointsList.size());

        // std::cout << "number of points: " << npoints << endl;

        auto houghGrid = GridInit();
        FillGrid(&houghGrid, pointsList);
        auto tracks = find_track(&houghGrid);
        int track_id_re = 0;

        double Qmin = 1.;
        int n_good_tracks = 0;
        for (auto track : *tracks) {
            double p_t = NAN, Q_xy = NAN, Q_z = NAN;
            if (track->HitALayers()) {
                // track->Print();
                // std::cout << boolalpha << track->ContainTrueTrack() << " "
                //           << noboolalpha << track->RatioTrues() << std::endl;
                bool fit_fine = track->FitLinear(&p_t, &Q_xy, &Q_z);

                if (fit_fine && (p_t > PtMin) && (Q_xy < QCut)) {
                    event_id = track->GetEventID(test_set.get());

                    track_id = track_id_re;
                    track_id_re++;
                    true_track = track->ContainTrueTrackMulti(test_set.get());
                    pt = p_t;
                    Q_min = Q_xy;
                    Qz = Q_z;
                    num_true = track->NumTruePointsMulti(test_set.get());
                    num_total = static_cast<int>(track->Counts());
                    Q_e = track->GetSpin();
                    savefile->cd();
                    savetree->Fill();

                    n_good_tracks++;
                    if (selected == mode::single) {
                        std::ofstream out1("tracks.txt", std::ios::app);
                        out1 << std::boolalpha << true_track << "\t";
                        for (auto point : *track->GetPoints()) {
                            out1 << point->id() << "\t";
                        }
                        out1 << "\n";
                    }
                }
            }
        }
        if (selected == mode::single) {
            std::ofstream out2("points.txt", std::ios::app);
            for (auto point : pointsList) {
                out2 << point->eventID() << "\t" << point->id() << "\t"
                     << point->x() << "\t" << point->y() << "\t" << point->z()
                     << "\n";
            }
        }
        // std::cout << "number of good tracks: " << n_good_tracks << std::endl;

        //
        //
        // delete
        for (auto ptr : pointsList) {
            delete ptr;
        }
        for (auto row : houghGrid) {
            for (auto grid : *row) {
                delete grid;
            }
            delete row;
        }
    }
    savefile->cd();
    savetree->Write();
    savefile->Write();
    savefile->Close();
    // delete file;
    // delete tree;
    // delete savetree;
    // delete savefile;
    std::cout << "Save Path: " << savepath << std::endl;
    std::cout << "total tracks useful: " << counts_useful_events << endl
              << endl;
    return 0;
}