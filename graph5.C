#include "TFile.h"
#include "TCanvas.h"
#include "TH1F.h"
#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include "TGraphErrors.h"
#include "TTree.h"

using namespace std;
void graph5()
{
    string filepath = "./trackdata_Pt65_noise21.root";
    auto file = new TFile(filepath.c_str());
    auto tree = (TTree *)gDirectory->Get("tree1");
    int event_id, track_id, num_true, num_total;
    bool true_track;
    double Pt, Q;
    tree->SetBranchAddress("event_id", &event_id);
    tree->SetBranchAddress("track_id", &track_id);
    tree->SetBranchAddress("num_true", &num_true);
    tree->SetBranchAddress("num_total", &num_total);
    tree->SetBranchAddress("true_track", &true_track);
    tree->SetBranchAddress("Pt", &Pt);
    tree->SetBranchAddress("Q", &Q);
    int entries = tree->GetEntries();
    int n_eff = 0;
    int n_all_true_points = 0;
    int n_total = 0;
    int event_id_temp1 = -1;
    int event_id_temp2 = -1;
    for (int ie = 0; ie < entries; ie++)
    {
        tree->GetEntry(ie);
        if (event_id_temp1 != event_id)
        {
            n_total++;
            event_id_temp1 = event_id;
        }
        if (true)
        {
            if ((true_track == 1) && (event_id_temp2 != event_id))
            {
                n_eff++;
                event_id_temp2 = event_id;
            }
            if ((true_track == 1) && (num_total == 3))
            {
                n_all_true_points++;
            }
        }
    }
    std::cout << entries << std::endl;
    std::cout << "n_total: " << n_total << "\tn_eff: " << n_eff << std::endl;
    std::cout << "eff: " << double(n_eff) / double(n_total) << std::endl;
    std::cout << "fake ratio: " << 1.0 - double(n_eff) / double(entries) << std::endl;
    std::cout << "number of tracks without fake points: " << n_all_true_points << std::endl;
}