#include "TFile.h"
#include "TCanvas.h"
#include "TH1F.h"
#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include "TGraphErrors.h"
#include "TTree.h"
#include "TMultiGraph.h"

using namespace std;

void graph6()
{
    string in_str = "./fake_rate.txt";
    ifstream in;
    double total[] = {939, 953, 952, 950,
                      952, 964, 974, 943,
                      968, 965, 972, 969,
                      970, 974, 973};
    double pts[] = {65, 70, 75, 80, 85, 90, 95, 100,
                    105, 110, 115, 120, 125, 130, 135};
    string pt_str[] = {"65", "70", "75", "80",
                       "85", "90", "95", "100",
                       "105", "110", "115", "120",
                       "125", "130", "135"};
    string noise_str[] = {"3", "6", "9", "12", "15", "18", "21"};
    int noise_n[] = {3, 6, 9, 12, 15, 18, 21};
    double noise_n_d[] = {3, 6, 9, 12, 15, 18, 21};
    double pt;
    int noise;
    double eff, fake_rate;

    double effs_pt[15];
    double fakes_pt[15];
    double effs_noise[7];
    double fakes_noise[7];
    double effs_pt_err[15] = {0};
    double fakes_pt_err[15] = {0};
    double effs_noise_err[7] = {0};
    double fakes_noise_err[7] = {0};

    //
    //
    //
    TCanvas c1("c1", "eff with noise");
    TCanvas c2("c2", "fake rate with noise");
    TMultiGraph eff_pt_all("eff_with_noise", "eff with noise");
    TMultiGraph fake_pt_all("fake_rate_with_noise", "fake rate with noise");
    eff_pt_all.GetXaxis()->SetTitle("Pt [MeV/c]");
    fake_pt_all.GetXaxis()->SetTitle("Pt [MeV/c]");
    eff_pt_all.GetYaxis()->SetTitle("eff");
    fake_pt_all.GetYaxis()->SetTitle("fake rate");
    for (int i = 0; i < 7; i++)
    {
        if (i % 2 != 0)
            continue;
        in.open(in_str.c_str());
        int i1 = 0;
        while (!in.eof())
        {
            in >> pt >> noise >> eff >> fake_rate;
            // std::cout << pt << " " << noise << " " << eff << " " << fake_rate << std::endl;
            if (noise == noise_n[i])
            {
                effs_pt[i1] = eff;
                fakes_pt[i1] = fake_rate;
                i1++;
            }
        }

        in.close();
        auto eff_pt = new TGraphErrors(15, pts, effs_pt, 0, 0);
        auto fake_pt = new TGraphErrors(15, pts, fakes_pt, 0, 0);
        eff_pt->SetTitle((noise_str[i] + " noise").c_str());
        fake_pt->SetTitle((noise_str[i] + " noise").c_str());
        eff_pt_all.Add(eff_pt);
        fake_pt_all.Add(fake_pt);
        c1.cd();
        eff_pt->Draw();
        c2.cd();
        fake_pt->Draw();
    }
    c1.cd();
    eff_pt_all.GetXaxis()->SetTitle("Pt [MeV]");
    eff_pt_all.GetYaxis()->SetTitle("eff");
    eff_pt_all.Draw();
    c1.Update();
    c1.BuildLegend();
    // c1.SaveAs("eff_pt.png");
    c1.Close();
    c2.cd();
    eff_pt_all.Draw();
    c2.BuildLegend();
    // c2.SaveAs("fake_pt.png");
    c2.Close();

    //
    //
    //
    TCanvas c3("c3", "eff with noise");
    TCanvas c4("c4", "fake rate with noise");
    TMultiGraph eff_noise_all("eff_with_noise", "eff with noise");
    TMultiGraph fake_noise_all("fake_rate_with_noise", "fake rate with noise");
    eff_noise_all.GetXaxis()->SetTitle("number of noise points");
    fake_noise_all.GetXaxis()->SetTitle("number of noise points");
    eff_noise_all.GetYaxis()->SetTitle("eff");
    fake_noise_all.GetYaxis()->SetTitle("fake rate");
    for (int j = 0; j < 15; ++j)
    {
        if (j % 3 != 0)
            continue;
        in.open(in_str.c_str());
        int i2 = 0;
        while (!in.eof())
        {
            in >> pt >> noise >> eff >> fake_rate;
            if (fabs(pt - pts[j]) < 1.0e-6)
            {
                effs_noise[i2] = eff;
                fakes_noise[i2] = fake_rate;
                i2++;
            }
        }
        in.close();
        auto eff_noise = new TGraphErrors(7, noise_n_d, effs_noise, 0, 0);
        auto fake_noise = new TGraphErrors(7, noise_n_d, fakes_noise, 0, 0);
        eff_noise->SetTitle((pt_str[j] + " MeV").c_str());
        fake_noise->SetTitle((pt_str[j] + " MeV").c_str());
        eff_noise_all.Add(eff_noise);
        fake_noise_all.Add(fake_noise);
        for (auto x : fakes_noise)
        {
            std::cout << x << ", ";
        }
        std::cout << std::endl;
    }

    c3.cd();
    eff_noise_all.Draw();
    c3.BuildLegend();
    // c3.SaveAs("eff_noise.png");
    c3.Close();
    c4.cd();
    eff_noise_all.Draw();
    c4.BuildLegend();
    // c4.SaveAs("fake_noise.png");
    c4.Close();
}