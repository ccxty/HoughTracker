#include <iostream>
#include <ostream>
#include <set>

#include "TFile.h"
#include "TTree.h"

using std::array;
using std::cout;
using std::endl;
using std::string;

array<array<double, 2>, 2> get_eff_fake(TTree *tree) {
    int eventID = 0;
    int trackID = 0;
    double DeltaDXY = NAN;
    double pt = NAN;
    double kz = NAN;
    double kz_true = NAN;
    double Qz = NAN;
    double Qe = NAN;
    int num_total = 0;
    bool true_track = false;
    tree->SetBranchAddress("eventID", &eventID);
    tree->SetBranchAddress("trackID", &trackID);
    tree->SetBranchAddress("DeltaDXY", &DeltaDXY);
    tree->SetBranchAddress("pt", &pt);
    tree->SetBranchAddress("kz", &kz);
    tree->SetBranchAddress("kz_true", &kz_true);
    tree->SetBranchAddress("Qz", &Qz);
    tree->SetBranchAddress("Qe", &Qe);
    tree->SetBranchAddress("num_total", &num_total);
    tree->SetBranchAddress("true_track", &true_track);
    int n_eff = 0;
    int n_true = 0;
    std::set<int> event_ids;
    auto n_all = static_cast<double>(tree->GetEntries());
    for (Long64_t ie = 0; ie < tree->GetEntries(); ie++) {
        tree->GetEntry(ie);
        if (eventID != -1) {
            event_ids.insert(eventID);
            n_true++;
        }
    }
    n_eff = event_ids.size();
    double eff = static_cast<double>(n_eff) / 10000.;
    if (n_eff == 0) {
        eff = 0.;
    }
    double eff_err = sqrt(eff * (1 - eff) * 10000.) / 10000.;
    double fake = 1 - static_cast<double>(n_true) / static_cast<double>(n_all);
    if (n_true == 0) {
        fake = 1.;
    }
    double fake_err = sqrt(fake * (1 - fake) * n_all) / n_all;
    array<array<double, 2>, 2> result{};
    result[0][0] = eff;
    result[0][1] = eff_err;
    result[1][0] = fake;
    result[1][1] = fake_err;
    return result;
}

void tiny_test() {
    // string tiny_test_file = "./data/pi-/trackdata_Pt60_noise39_multi1.root";
    string tiny_test_file =
        "./data/e-/trackdata_Pt50_noise66_multi1_deg90.root";
    TFile *file = new TFile(tiny_test_file.c_str());
    auto *tree = dynamic_cast<TTree *>(gDirectory->Get("tree1"));
    auto result = get_eff_fake(tree);
    cout << "eff: " << result[0][0] << "\tfake: " << result[1][0] << endl;
}
