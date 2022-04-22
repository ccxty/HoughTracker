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
void graph7()
{
    string pt_str[] = {"65", "75",
                       "85", "95",
                       "110",
                       "125", "135"};
    string noise_str[] = {"0", "3", "6", "9", "12", "15", "18", "21"};
    string out_txt_str = "./fake_rate_multi.txt";
    ofstream out_txt_stream(out_txt_str, ios::app);
    double total[] = {939, 952,
                      952, 974,
                      965,
                      970, 973};
    int multi[] = {2, 3, 4};
    string multi_str[] = {"2", "3", "4"};
    for (int i = 0; i < 7; i++)
    {
        for (int j = 0; j < 8; ++j)
        {
            for (int k = 0; k < 3; k++)
            {
                string filepath = "./trackdata_Pt" + pt_str[i] +
                                  "_noise" + noise_str[j] + "_" +
                                  multi_str[k] + ".root";
                std::cout << filepath << std::endl;
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
                            // event_id_temp2 = event_id;
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
                std::cout << "fake rate: " << 1.0 - double(n_eff) / double(num_with_cut) << std::endl;
                std::cout << "number of tracks without fake points: " << n_all_true_points << std::endl;
                out_txt_stream << multi_str[k] << " " << pt_str[i] << " " << noise_str[j] << " " << double(n_eff) / n_total << " " << 1.0 - double(n_eff) / double(num_with_cut) << std::endl;
            }
        }
    }
}