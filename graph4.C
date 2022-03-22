#include "TFile.h"
#include "TCanvas.h"
#include "TH1F.h"
#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include "TGraphErrors.h"

void graph4()
{
    const int noise[] = {3, 6, 9, 12, 15, 18, 21};
    const double fnoise[] = {3., 6., 9., 12., 15., 18., 21.};
    const std::string noiseStr[] = {"3", "6", "9", "12", "15", "18", "21"};
    const std::string pt_str[] = {"65", "75", "85", "95", "105", "115", "125", "135"};
    const int N = 7;
    std::map<std::string, double> pt_total;
    pt_total.insert(std::pair<std::string, double>("65", 939));
    pt_total.insert(std::pair<std::string, double>("75", 952));
    pt_total.insert(std::pair<std::string, double>("85", 952));
    pt_total.insert(std::pair<std::string, double>("95", 974));
    pt_total.insert(std::pair<std::string, double>("105", 968));
    pt_total.insert(std::pair<std::string, double>("115", 972));
    pt_total.insert(std::pair<std::string, double>("125", 970));
    pt_total.insert(std::pair<std::string, double>("135", 973));

    auto item = pt_total.find(pt_str[0]);
    double total = item->second;

    std::string inpath = "./PtRe" + pt_str[0] + ".txt";
    std::ifstream infile(inpath.c_str());
    char buff[256];
    std::string title = "sucesses rate @" + pt_str[0] + " MeV/c with noise";
    std::string savepath = "./graph/sucesses" + pt_str[0] + "_noise.png";
    std::cout << title.c_str() << std::endl;
    std::cout << savepath.c_str() << std::endl;
    auto c1 = new TCanvas("c1", "");

    double good[N];
    double rate[N];
    double error[N];
    for (int i = 0; i < N; i++)
        good[i] = 0;
    int eventID, n_noise, npoints, npoints_true;
    double pt;
    bool track_success;
    std::string track_is_success;
    while (!infile.eof())
    {
        infile >> eventID >> n_noise >> track_is_success >> npoints_true >> npoints >> pt;
        for (int i = 0; i < N; i++)
        {
            if (n_noise == noise[i])
            {
                if (track_is_success == "true")
                {
                    track_success = true;
                    good[i]++;
                }
                else if (track_is_success == "false")
                {
                    track_success = false;
                }
            }
        }
    }
    // auto g = new TGraphErrors();
    for (int i = 0; i < N; i++)
    {
        rate[i] = good[i] / total;
        error[i] = sqrt(rate[i] * (1 - rate[i]) * total) / total;
        std::cout << good[i] << std::endl;
        // g->SetPoint(i, noise[i], rate[i]);
    }
    auto g = new TGraphErrors(N, fnoise, (const double *)rate, 0, (const double *)error);
    c1->cd();
    g->SetTitle(title.c_str());
    g->GetXaxis()->SetTitle("noise points");
    g->GetXaxis()->CenterTitle();
    g->SetMarkerStyle(EMarkerStyle::kFullCircle);
    g->GetYaxis()->SetRangeUser(0.2, 1);
    g->GetYaxis()->SetTitle("success rate");
    g->GetYaxis()->CenterTitle();
    g->Draw();
    c1->SaveAs(savepath.c_str());
}
