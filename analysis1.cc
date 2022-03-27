#include <TGraphErrors.h>
#include <TMultiGraph.h>

#include <cmath>
#include <iostream>

#include "TCanvas.h"
#include "TFile.h"
#include "TH1F.h"
#include "TTree.h"

using namespace std;

void analysis1() {
  string pt_str[] = {"50",  "55",  "60",  "65",  "70",  "75",
                     "80",  "85",  "90",  "95",  "100", "105",
                     "110", "115", "120", "125", "130", "135"};
  string noise_str[] = {"0",  "6",  "12", "18", "24",
                        "30", "39", "48", "57", "66"};
  double noise_d[] = {0, 6, 12, 18, 24, 30, 39, 48, 57, 66};
  string multi_str[] = {"1", "3", "5", "7"};

  double total_e_minus[] = {9938, 9951, 9956, 9956, 9962, 9971,
                            9970, 9968, 9968, 9968, 9980, 9983,
                            9987, 9983, 9981, 9977, 9982, 9978};
  double total_e_plus[] = {9917, 9930, 9936, 9943, 9951, 9951,
                           9950, 9960, 9957, 9960, 9961, 9964,
                           9962, 9961, 9974, 9962, 9964, 9971};

  int i_pt = 0;
  int i_multi = 0;
  double effs[10];
  double effs_err[10];
  double fakes[10];
  double fakes_err[10];
  string save_path_eff = "./eff_Pt";
  save_path_eff += pt_str[i_pt];
  save_path_eff += "_multi";
  save_path_eff += multi_str[i_multi];
  save_path_eff += ".png";
  string save_path_fake = "./fake_Pt";
  save_path_fake += pt_str[i_pt];
  save_path_fake += "_multi";
  save_path_fake += multi_str[i_multi];
  save_path_fake += ".png";
  for (int j = 0; j < 10; j++) {
    double total = total_e_minus[i_pt];
    string path("./trackdata_Pt");
    path += pt_str[i_pt];
    path += "_noise";
    path += noise_str[j];
    path += "_multi";
    path += multi_str[i_multi];
    path += ".root";
    cout << path << endl;
    TFile file(path.c_str());
    auto tree = (TTree *)gDirectory->Get("tree1");
    int event_id;
    int track_id;
    bool true_track;
    double Pt;
    double Q;
    double Qz;
    int num_true;
    int num_total;
    int Qe;
    tree->SetBranchAddress("event_id", &event_id);
    tree->SetBranchAddress("track_id", &track_id);
    tree->SetBranchAddress("true_track", &true_track);
    tree->SetBranchAddress("Pt", &Pt);
    tree->SetBranchAddress("Q", &Q);
    tree->SetBranchAddress("Qz", &Qz);
    tree->SetBranchAddress("num_true", &num_true);
    tree->SetBranchAddress("num_total", &num_total);
    tree->SetBranchAddress("Qe", &Qe);

    int n_eff = 0;
    double n_true = 0;
    int event_id_skip = -1;
    int n_all = tree->GetEntries();
    for (Long64_t ie = 0; ie < n_all; ie++) {
      tree->GetEntry(ie);
      if ((event_id != event_id_skip) && (event_id != -1)) {
        event_id_skip = event_id;
        n_eff++;
      }
      if (event_id != -1) {
        n_true += 1.0;
      }
    }
    effs[j] = n_eff / total;
    fakes[j] = 1 - n_true / n_all;
    effs_err[j] = sqrt(effs[j] * (1 - effs[j]) * total) / total;
    fakes_err[j] = sqrt(fakes[j] * (1 - fakes[j]) * n_all) / n_all;
    cout << "eff: " << effs_err[j] << "\tfake: " << fakes_err[j] << endl;
  }
  TCanvas *c1 = new TCanvas();
  TCanvas *c2 = new TCanvas();
  TGraphErrors g_eff(10, noise_d, effs, 0, effs_err);
  TGraphErrors g_fake(10, noise_d, fakes, 0, fakes_err);
  g_eff.SetMarkerStyle(EMarkerStyle::kFullCircle);
  g_eff.SetMarkerSize(0.5);
  g_eff.SetTitle("eff with noise");
  g_eff.GetXaxis()->SetTitle("number of noise points");
  g_eff.GetYaxis()->SetTitle("eff");
  c1->cd();
  g_eff.Draw();
  c1->Draw();
  c1->SaveAs(save_path_eff.c_str());
  c1->Close();
  delete c1;
  // g_fake.SetMarkerStyle(EMarkerStyle::kFullCircle);
  // g_fake.SetMarkerSize(0.5);
  g_fake.SetTitle("fake rate with noise");
  g_fake.GetXaxis()->SetTitle("number of noise points");
  g_fake.GetYaxis()->SetTitle("fake rate");
  c2->cd();
  g_fake.Draw();
  c2->Draw();
  c2->SaveAs(save_path_fake.c_str());
  c2->Close();
  delete c2;
}