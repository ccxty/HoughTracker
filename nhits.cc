#include <array>
#include <iostream>
#include <ostream>
#include <string>
#include <vector>

#include "Rtypes.h"
#include "RtypesCore.h"
#include "TCanvas.h"
#include "TFile.h"
#include "TGraphErrors.h"
#include "TH1.h"
#include "TH2.h"
#include "TMarker.h"
#include "TMultiGraph.h"
#include "TTree.h"

using std::array;
using std::cout;
using std::endl;
using std::string;
using std::vector;

const array<string, 6> particle_str = {"e-", "mu-", "pi-", "e+", "mu+", "pi+"};
const array<string, 9> deg_str = {"90",      "84.2608", "78.463",
                                  "72.5424", "66.4218", "60",
                                  "53.1301", "45.573",  "36.8699"};
const array<double, 9> deg_double = {90, 84.2608, 78.463, 72.5424, 66.4218,
                                     60, 53.1301, 45.573, 36.8699};
const array<string, 18> pt_str = {"50",  "55",  "60",  "65",  "70",  "75",
                                  "80",  "85",  "90",  "95",  "100", "105",
                                  "110", "115", "120", "125", "130", "135"};
const array<double, 18> pt_double = {50,  55,  60,  65,  70,  75,
                                     80,  85,  90,  95,  100, 105,
                                     110, 115, 120, 125, 130, 135};
const array<EColor, 4> pt_color = {kBlack, kRed, kViolet, kGray};
const array<EColor, 3> deg_color = {kBlack, kRed, kViolet};
const array<EColor, 3> particle_color = {kBlack, kRed, kViolet};

string get_file_name(int i_particle, int i_pt, int i_deg) {
    return "./root_data_source/" + particle_str[i_particle] + "/posPt" +
           pt_str[i_pt] + "_deg" + deg_str[i_deg] + ".root";
}

array<double, 2> get_nhits(TTree* tree) {
    int eventID = 0;
    vector<int>* trackID = nullptr;
    vector<int>* layerID = nullptr;
    tree->SetBranchAddress("eventID", &eventID);
    tree->SetBranchAddress("trackID", &trackID);
    tree->SetBranchAddress("layerID", &layerID);
    array<double, 2> result{};
    auto n = tree->GetEntries();
    double sum_x = 0;
    double sum_xx = 0;
    for (auto ie = 0; ie < n; ie++) {
        tree->GetEntry(ie);
        int nhits = 0;
        for (auto track_id : *trackID) {
            if (track_id == 1) {
                nhits++;
            }
        }
        sum_x += nhits;
        sum_xx += nhits * nhits;
    }
    result[0] = sum_x / static_cast<double>(n);
    result[1] = sqrt(sum_xx / static_cast<double>(n) - result[0] * result[0]);
    return result;
}

void nhits_pt_particle(int i_deg) {
    auto* c1 = new TCanvas("c1", "c1");
    auto* g = new TMultiGraph("g", "nhits @90deg");
    c1->cd();
    for (int i_particle = 0; i_particle < 3; i_particle++) {
        array<double, 18> nhits{};
        array<double, 18> nhits_err{};
        for (int i_pt = 0; i_pt < 18; i_pt++) {
            string file_name = get_file_name(i_particle, i_pt, i_deg);
            auto* file = new TFile(file_name.c_str());
            TTree* tree = dynamic_cast<TTree*>(gDirectory->Get("tree1"));
            auto result = get_nhits(tree);
            nhits[i_pt] = result[0];
            nhits_err[i_pt] = result[1];
        }
        for (int i = 0; i < 18; i++) {
            cout << nhits[i] << "\t" << nhits_err[i] << endl;
        }
        cout << endl;
        auto* g_particle = new TGraphErrors(18, pt_double.data(), nhits.data(),
                                            nullptr, nhits_err.data());
        g_particle->SetLineColor(particle_color[i_particle]);
        g_particle->SetTitle(particle_str[i_particle].c_str());
        g->Add(g_particle);
    }
    g->GetXaxis()->SetTitle("Pt [MeV/c]");
    g->GetYaxis()->SetTitle("nhits");
    g->Draw("AC");
    c1->BuildLegend();
    c1->SaveAs("nhits_pt_particle.png");
}

void nhits_pt_deg() {}

void nhits_deg_particle(int i_pt) {
    auto* c1 = new TCanvas("c1", "c1");
    auto* g = new TMultiGraph("g", "nhits @60MeV/c");
    for (int i_particle = 0; i_particle < 3; i_particle++) {
        array<double, 9> nhits{};
        array<double, 9> nhits_err{};
        for (int i_deg = 0; i_deg < 9; i_deg++) {
            string file_name = get_file_name(i_particle, i_pt, i_deg);
            auto* file = new TFile(file_name.c_str());
            TTree* tree = dynamic_cast<TTree*>(gDirectory->Get("tree1"));
            auto result = get_nhits(tree);
            nhits[i_deg] = result[0];
            nhits_err[i_deg] = result[1];
        }
        auto* g_particle = new TGraphErrors(9, deg_double.data(), nhits.data(),
                                            nullptr, nhits_err.data());
        g_particle->SetLineColor(particle_color[i_particle]);
        g_particle->SetTitle(particle_str[i_particle].c_str());
        g->Add(g_particle);
    }
    g->GetXaxis()->SetTitle("deg");
    g->GetYaxis()->SetTitle("nhits");
    g->Draw("AC");
    c1->BuildLegend();
    c1->SaveAs("nhits_deg_particle.png");
}

void nhits_deg_pt() {}

void nhits_distribution(int i_particle, int i_pt, int i_deg) {
    auto* c1 = new TCanvas("c1", "c1");
    string file_name = get_file_name(i_particle, i_pt, i_deg);
    auto* file = new TFile(file_name.c_str());
    TTree* tree = dynamic_cast<TTree*>(gDirectory->Get("tree1"));
    int eventID = 0;
    std::vector<int>* trackID = nullptr;
    tree->SetBranchAddress("eventID", &eventID);
    tree->SetBranchAddress("trackID", &trackID);
    TH1F* h1 = new TH1F("h1", "h1", 50, 0, 50);
    auto nevents = tree->GetEntries();
    for (auto ie = 0; ie < nevents; ie++) {
        tree->GetEntry(ie);
        int nhits = 0;
        for (auto track_id : *trackID) {
            if (track_id == 1) {
                nhits++;
            }
        }
        h1->Fill(nhits);
    }
    c1->cd();
    h1->Draw();
    c1->SaveAs("nhits_distribution.png");
}

void nhits() {
    nhits_pt_particle(0);
    nhits_pt_deg();
    nhits_deg_particle(15);
    nhits_deg_pt();
    // nhits_distribution(0,0,0);
}
