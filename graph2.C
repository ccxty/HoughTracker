#include <iostream>
#include "TCanvas.h"
#include "TGraph.h"
#include "TAxis.h"

void graph2()
{
    auto c1 = new TCanvas("c1", "c1");
    int N = 15;
    double pt[] = {65, 70, 75, 80,
                   85, 90, 95, 100,
                   105, 110, 115, 120,
                   125, 130, 135};
    double total[] = {939, 953, 952, 950,
                      952, 964, 974, 943,
                      968, 965, 972, 969,
                      970, 974, 973};
    double good[] = {922, 936, 934, 934,
                     937, 947, 961, 933,
                     956, 946, 957, 951,
                     950, 965, 961};
    double rate[N];
    for (int i = 0; i < N; i++)
    {
        rate[i] = good[i] / total[i];
    }
    auto g = new TGraph(N, pt, rate);
    c1->cd();
    g->GetXaxis()->SetTitle("Pt [MeV/c]");
    g->GetXaxis()->CenterTitle();
    g->SetMarkerStyle(EMarkerStyle::kFullCircle);
    g->GetYaxis()->SetRangeUser(0.95, 1);
    g->GetYaxis()->SetTitle("success rate");
    g->GetYaxis()->CenterTitle();
    g->Draw();
    c1->SaveAs("./graph/sucesses_rate_pt.png");
}