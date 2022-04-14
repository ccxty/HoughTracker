#include <iostream>
#include <ostream>

#include "TFile.h"
#include "TTree.h"

using std::cout;
using std::endl;
using std::string;

void tiny_test() {
    // string tiny_test_file = "./data/pi-/trackdata_Pt60_noise39_multi1.root";
    string tiny_test_file = "./data/e-/trackdata_Pt50_noise39_multi1.root";
    TFile *file = new TFile(tiny_test_file.c_str());
    auto tree = dynamic_cast<TTree *>(gDirectory->Get("tree1"));
    int event_id = 0;
    int track_id = 0;
    bool true_track = false;
    double P_t = NAN;
    double Q_xy = NAN;
    double Q_z = NAN;
    int num_true = 0;
    int num_total = 0;
    int Q_e = 0;
    tree->SetBranchAddress("event_id", &event_id);
    tree->SetBranchAddress("track_id", &track_id);
    tree->SetBranchAddress("true_track", &true_track);
    tree->SetBranchAddress("Pt", &P_t);
    tree->SetBranchAddress("Q", &Q_xy);
    tree->SetBranchAddress("Qz", &Q_z);
    tree->SetBranchAddress("num_true", &num_true);
    tree->SetBranchAddress("num_total", &num_total);
    tree->SetBranchAddress("Qe", &Q_e);
    int n_eff = 0;
    int n_true = 0;
    int event_id_skip = -1;
    auto n_all = tree->GetEntries();
    for (Long64_t ie = 0; ie < n_all; ie++) {
        tree->GetEntry(ie);
        if ((event_id != event_id_skip) && (event_id != -1)) {
            event_id_skip = event_id;
            n_eff++;
        }
        if (event_id != -1) {
            n_true++;
        }
    }
    double total = 9950;
    double eff = n_eff / total;
    double fake = 1 - n_true / static_cast<double>(n_all);
    cout << "eff: " << eff << "\tfake: " << fake << endl;
}