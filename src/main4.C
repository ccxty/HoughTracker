#include <algorithm>
#include <fstream>
#include <iostream>
#include <memory>
#include <random>
#include <set>
#include <vector>

#include "HitPoint.h"
#include "TFile.h"
#include "TMath.h"
#include "TRandom3.h"
#include "TTree.h"
#include "Track.h"
#include "TreeRead.h"
#include "args.h"
#include "clipp.h"
#include "global.h"

using std::cerr;
using std::cout;
using std::endl;
using std::set;
using std::shared_ptr;
using std::string;
using std::unique_ptr;
using std::vector;

// sigma(Pt)/Pt ~= 1.0e-4 * Pt
// sigma(Pt) ~= 1.0e-4* Pt * Pt
// 20 -> 0.04 ; 100 -> 1 ; 135 -> 1.82 ; 200 -> 4
int main(int argc, char **argv) {
    /**
     * @brief Commandline Arguments Parser with include/clipp.h
     */
    Args args;
    args_parse(argc, argv, args);
    args_out_json(args);
    double Pt_data = atof(args.pt_str.c_str());
    int n_noise = atoi(args.n_noise_str.c_str());
    int n_tracks_in_event = atoi(args.n_track_str.c_str());

    /**
     *  @brief Initialize the source data
     */
    // string path =
    //     "/home/txiao/STCF_Oscar2.0.0/share/pi+/test2/root_data_source/";
    string path =
        "/home/ubuntu-tyxiao/work/STCF_Oscar2.0.0/HoughTracker/"
        "root_data_source/";
    path += args.particle + "/posPt";
    path += args.pt_str;
    path += ".root";
    TreeData data = TreeData(path.c_str());
    if (data.isEmpty()) {
        cerr << "Data not found" << endl;
        return 0;
    }

    /**
     * @brief Initialize the test eventIDs
     */
    const Long64_t nevents = data.GetEntries();
    unique_ptr<set<int>> eventIDs_toTest;
    set<int> eventIDs_all;
    for (auto i = 0; i < nevents; i++) {
        eventIDs_all.insert(i);
    }
    if (args.mode == ExecMode::single) {
        eventIDs_toTest = GetRandomSet(n_tracks_in_event, eventIDs_all);
    } else if (args.mode == ExecMode::all) {
        eventIDs_toTest.reset(&eventIDs_all);
    }

    /**
     *  @brief Initialize the output file
     */
    string savepath = "./data/" + args.particle + "/trackdata_Pt" +
                      args.pt_str + "_noise" + args.n_noise_str;
    if (n_tracks_in_event != 0) {
        savepath += "_multi" + args.n_track_str;
    }
    savepath += ".root";
    TFile savefile(savepath.c_str(), "RECREATE");
    TTree savetree("tree1", "tree1");
    int event_id = 0, track_id = 0, num_true = 0, num_total = 0, Q_e = 0;
    double Q_min = NAN, pt = NAN, Qz = NAN;
    bool true_track = false;
    int counts_useful_events = 0;
    savetree.Branch("event_id", &event_id);
    savetree.Branch("track_id", &track_id);
    savetree.Branch("true_track", &true_track);
    savetree.Branch("Pt", &pt);
    savetree.Branch("Q", &Q_min);
    savetree.Branch("Qz", &Qz);
    savetree.Branch("num_true", &num_true);
    savetree.Branch("num_total", &num_total);
    savetree.Branch("Qe", &Q_e);
    cout << path << endl;

    /**
     * @brief Get true points from source data
     */
    while (eventIDs_toTest->size() >= n_tracks_in_event) {
        std::cout << "There are " << eventIDs_toTest->size()
                  << " events left\n";
        // int eventIDTest;
        auto test_set = GetRandomSet(n_tracks_in_event, *eventIDs_toTest);
        for (auto event : *test_set) {
            eventIDs_toTest->erase(event);
        }
        std::vector<HitPoint *> pointsList;
        int eventID_temp = -1;
        int eventID_skip = -1;
        for (auto i_event : *test_set) {
            data.GetEntry(i_event);
            int nhits = data.NHits();
            if (nhits < 3) {
                continue;
            }
            int read_count = 0;
            counts_useful_events++;
            for (int ip = 0; ip < nhits; ip++) {
                if ((data.TrackID()->at(ip) == 1) &&
                    (data.EventID() != eventID_skip)) {
                    auto *ptr =
                        new HitPoint(data.PosX()->at(ip), data.PosY()->at(ip),
                                     data.PosZ()->at(ip), data.EventID(),
                                     data.TrackID()->at(ip),
                                     data.LayerID()->at(ip), data.Pt()->at(ip));
                    ptr->SetId(static_cast<int>(pointsList.size()));
                    pointsList.push_back(ptr);
                    read_count++;
                }
                if (read_count >= 3) {
                    eventID_skip = data.EventID();
                }
            }
        }
        if (pointsList.size() < 3) {
            continue;
        }
        /**
         * @brief add noise points
         */
        AddNoise(n_noise, pointsList);
        int npoints = static_cast<int>(pointsList.size());
        // std::cout << "number of points: " << npoints << endl;

        /**
         * @brief Hough transform
         *
         */
        std::vector<Track> tracks;
        int track_id_re = 0;
        for (const auto &track : tracks) {
            event_id = track.GetEventID(test_set.get());
            track_id = track_id_re;
            track_id_re++;
            true_track = track.ContainTrueTrackMulti(test_set.get());
            pt = track.Pt();
            Qz = track.Qz();
            Q_e = 0;
            Q_min = track->DOrigin();
            num_true = track.NumTruePointsMulti(test_set.get());
            // savefile->cd();
            savetree.Fill();
        }
    }
    savefile.cd();
    savetree.Write();
    savefile.Write();
    savefile.Close();
    cout << "Save Path: " << savepath << endl;
    cout << "total tracks useful: " << counts_useful_events << endl << endl;
    return 0;
}
