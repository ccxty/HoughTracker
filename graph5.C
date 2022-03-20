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
    string pt_str[] = {"65", "70", "75", "80",
                       "85", "90", "95", "100",
                       "105", "110", "115", "120",
                       "125", "130", "135"};
    string noise_str[] = {"3", "6", "9", "12", "15", "18", "21"};
    string out_txt_str = "./fake_rate.txt";
    ofstream out_txt_stream(out_txt_str, ios::app);
    double total[] = {939, 953, 952, 950,
                      952, 964, 974, 943,
                      968, 965, 972, 969,
                      970, 974, 973};
    for (int i = 0; i < 15; i++)
    {
        for (int j = 0; j < 7; ++j)
        {
            string filepath = "./trackdata_Pt" + pt_str[i] +
                              "_noise" + noise_str[j] + ".root";
            auto file = new TFile(filepath.c_str());
            auto tree = (TTree *)gDirectory->Get("tree1");
            int event_id, track_id, num_true, num_total, num_with_cut = 0;
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
            // int n_total = 0;
            double n_total = total[i];
            int event_id_temp1 = -1;
            int event_id_temp2 = -1;
            for (int ie = 0; ie < entries; ie++)
            {
                tree->GetEntry(ie);
                if ((event_id_temp1 != event_id))
                {
                    // n_total++;
                    event_id_temp1 = event_id;
                }
                if (Pt > 1)
                {
                    num_with_cut++;
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
            std::cout << "n_total: " << n_total << "\tn_eff: " << n_eff << "\tnum_with_cut: " << num_with_cut << std::endl;
            std::cout << "eff: " << double(n_eff) / double(n_total) << std::endl;
            std::cout << "fake ratio: " << 1.0 - double(n_eff) / double(num_with_cut) << std::endl;
            std::cout << "number of tracks without fake points: " << n_all_true_points << std::endl;
            out_txt_stream << pt_str[i] << " " << noise_str[j] << " " << double(n_eff) / n_total << " " << 1.0 - double(n_eff) / double(num_with_cut) << std::endl;
        }
    }
}