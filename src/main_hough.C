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

struct SaveData {
    int event_id;
    int track_id;
    int num_true;
    int num_total;
    bool true_track;
    double Qz;
    double Qxy;
    double Qe;
    double pt;
};

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
        cerr << "Data not found" << endl;
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
    TFile savefile(args.output_file.c_str(), "RECREATE");
    TTree savetree("tree1", "tree1");
    SaveData saveData{};
    int counts_useful_events = 0;
    savetree.Branch("event_id", &saveData.event_id);
    savetree.Branch("track_id", &saveData.track_id);
    savetree.Branch("true_track", &saveData.true_track);
    savetree.Branch("Pt", &saveData.pt);
    savetree.Branch("Q", &saveData.Qxy);
    savetree.Branch("Qz", &saveData.Qz);
    savetree.Branch("num_true", &saveData.num_true);
    savetree.Branch("num_total", &saveData.num_total);
    savetree.Branch("Qe", &saveData.Qe);

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
        auto houghGrid = Hough::GridInit<Hough::NAlpha, Hough::NumD>();
        Hough::FillGrid<Hough::NAlpha, Hough::NumD>(houghGrid, pointsList);
        auto tracks = Hough::FindTrack<Hough::NAlpha, Hough::NumD>(houghGrid);

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
                    saveData.event_id = track->GetEventID(test_set.get());
                    saveData.track_id = track_id_re;
                    track_id_re++;
                    saveData.true_track =
                        track->ContainFirstHalf(test_set.get());
                    saveData.pt = track->Pt();
                    saveData.Qxy = Q_xy;
                    saveData.Qz = Q_z;
                    saveData.num_true =
                        track->NumFirstHalfPoints(test_set.get());
                    saveData.num_total = static_cast<int>(track->Counts());
                    saveData.Qe = track->GetSpin();
                    savefile.cd();
                    savetree.Fill();

                    n_good_tracks++;
                    if (args.mode == ExecMode::single) {
                        ofstream out1("tracks.txt", std::ios::app);
                        out1 << std::boolalpha << saveData.true_track << "\t";
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
    savefile.cd();
    savetree.Write();
    savefile.Write();
    savefile.Close();
    cout << "Save Path: " << args.output_file << endl
         << "total tracks useful: " << counts_useful_events << endl
         << endl;
    return 0;
}