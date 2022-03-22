#include <iostream>
#include "TMultiGraph.h"
#include "TCanvas.h"
#include "TAxis.h"
#include "TGraphErrors.h"

void graph8()
{
    double fake_noise3[] = {0.106825, 0.0931615, 0.0992141, 0.0979432,
                            0.11423, 0.0968054, 0.0960615, 0.100691,
                            0.0958773, 0.0702811, 0.0997124, 0.0967118,
                            0.112595, 0.101905, 0.0817916};
    double fake_noise9[] = {0.478085, 0.466053, 0.484016, 0.473081,
                            0.470994, 0.468146, 0.470885, 0.469845,
                            0.46329, 0.471088, 0.465301, 0.489303,
                            0.471517, 0.466479, 0.47049};
    double fake_noise15[] = {0.748678, 0.750478, 0.749658, 0.746553,
                             0.742467, 0.750268, 0.741431, 0.751281,
                             0.7369, 0.747189, 0.753504, 0.753846,
                             0.745971, 0.746972, 0.745831};
    double fake_noise21[] = {0.865773, 0.87082, 0.86858, 0.867662,
                             0.867276, 0.867211, 0.867543, 0.868048,
                             0.867919, 0.866763, 0.867228, 0.866734,
                             0.872843, 0.866752, 0.867413};
    double eff_noise3[] = {0.961661, 0.960126, 0.963235, 0.969474,
                           0.961134, 0.967842, 0.966119, 0.966066,
                           0.974174, 0.959585, 0.966049, 0.96388,
                           0.958763, 0.968172, 0.969168};
    double eff_noise9[] = {0.963791, 0.973767, 0.966387, 0.968421,
                           0.967437, 0.969917, 0.970226, 0.978791,
                           0.974174, 0.966839, 0.967078, 0.960784,
                           0.965979, 0.972279, 0.97739};
    double eff_noise15[] = {0.961661, 0.958027, 0.961134, 0.967368,
                            0.969538, 0.966805, 0.968172, 0.977731,
                            0.980372, 0.95544, 0.958848, 0.957688,
                            0.958763, 0.965092, 0.971223};
    double eff_noise21[] = {0.948882, 0.944386, 0.938025, 0.961053,
                            0.961134, 0.961618, 0.962012, 0.966066,
                            0.965909, 0.95544, 0.957819, 0.954592,
                            0.949485, 0.959959, 0.960946};
    double pts[] = {65, 70, 75, 80, 85, 90, 95, 100,
                    105, 110, 115, 120, 125, 130, 135};
    double eff_pt65[] = {0.961661, 0.969116, 0.963791, 0.966986, 0.961661, 0.952077, 0.948882};
    double eff_pt80[] = {0.969474, 0.965263, 0.968421, 0.967368, 0.967368, 0.964211, 0.961053};
    double eff_pt95[] = {0.966119, 0.969199, 0.970226, 0.968172, 0.968172, 0.964066, 0.962012};
    double eff_pt110[] = {0.959585, 0.970984, 0.966839, 0.965803, 0.95544, 0.963731, 0.95544};
    double eff_pt125[] = {0.958763, 0.963918, 0.965979, 0.958763, 0.958763, 0.961856, 0.949485};
    double fake_pt65[] = {0.106825, 0.272, 0.478085, 0.639253, 0.748678, 0.818219, 0.865773};
    double fake_pt80[] = {0.0979432, 0.271065, 0.473081, 0.635028, 0.746553, 0.815953, 0.867662};
    double fake_pt95[] = {0.0960615, 0.281036, 0.470885, 0.639801, 0.741431, 0.816458, 0.867543};
    double fake_pt110[] = {0.0702811, 0.2498, 0.471088, 0.63904, 0.747189, 0.822958, 0.866763};
    double fake_pt125[] = {0.112595, 0.283525, 0.471517, 0.643951, 0.745971, 0.820508, 0.872843};
    double noise_d[] = {3, 6, 9, 12, 15, 18, 21};

    auto c1 = new TCanvas("c1", "c1");
    auto c2 = new TCanvas("c2", "c2");
    auto c3 = new TCanvas("c3", "c3");
    auto c4 = new TCanvas("c4", "c4");
    TGraphErrors g1(15, pts, eff_noise3, 0, 0);
    TGraphErrors g2(15, pts, fake_noise3, 0, 0);
    g1.SetTitle("noise 3");
    g2.SetTitle("noise 3");
    g1.SetMarkerStyle(EMarkerStyle::kFullCircle);
    g2.SetMarkerStyle(EMarkerStyle::kFullCircle);
    g1.GetXaxis()->SetTitle("Pt [MeV]");
    g1.GetYaxis()->SetTitle("eff");
    g2.GetXaxis()->SetTitle("Pt [MeV]");
    g2.GetYaxis()->SetTitle("fake rate");
    c1->cd();
    g1.Draw();
    c1->SaveAs("./eff_noise3.png");
    c1->Close();
    c2->cd();
    g2.Draw();
    c2->SaveAs("./fake_noise3.png");
    c2->Close();

    TGraphErrors g3(7, noise_d, eff_pt65, 0, 0);
    TGraphErrors g4(7, noise_d, fake_pt65, 0, 0);
    g3.SetTitle("Pt == 65 MeV");
    g4.SetTitle("Pt == 65 MeV");
    g3.SetMarkerStyle(EMarkerStyle::kFullCircle);
    g4.SetMarkerStyle(EMarkerStyle::kFullCircle);
    g3.GetXaxis()->SetTitle("number of noise points");
    g3.GetYaxis()->SetTitle("eff");
    g4.GetXaxis()->SetTitle("number of noise points");
    g4.GetYaxis()->SetTitle("fake rate");
    c3->cd();
    g3.Draw();
    c3->SaveAs("./eff_pt65.png");
    c3->Close();
    c4->cd();
    g4.Draw();
    c4->SaveAs("./fake_pt65.png");
    c4->Close();
    delete c1;
    delete c2;
    delete c3;
    delete c4;
}