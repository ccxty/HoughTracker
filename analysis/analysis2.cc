#include <fstream>
#include <iostream>
#include <string>

#include "TCanvas.h"
#include "TFile.h"
#include "TGraphErrors.h"
#include "TH1F.h"
#include "TTree.h"

using std::array;
using std::endl;
using std::string;
using std::vector;

void analysis2() {
    array<string, 6> particle_str = {"e-", "e+", "pi-", "pi+", "mu-", "mu+"};
    array<string, 18> pt_str = {"50",  "55",  "60",  "65",  "70",  "75",
                                "80",  "85",  "90",  "95",  "100", "105",
                                "110", "115", "120", "125", "130", "135"};
    for (int i = 0; i < 6; i++) {
        string savepath = "total_" + particle_str[i] + ".txt";
        std::ofstream out(savepath, std::ios::app);
        for (int j = 0; j < 18; j++) {
            string path =
                "/home/ubuntu-tyxiao/work/STCF_Oscar2.0.0/Tracker/"
                "root_data_source/";
            path += particle_str[i] + "/posPt" + pt_str[j] + ".root";
            auto *file = new TFile(path.c_str());
            TTree *tree = dynamic_cast<TTree *>(gDirectory->Get("tree1"));
            const Long64_t nevents = tree->GetEntries();
            vector<double> *posX = nullptr;
            vector<double> *posY = nullptr;
            vector<double> *posZ = nullptr;
            vector<int> *trackID = nullptr;
            vector<int> *layerID = nullptr;
            vector<double> *P_t = nullptr;
            TBranch *b_posX = nullptr;
            TBranch *b_posY = nullptr;
            TBranch *b_posZ = nullptr;
            TBranch *b_trackID = nullptr;
            TBranch *b_Pt = nullptr;
            TBranch *b_layerID = nullptr;
            int eventID = 0;
            int nhits = 0;
            tree->SetBranchAddress("posX", &posX, &b_posX);
            tree->SetBranchAddress("posY", &posY, &b_posY);
            tree->SetBranchAddress("posZ", &posZ, &b_posZ);
            tree->SetBranchAddress("eventID", &eventID);
            tree->SetBranchAddress("layerID", &layerID, &b_layerID);
            tree->SetBranchAddress("trackID", &trackID, &b_trackID);
            tree->SetBranchAddress("Pt", &P_t, &b_Pt);
            tree->SetBranchAddress("nhits", &nhits);

            int n_total = 0;
            for (auto ie = 0; ie < nevents; ie++) {
                tree->GetEntry(ie);
                int npoints = 0;
                for (auto identity : *trackID) {
                    if (identity == 1) {
                        npoints++;
                    }
                }
                if (npoints >= 3) {
                    n_total++;
                }
            }
            out << n_total << endl;
        }
    }
}