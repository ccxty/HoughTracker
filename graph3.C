#include "TFile.h"
#include "TCanvas.h"
#include "TH1F.h"
#include <iostream>
#include <fstream>
#include <string>

void graph3()
{
    int noise[] = {3, 6, 9, 12, 15, 18, 21};
    std::string noiseStr[] = {"3", "6", "9", "12", "15", "18", "21"};

    for (int j = 0; j < 7; ++j)
    {
        std::string inpath = "./PtRe65.txt";
        std::ifstream infile(inpath.c_str());
        char buff[256];
        std::string line;
        std::string title = "Pt_Re - Pt_true @65. MeV/c with noise_points ";
        std::string savepath = "./graph/PtRe65.noise";
        title.append(noiseStr[j]);
        savepath.append(noiseStr[j]);
        savepath.append(".png");
        std::cout << title.c_str() << std::endl;
        std::cout << savepath.c_str() << std::endl;
        auto c1 = new TCanvas("c1", "");
        auto h1 = new TH1F("h1", title.c_str(), 100, -50, 50);

        int eventID, n_noise, npoints, npoints_true;
        double pt;
        bool track_success;
        std::string track_is_success;
        while (!infile.eof())
        {
            infile >> eventID >> n_noise >> track_is_success >> npoints_true >> npoints >> pt;
            if (track_is_success == "true")
            {
                track_success = true;
            }
            else if (track_is_success == "false")
            {
                track_success = false;
            }
            if (n_noise == noise[j])
            {
                h1->Fill(pt - 65.);
            }
        }
        h1->GetXaxis()->SetTitle("Pt [MeV/c]");
        h1->GetXaxis()->CenterTitle();
        h1->GetYaxis()->SetTitle("N");
        h1->GetYaxis()->CenterTitle();
        c1->cd();
        h1->Draw();
        c1->SaveAs(savepath.c_str());
        std::cout << h1->GetEntries() << std::endl;
    }
}
