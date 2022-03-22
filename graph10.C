#include <iostream>
#include "TCanvas.h"
#include "TGraphErrors.h"
#include "TAxis.h"

void graph10()
{
    double pts[] = {65, 75, 85, 95, 110, 125, 135};
    double noise_d[] = {0, 3, 6, 9, 12, 15, 18, 21};
    double multi_d[] = {2, 3, 4};

    double fake_pt65_noise0[] = {0.0442105, 0.108738, 0.163004};
    double fake_pt65_noise6[] = {0.309542, 0.363191, 0.434243};
    double fake_pt65_noise12[] = {0.60043, 0.599564, 0.620008};
    double fake_pt65_noise18[] = {0.766792, 0.748273, 0.743719};

    double fake_noise0_multi3[] = {0.108738, 0.108949, 0.100684, 0.0983763,
                                   0.105714, 0.107345, 0.101207};
    double fake_noise0_multi4[] = {0.163004, 0.202257, 0.174888, 0.175652,
                                   0.143898, 0.178883, 0.155399};

    double fake_pt65_multi3[] = {0.108738, 0.223855, 0.363191, 0.483184,
                                 0.599564, 0.680265, 0.748273, 0.796486};
    double fake_pt135_multi3[] = {0.101207, 0.221683, 0.353764, 0.486316,
                                  0.584158, 0.670348, 0.734294, 0.781975};
    auto c1 = new TCanvas("c1", "c1");
    auto c2 = new TCanvas("c2", "c2");
    auto c3 = new TCanvas("c3", "c3");
    TGraphErrors g1(3, multi_d, fake_pt65_noise18, 0, 0);
    TGraphErrors g2(7, pts, fake_noise0_multi4, 0, 0);
    TGraphErrors g3(8, noise_d, fake_pt135_multi3, 0, 0);
    g1.SetTitle("noise 18, Pt == 65 MeV");
    g1.GetXaxis()->SetTitle("num of tracks");
    g1.GetYaxis()->SetTitle("fake rate");
    g1.SetMarkerStyle(EMarkerStyle::kFullCircle);
    g2.SetTitle("noise 0, 4 tracks");
    g2.GetXaxis()->SetTitle("Pt [MeV]");
    g2.GetYaxis()->SetTitle("fake rate");
    g2.SetMarkerStyle(EMarkerStyle::kFullCircle);
    g3.SetTitle("3 tracks, Pt == 135 MeV");
    g3.GetXaxis()->SetTitle("num of noise points");
    g3.GetYaxis()->SetTitle("fake rate");
    g3.SetMarkerStyle(EMarkerStyle::kFullCircle);
    c1->cd();
    g1.Draw();
    c1->SaveAs("./fake_multi_pt65_noise18.png");
    c1->Close();
    c2->cd();
    g2.Draw();
    c2->SaveAs("./fake_multi4_noise0.png");
    c2->Close();
    c3->cd();
    g3.Draw();
    c3->SaveAs("./fake_multi3_pt135.png");
    c3->Close();
    delete c1;
    delete c2;
    delete c3;
}