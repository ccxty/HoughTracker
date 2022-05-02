#include "BasicTreeSave.h"
#include "HitPoint.h"
#include "Hough.h"
#include "HoughGridArea.h"
#include "Track.h"
#include "TreeRead.h"
#include "args.h"

using std::array;
using std::cerr;
using std::cout;
using std::endl;
using std::make_unique;
using std::move;
using std::ofstream;
using std::set;
using std::string;
using std::unique_ptr;
using std::vector;

int main(int argc, char **argv) {
    Args args;
    args_parse(argc, argv, "HoughTracker", args);
    args_out_json(args);

    /**
     *  @brief Initialize the source data
     */
    TreeRead data = TreeRead(args.data_file.c_str());
    cout << args.data_file << endl;
    if (data.isEmpty()) {
        cerr << "Data not found\n";
        return 0;
    }

    /**
     * @brief Initialize the eventIDs to test
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

    // <eventID, num of all true points in this event>

    /**
     *  @brief Initialize the output file
     */
    BasicTreeSave save(args.output_file.c_str());
    int counts_useful_events = 0;
    int num_second = 0;
    int num_layer0 = 0, num_layer1 = 0, num_layer2 = 0;
    save.tree.Branch("num_layer0", &num_layer0);
    save.tree.Branch("num_layer1", &num_layer1);
    save.tree.Branch("num_layer2", &num_layer2);

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
        int npoints = static_cast<int>(pointsList.size());

        /**
         * @brief Hough transform
         *
         */
        auto houghGrid1 = Hough::GridInit<Hough::NAlpha, Hough::NRho>(0);
        auto houghGrid2 = Hough::GridInit<Hough::NAlpha, Hough::NRho>(0.5);
        Hough::FillGrid<Hough::NAlpha, Hough::NRho>(houghGrid1, pointsList);
        Hough::FillGrid<Hough::NAlpha, Hough::NRho>(houghGrid2, pointsList);
        auto tracks1 = Hough::FindTrack<Hough::NAlpha, Hough::NRho>(houghGrid1);
        auto tracks2 = Hough::FindTrack<Hough::NAlpha, Hough::NRho>(houghGrid2);
        auto tracks = Hough::MergeTracks(tracks1, tracks2);
        // auto tracks = std::move(tracks1);

        /**
         * @brief track filter
         */
        int track_id_re = 0;
        int n_good_tracks = 0;
        for (auto &track : tracks) {
            double Q_xy = NAN, Q_z = NAN;
            if (track->HitALayers()) {
                bool fit_fine = track->FitLinear(&Q_xy, &Q_z);
                if (fit_fine && (track->Pt() > PtMin) && (Q_xy < QCut)) {
                    save.event_id = track->GetEventID(test_set.get());
                    save.track_id = track_id_re;
                    track_id_re++;
                    save.true_track = track->ContainFirstHalf(test_set.get());
                    save.pt = track->Pt();
                    save.Qxy = Q_xy;
                    save.Qz = Q_z;
                    save.num_first_half =
                        track->NumFirstHalfPoints(test_set.get());
                    save.num_total = track->Counts();
                    save.Qe = track->GetSpin();
                    num_second = track->NumSecondHalfPoints(test_set.get());

                    auto num_layers = track->GetLayerDistribution();
                    num_layer0 = std::get<0>(num_layers);
                    num_layer1 = std::get<1>(num_layers);
                    num_layer2 = std::get<2>(num_layers);
                    save.Fill();

                    n_good_tracks++;
                    if (args.mode == ExecMode::single) {
                        ofstream out1("tracks.txt", std::ios::app);
                        out1 << std::boolalpha << save.true_track << "\t";
                        for (auto *point : track->GetPoints()) {
                            out1 << point->id() << "\t";
                        }
                        out1 << "\n";
                    }
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

        for (auto *ptr : pointsList) {
            delete ptr;
        }
    }
    save.Write().Close();
    cout << "Save Path: " << args.output_file << "\n"
         << "total tracks useful: " << counts_useful_events << "\n"
         << endl;
    return 0;
}