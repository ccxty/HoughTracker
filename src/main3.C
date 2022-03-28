#include <TVector3.h>
#include <unistd.h>

#include <algorithm>
#include <array>
#include <fstream>
#include <iostream>
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
using std::cout;
using std::endl;
using std::string;
using std::vector;
using namespace clipp;

void find_peak(std::vector<std::vector<HoughGridArea *> *> *gridMatrix) {
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

std::vector<HoughTrack *> *find_track(
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
                            for (auto exsitingTrack : *ptr) {
                                if (exsitingTrack->operator==(ptr_temp)) {
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

std::vector<std::vector<HoughGridArea *> *> *GridInit() {
    auto ptr1 = new vector<vector<HoughGridArea *> *>;
    for (int i = 0; i < NumAlpha; i++) {
        auto ptr2 = new vector<HoughGridArea *>;
        for (int j = 0; j < NumD; j++) {
            auto ptr3 = new HoughGridArea(AlphaMin + i * AlphaBinWidth,
                                          DMin + j * DBinWidth,
                                          DMin + (j + 1) * DBinWidth);
            ptr2->push_back(ptr3);
        }
        ptr1->push_back(ptr2);
    }
    // std::cout << "Init completed" << std::endl;
    return ptr1;
}

void FillGrid(std::vector<std::vector<HoughGridArea *> *> *gridMatrix,
              vector<HoughPoint *> &pointsList) {
    for (auto point : pointsList) {
        for (auto row : *gridMatrix) {
            for (auto grid : *row) {
                double alpha = grid->xMid();
                double d = point->xConformal() * cos(alpha) +
                           point->yConformal() * sin(alpha);

                if ((d >= grid->yMin()) && (d < grid->yMax())) {
                    // cout << d << endl;
                    grid->CountsAddOne();
                    grid->GetPointsHere()->push_back(point);
                }
            }
        }
        // std::cout << ip << " of " << pointsList.size() << endl;
    }
}

void AddNoise(int n_noise, std::vector<HoughPoint *> &points) {
    if (n_noise <= 0) {
        return;
    } else {
        auto rdm = new TRandom3();
        auto rdm_layer = new TRandom3();
        auto rdm_z = new TRandom3();
        std::random_device rd_device;
        array<double, 3> radius = {65.115, 115.11, 165.11};
        auto len = points.size();
        rdm->SetSeed(rd_device() % kMaxULong);
        rdm_layer->SetSeed(rd_device() % kMaxULong);
        rdm_z->SetSeed(rd_device() % kMaxULong);
        double cos_20 = cos(20 * TMath::Pi() / 180.);

        for (int i = 0; i < n_noise; i++) {
            auto layerID = rdm_layer->Integer(3);
            double posX, posY, posZ, cos_theta;
            rdm->Circle(posX, posY, radius[layerID]);
            cos_theta = rdm_z->Rndm() * (2 * cos_20) - cos_20;
            posZ =
                radius[layerID] * cos_theta / sqrt(1 - cos_theta * cos_theta);
            auto point = new HoughPoint(posX, posY, posZ, -1, 1,
                                        static_cast<int>(layerID), 0);
            point->SetId(static_cast<int>(len) + i - 1);
            points.push_back(point);
        }
        delete rdm;
        delete rdm_layer;
        delete rdm_z;
    }
}

std::set<int> *GetRandomSet(int n_tracks_in_event, std::set<int> &base) {
    auto set = new std::set<int>;
    while (set->size() < n_tracks_in_event) {
        auto iter(base.begin());
        advance(iter, rand() % base.size());
        set->insert(*iter);
    }
    return set;
}

// bool SetContain(std::set<int> *set, int num)
// {
//     bool contain = false;
//     for (auto e : *set)
//     {
//         if (e == num)
//             contain = true;
//     }
//     return contain;
// }

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
         n_track_arg % "number of tracks in sigle event");
    auto single_mode =
        (command("single").set(selected, mode::single),
         pt_arg % "Pt of the data file", noise_arg % "number of noise points",
         particle_arg % "the particle stored in the data file",
         n_track_arg % "number of tracks in sigle event");
    auto cli =
        ((all_mode | single_mode | command("help").set(selected, mode::help)),
         option("-v", "--version")
             .call([] { cout << "version 1.0\n\n"; })
             .doc("show version"));
    if (parse(argc, argv, cli)) {
        switch (selected) {
            case mode::all:
                break;
            case mode::single:
                break;
            case mode::help:
                cout << make_man_page(cli, "HoughTracker");
                return 0;
        }
    } else {
        cout << usage_lines(cli, "HoughTracker") << '\n';
        return 0;
    }
    // ./program particle pt n_noise mode multi
    // argv:0        1    2     3     4     5
    // string particle(argv[1]);
    double Pt_data = atof(pt_str.c_str());
    // string path =
    //     "/home/txiao/STCF_Oscar2.0.0/share/pi+/test2/root_data_source/";
    string path =
        "/home/ubuntu-tyxiao/work/STCF_Oscar2.0.0/HoughTracker/"
        "root_data_source/";
    path += particle + "/posPt";
    path += pt_str;
    path += ".root";

    int n_noise = atoi(noise_str.c_str());
    int n_tracks_in_event = atoi(n_track_str.c_str());

    auto *file = new TFile(path.c_str());
    TTree *tree = dynamic_cast<TTree *>(gDirectory->Get("tree1"));
    const Long64_t nevents = tree->GetEntries();

    std::set<int> *eventIDs_toTest = nullptr;
    std::set<int> eventIDs_all;
    for (auto i = 0; i < nevents; i++) {
        eventIDs_all.insert(i);
    }
    if (selected == mode::single) {
        eventIDs_toTest = GetRandomSet(n_tracks_in_event, eventIDs_all);
    } else if (selected == mode::all) {
        eventIDs_toTest = &eventIDs_all;
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
    int eventID;
    int npoints, nhits;
    tree->SetBranchAddress("posX", &posX, &b_posX);
    tree->SetBranchAddress("posY", &posY, &b_posY);
    tree->SetBranchAddress("posZ", &posZ, &b_posZ);
    tree->SetBranchAddress("eventID", &eventID);
    tree->SetBranchAddress("layerID", &layerID, &b_layerID);
    tree->SetBranchAddress("trackID", &trackID, &b_trackID);
    tree->SetBranchAddress("Pt", &P_t, &b_Pt);
    tree->SetBranchAddress("nhits", &nhits);

    string savepath =
        "./trackdata_Pt" + string(argv[2]) + "_noise" + string(argv[3]);
    if (n_tracks_in_event != 0) {
        savepath += "_multi" + string(argv[5]);
    }
    savepath += ".root";
    auto savefile = new TFile(savepath.c_str(), "RECREATE");
    auto savetree = new TTree("tree1", "tree1");
    int event_id, track_id, num_true, num_total, Q_e;
    double Q_min, p_t, Q_z;
    bool true_track;
    savetree->Branch("event_id", &event_id);
    savetree->Branch("track_id", &track_id);
    savetree->Branch("true_track", &true_track);
    savetree->Branch("Pt", &p_t);
    savetree->Branch("Q", &Q_min);
    savetree->Branch("Qz", &Q_z);
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
        FillGrid(houghGrid, pointsList);
        auto tracks = find_track(houghGrid);
        std::vector<double> PtReContruction;
        std::vector<double> QReContruction;

        double Qmin = 1.;
        int n_good_tracks = 0;
        for (auto track : *tracks) {
            double p_t, Q_xy, Q_z;
            if (track->HitALayers()) {
                // track->Print();
                // std::cout << boolalpha << track->ContainTrueTrack() << " "
                //           << noboolalpha << track->RatioTrues() << std::endl;
                bool fit_fine = track->FitLinear(&p_t, &Q_xy, &Q_z);

                if (fit_fine && (p_t > PtMin) && (Q_xy < QCut)) {
                    event_id = track->GetEventID(test_set);
                    PtReContruction.push_back(p_t);
                    QReContruction.push_back(Q_xy);

                    track_id = static_cast<int>(PtReContruction.size());
                    true_track = track->ContainTrueTrackMulti(test_set);
                    p_t = p_t;
                    Q_min = Q_xy;
                    Q_z = Q_z;
                    num_true = track->NumTruePointsMulti(test_set);
                    num_total = static_cast<int>(track->Counts());
                    Q_e = track->GetSpin();
                    savefile->cd();
                    savetree->Fill();

                    n_good_tracks++;
                }
            }
        }
        // std::cout << "number of good tracks: " << n_good_tracks << std::endl;

        //
        //
        // delete
        for (auto ptr : pointsList) {
            delete ptr;
        }
        for (int i = 0; i < NumAlpha; i++) {
            for (int j = 0; j < NumD; j++) {
                delete houghGrid->at(i)->at(j);
            }
            delete houghGrid->at(i);
        }
        delete houghGrid;
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
