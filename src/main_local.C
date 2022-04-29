#include <cmath>

#include "BasicTreeSave.h"
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
using std::string;
using std::unique_ptr;
using std::vector;

inline Track find_track(HitPoint *point, const Points &points) {
    Track track(point);
    double k1 = point->z / InnerDectectorR[0];
    std::vector<HitPoint *> temp_l1;
    for (auto *other : points) {
        bool z_filter = fabs(other->z - point->z - k1 * DeltaR01) < DeltaZ01;
        bool xy_filter = point->xyDistance(other) < DMin01;
        if (other->layerID == 1 && z_filter && xy_filter) {
            track.AddPoint(other);
            temp_l1.push_back(other);
        }
    }
    for (auto *point1 : temp_l1) {
        double k2 = (point1->z - point->z) / DeltaR01;
        for (auto *other : points) {
            double phi2 = other->Phi();
            bool z_filter =
                fabs(other->z - point1->z - k2 * DeltaR12) < DeltaZ12;
            bool xy_filter = point1->xyDistance(other) < DMin12;
            if (other->layerID == 2 && z_filter && xy_filter) {
                track.AddPoint(other);
            }
        }
    }
    if (!track.HitALayers()) {
        track.Clear();
        return std::move(track);
    }
    return std::move(track);
}

int main(int argc, char **argv) {
    Args args;
    args_parse(argc, argv, "LocalTracker", args);
    args_out_json(args);

    /**
     *  @brief Initialize the source data
     */
    TreeRead data = TreeRead(args.data_file.c_str());
    cout << "DataFile:" << args.data_file << endl;
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
    int counts_useful_events = 0;
    BasicTreeSave save(args.output_file.c_str(), "tree1");

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
                    ptr->SetId(read_count);
                    pointsList.push_back(ptr);
                    read_count++;
                }
                if (read_count >= 3) {
                    // eventID_skip = data.EventID();
                }
            }
        }
        if (pointsList.size() < 3) {
            continue;
        }
        InnerAddNoise(args.n_noise, pointsList);

        vector<Track> tracks;
        for (auto *point : pointsList) {
            auto track = find_track(point, pointsList);
            if (tracks.empty()) {
                tracks.push_back(std::move(track));
            } else {
                for (auto &exist : tracks) {
                    if (exist >= track) {
                        break;
                    }
                    tracks.push_back(std::move(track));
                }
            }
        }
        int track_id_re = 0;
        for (auto track : tracks) {
            StraightLine line;
            double Qz_swap = NAN;
            double Qmin_swap = NAN;
            if ((track.Counts() >= 3) && (track.HitALayers()) &&
                (track.FitLinear(&Qmin_swap, &Qz_swap))) {
                auto params = track.GetTrackParameters();
                save.num_total = track.Counts();
                save.num_first_half = track.NumFirstHalfPoints(test_set.get());
                save.true_track = track.ContainFirstHalf(test_set.get());
                save.pt = track.Pt();
                save.pt_true = args.pt;
                save.event_id = track.GetEventID(test_set.get());
                save.track_id = track_id_re;
                track_id_re++;
                save.Qz = Qz_swap;
                save.Qxy = Qmin_swap;
                save.kz = track.GetTrackParameters().kz;
                save.contain_first_half =
                    track.ContainFirstHalf(test_set.get());
                save.contain_second_half =
                    track.ContainSecondHalf(test_set.get());
                save.num_second_half =
                    track.NumSecondHalfPoints(test_set.get());
                save.DeltaDXY = fabs(sqrt(params.center.x * params.center.x +
                                          params.center.y * params.center.y));
                save.Fill();
                if (args.mode == ExecMode::single) {
                    ofstream out1("tracks.txt", std::ios::app);
                    out1 << save.event_id << "\t" << std::boolalpha
                         << save.true_track << "\t";
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
                out2 << point->eventID << "\t" << point->layerID << "\t"
                     << point->id() << "\t" << point->x << "\t" << point->y
                     << "\t" << point->z << "\n";
            }
        }
    }
    save.Write().Close();
    cout << "Save Path: " << args.output_file << endl
         << "total tracks useful: " << counts_useful_events << endl;
    return 0;
}