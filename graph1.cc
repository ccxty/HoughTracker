#include <iostream>
#include <fstream>
#include "TCanvas.h"
#include "TH1F.h"
void graph1()
{
    std::string inpath = "./PtRe65.txt";
    std::ifstream infile(inpath.c_str());
    char buff[256];

    auto c1 = new TCanvas("c1", "Pt");
    auto h1 = new TH1F("h1", "Pt_Re - Pt_true @65. MeV/c", 60, -30, 30);
    while (infile.getline(buff, 256))
    {
        double pt = atof(buff);
        h1->Fill(pt - 65.);
    }
    h1->GetXaxis()->SetTitle("Pt [MeV/c]");
    h1->GetXaxis()->CenterTitle();
    h1->GetYaxis()->SetTitle("N");
    h1->GetYaxis()->CenterTitle();
    c1->cd();
    h1->Draw();
    c1->SaveAs("./graph/PtRe65.png");
    std::cout << h1->GetEntries() << std::endl;
    return;
}