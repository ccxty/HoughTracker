#include <cmath>

#include "HitPoint.h"
#include "Track.h"
#include "TreeRead.h"
#include "args.h"
#include "global.h"

using std::cerr;
using std::cout;
using std::endl;
using std::ofstream;
using std::set;
using std::shared_ptr;
using std::string;
using std::unique_ptr;
using std::vector;

constexpr double DeltaPhi = 0.1;

inline Track find_track(HitPoint *point, const Points &points) {
    Track track(point);
    double phi = point->Phi();
    for (auto *other : points) {
        double phi2 = other->Phi();
        bool z_filter = (fabs(phi - phi2) < DeltaPhi) ||
                        (fabs(phi - phi2) > (2 * M_PI - DeltaPhi));
        if (other->layerID != 0 && z_filter) {
            if (other->layerID == 1 && point->xyDistance(other) < DMin01) {
                track.AddPoint(other);
            }
            if (other->layerID == 2 && point->xyDistance(other) < DMin02) {
                track.AddPoint(other);
            }
        }
    }
    if (!track.HitALayers()) {
        track.Clear();
        return std::move(track);
    }
    return std::move(track);
    // Track result;
    // vector<HitPoint *> layer0;
    // vector<HitPoint *> layer1;
    // vector<HitPoint *> layer2;
    // for (auto *point : track.GetPoints()) {
    //     if (point->layerID == 0) {
    //         layer0.push_back(point);
    //     } else if (point->layerID == 1) {
    //         layer1.push_back(point);
    //     } else if (point->layerID == 2) {
    //         layer2.push_back(point);
    //     }
    // }
    // return std::move(result);
}

int main(int argc, char **argv) {
    Args args;
    args_parse(argc, argv, "LocalTracker", args);
    args_out_json(args);

    /**
     *  @brief Initialize the source data
     */
    TreeData data = TreeData(args.data_file.c_str());
    cout << args.output_file << endl;
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
        eventIDs_toTest = GetRandomSet(args.n_track, eventIDs_all);
    } else if (args.mode == ExecMode::all) {
        eventIDs_toTest.reset(&eventIDs_all);
    }

    /**
     *  @brief Initialize the output file
     */
    TFile savefile(args.output_file.c_str(), "RECREATE");
    TTree savetree("tree1", "tree1");
    int event_id = 0, track_id = 0, num_true = 0, num_total = 0, Q_e = 0;
    double Qmin = NAN, pt = NAN, Qz = NAN;
    bool true_track = false;
    int counts_useful_events = 0;
    savetree.Branch("event_id", &event_id);
    savetree.Branch("track_id", &track_id);
    savetree.Branch("true_track", &true_track);
    savetree.Branch("Pt", &pt);
    savetree.Branch("Q", &Qmin);
    savetree.Branch("Qz", &Qz);
    savetree.Branch("num_true", &num_true);
    savetree.Branch("num_total", &num_total);
    savetree.Branch("Qe", &Q_e);

    /**
     * @brief Get true points from source data
     */
    while (eventIDs_toTest->size() >= args.n_track) {
        std::cout << "There are " << eventIDs_toTest->size()
                  << " events left\n";
        // int eventIDTest;
        auto test_set = GetRandomSet(args.n_track, *eventIDs_toTest);
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
        InnerAddNoise(args.n_noise, pointsList);
        vector<Track> tracks;
        for (auto *point : pointsList) {
            auto track = find_track(point, pointsList);
            if (tracks.empty()) {
                tracks.push_back(std::move(track));
            } else {
                for (auto &exist : tracks) {
                    if (exist == track) {
                        break;
                    }
                    if (exist > track) {
                        break;
                    }
                    tracks.push_back(std::move(track));
                }
            }
        }
        int track_id_re = 0;
        for (auto track : tracks) {
            Polynomial<2> line;
            double Qz_swap = NAN;
            double pt_swap = NAN;
            double Qmin_swap = NAN;
            if ((track.Counts() >= 3) && (track.HitALayers()) &&
                (track.FitLinear(&pt_swap, &Qmin_swap, &Qz_swap))) {
                num_total = static_cast<int>(track.Counts());
                num_true =
                    static_cast<int>(track.NumTruePointsMulti(test_set.get()));
                true_track = track.ContainTrueTrack();
                pt = pt_swap;
                event_id = track.GetEventID(test_set.get());
                track_id = track_id_re;
                track_id_re++;
                Qz = Qz_swap;
                Qmin = Qmin_swap;
                savetree.Fill();
                if (args.mode == ExecMode::single) {
                    ofstream out1("tracks.txt", std::ios::app);
                    out1 << std::boolalpha << true_track << "\t";
                    for (auto *point : track.GetPoints()) {
                        out1 << point->id() << "\t";
                    }
                    out1 << "\n";
                }
            }
        }
        if (args.mode == ExecMode::single) {
            ofstream out2("points.txt", std::ios::app);
            for (auto *point : pointsList) {
                out2 << point->eventID << "\t" << point->id() << "\t"
                     << point->x << "\t" << point->y << "\t" << point->z
                     << "\n";
            }
        }
    }
    savefile.cd();
    savetree.Write();
    savefile.Write();
    savefile.Close();
    cout << "Save Path: " << args.output_file << endl;
    cout << "total tracks useful: " << counts_useful_events << endl << endl;
    return 0;
}
