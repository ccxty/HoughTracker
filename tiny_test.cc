#include <iostream>
#include <ostream>

#include "TFile.h"
#include "TTree.h"

using std::cout;
using std::endl;
using std::string;

void tiny_test() {
    // string tiny_test_file = "./data/pi-/trackdata_Pt60_noise39_multi1.root";
    string tiny_test_file =
        "./data/e-/trackdata_Pt50_noise66_multi1_deg90.root";
    TFile *file = new TFile(tiny_test_file.c_str());
    auto tree = dynamic_cast<TTree *>(gDirectory->Get("tree1"));
    int event_id = 0;
    int track_id = 0;
    bool true_track = false;
    double pt = NAN;
    double Qxy = NAN;
    double Qz = NAN;
    int num_first_half = 0;
    int num_total = 0;
    double Qe = 0;
    tree->SetBranchAddress("eventID", &event_id);
    tree->SetBranchAddress("trackID", &track_id);
    tree->SetBranchAddress("true_track", &true_track);
    tree->SetBranchAddress("pt", &pt);
    tree->SetBranchAddress("Qxy", &Qxy);
    tree->SetBranchAddress("Qz", &Qz);
    tree->SetBranchAddress("num_first_half", &num_first_half);
    tree->SetBranchAddress("num_total", &num_total);
    tree->SetBranchAddress("Qe", &Qe);
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