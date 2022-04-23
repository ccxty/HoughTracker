#include <array>
#include <iostream>
#include <ostream>
#include <string>
#include <vector>

#include "Rtypes.h"
#include "TCanvas.h"
#include "TFile.h"
#include "TH1.h"
#include "TTree.h"
#include "include/HitPoint.h"
#include "include/TreeRead.h"
#include "include/global.h"

using std::array;
using std::cout;
using std::endl;
using std::string;
using std::vector;

void compare_pt() {
    string re_file_path = "./data/e-/trackdata_Pt50_noise66_multi1.root";
    string origin_file_path = "./data/e-/trackdata_Pt50_noise0_multi1.root";

    auto *re_file = new TFile(re_file_path.c_str());
    auto *tree_re = dynamic_cast<TTree *>(gDirectory->Get("tree1"));
    int event_id = 0;
    int track_id = 0;
    bool true_track = false;
    double P_t = NAN;
    double Q_xy = NAN;
    double Q_z = NAN;
    int num_true = 0;
    int num_total = 0;
    int Q_e = 0;
    tree_re->SetBranchAddress("event_id", &event_id);
    tree_re->SetBranchAddress("track_id", &track_id);
    tree_re->SetBranchAddress("true_track", &true_track);
    tree_re->SetBranchAddress("Pt", &P_t);
    tree_re->SetBranchAddress("Q", &Q_xy);
    tree_re->SetBranchAddress("Qz", &Q_z);
    tree_re->SetBranchAddress("num_true", &num_true);
    tree_re->SetBranchAddress("num_total", &num_total);
    tree_re->SetBranchAddress("Qe", &Q_e);

    auto *c1 = new TCanvas("c1", "c1");
    TH1F *h1 = new TH1F("h1", "noise 66", 250, PtMin, 70);
    TH1F *h2 = new TH1F("h2", "noise 0", 250, PtMin, 70);

    for (int i = 0; i < tree_re->GetEntries(); i++) {
        tree_re->GetEntry(i);
        if (event_id == -1) {
            continue;
        }
        if (static_cast<int>(true_track) == 1) {
            h1->Fill(P_t);
        }
    }
    c1->cd();
    h1->Draw();
    c1->Update();
    auto *or_file = new TFile(origin_file_path.c_str());
    auto *tree_or = dynamic_cast<TTree *>(gDirectory->Get("tree1"));
    tree_or->SetBranchAddress("event_id", &event_id);
    tree_or->SetBranchAddress("track_id", &track_id);
    tree_or->SetBranchAddress("true_track", &true_track);
    tree_or->SetBranchAddress("Pt", &P_t);
    tree_or->SetBranchAddress("Q", &Q_xy);
    tree_or->SetBranchAddress("Qz", &Q_z);
    tree_or->SetBranchAddress("num_true", &num_true);
    tree_or->SetBranchAddress("num_total", &num_total);
    tree_or->SetBranchAddress("Qe", &Q_e);
    for (int i = 0; i < tree_or->GetEntries(); i++) {
        tree_or->GetEntry(i);
        if (event_id == -1) {
            continue;
        }
        if (static_cast<int>(true_track) == 1) {
            h2->Fill(P_t);
        }
    }
    c1->cd();
    h2->SetLineColor(kRed);
    h2->Draw("same");
    std::cout << "mean: " << h1->GetMean() << std::endl;
    std::cout << "std Dev: " << h1->GetRMS() << std::endl << std::endl;
    std::cout << "mean: " << h2->GetMean() << std::endl;
    std::cout << "std Dev: " << h2->GetRMS() << std::endl << std::endl;
}