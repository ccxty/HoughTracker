#include <fstream>
#include <iostream>
#include <set>
#include <string>
#include <tuple>
#include <vector>

#include "TAxis.h"
#include "TCanvas.h"
#include "TFile.h"
#include "TGraphErrors.h"
#include "TMultiGraph.h"
#include "TTree.h"

using std::array;
using std::string;
using std::tuple;
using std::vector;

const array<string, 6> particle_str = {"e-", "mu-", "pi-", "e+", "mu+", "pi+"};
const array<string, 18> pt_str = {"50",  "55",  "60",  "65",  "70",  "75",
                                  "80",  "85",  "90",  "95",  "100", "105",
                                  "110", "115", "120", "125", "130", "135"};
const array<double, 18> pt_double = {50,  55,  60,  65,  70,  75,
                                     80,  85,  90,  95,  100, 105,
                                     110, 115, 120, 125, 130, 135};
const array<string, 13> noise_str = {"0",  "6",  "12", "18", "24", "30", "39",
                                     "48", "57", "66", "75", "85", "100"};
const array<double, 13> noise_double = {0,  6,  12, 18, 24, 30, 39,
                                        48, 57, 66, 75, 85, 100};
const array<string, 3> multi_str = {"1", "3", "5"};
const array<double, 3> multi_double = {1, 3, 5};
const array<EColor, 6> particle_color = {kBlack,  kViolet, kRed,
                                         kYellow, kBlue,   kGreen};
const array<string, 9> deg_str = {"90",      "84.2608", "78.463",
                                  "72.5424", "66.4218", "60",
                                  "53.1301", "45.573",  "36.8699"};
const array<double, 9> deg_double = {90, 84.2608, 78.463, 72.5424, 66.4218,
                                     60, 53.1301, 45.573, 36.8699};
const array<EColor, 3> multi_color = {kBlack, kRed, kViolet};
const array<EColor, 4> pt_color = {kBlack, kRed, kViolet, kGray};
const array<EColor, 5> noise_color = {kBlack, kRed, kViolet, kBlue, kGray};
const array<EColor, 3> deg_color = {kBlack, kRed, kViolet};

string get_file_name(int i_particle, int i_pt, int i_noise, int i_multi,
                     int i_deg, string &program_name) {
    string file_name = "../data/20220502/" + program_name + "Tracker/";
    file_name += particle_str[i_particle] + "/trackdata_Pt";
    file_name += pt_str[i_pt] + "_noise";
    file_name += noise_str[i_noise] + "_multi";
    file_name += multi_str[i_multi] + "_deg";
    file_name += deg_str[i_deg] + ".root";
    return file_name;
}

array<array<double, 2>, 2> get_eff_fake(TTree *tree) {
    int eventID = 0;
    int trackID = 0;
    double DeltaDXY = NAN;
    double pt = NAN;
    double kz = NAN;
    double kz_true = NAN;
    double Qz = NAN;
    double Qe = NAN;
    int num_total = 0;
    bool true_track = false;
    tree->SetBranchAddress("eventID", &eventID);
    tree->SetBranchAddress("trackID", &trackID);
    tree->SetBranchAddress("DeltaDXY", &DeltaDXY);
    tree->SetBranchAddress("pt", &pt);
    tree->SetBranchAddress("kz", &kz);
    tree->SetBranchAddress("kz_true", &kz_true);
    tree->SetBranchAddress("Qz", &Qz);
    tree->SetBranchAddress("Qe", &Qe);
    tree->SetBranchAddress("num_total", &num_total);
    tree->SetBranchAddress("true_track", &true_track);
    int n_eff = 0;
    int n_true = 0;
    std::set<int> event_ids;
    auto n_all = static_cast<double>(tree->GetEntries());
    for (Long64_t ie = 0; ie < tree->GetEntries(); ie++) {
        tree->GetEntry(ie);
        if (eventID != -1) {
            event_ids.insert(eventID);
            n_true++;
        }
    }
    n_eff = event_ids.size();
    double eff = static_cast<double>(n_eff) / 10000.;
    if (n_eff == 0) {
        eff = 0.;
    }
    double eff_err = sqrt(eff * (1 - eff) * 10000.) / 10000.;
    double fake = 1 - static_cast<double>(n_true) / static_cast<double>(n_all);
    if (n_true == 0) {
        fake = 1.;
    }
    double fake_err = sqrt(fake * (1 - fake) * n_all) / n_all;
    array<array<double, 2>, 2> result{};
    result[0][0] = eff;
    result[0][1] = eff_err;
    result[1][0] = fake;
    result[1][1] = fake_err;
    return result;
}

void graph_pt_deg(string &program_name) {
    int i_particle = 0;
    int i_noise = 9;
    int i_multi = 0;
    TMultiGraph *effs = new TMultiGraph("effs", "eff    e- with 66 noise");
    TMultiGraph *fakes =
        new TMultiGraph("fakes", "fake    e- rate with 66 noise");
    TCanvas *c1 = new TCanvas("c1", "c1");
    TCanvas *c2 = new TCanvas("c2", "c2");
    for (int i_deg = 0; i_deg < 9; i_deg += 4) {
        array<double, 18> eff;
        array<double, 18> eff_err;
        array<double, 18> fake;
        array<double, 18> fake_err;
        for (int i_pt = 0; i_pt < 18; i_pt++) {
            string file_name = get_file_name(i_particle, i_pt, i_noise, i_multi,
                                             i_deg, program_name);
            auto *file = new TFile(file_name.c_str());
            auto *tree = dynamic_cast<TTree *>(gDirectory->Get("tree1"));
            auto result = get_eff_fake(tree);
            eff[i_pt] = result[0][0];
            eff_err[i_pt] = result[0][1];
            fake[i_pt] = result[1][0];
            fake_err[i_pt] = result[1][1];
        }
        auto *g_eff = new TGraphErrors(18, pt_double.data(), eff.data(),
                                       nullptr, eff_err.data());
        auto *g_fake = new TGraphErrors(18, pt_double.data(), fake.data(),
                                        nullptr, fake_err.data());
        g_eff->SetLineColor(deg_color[i_deg / 4]);
        g_fake->SetLineColor(deg_color[i_deg / 4]);
        g_eff->SetTitle(("degree " + deg_str[i_deg]).c_str());
        g_fake->SetTitle(("degree " + deg_str[i_deg]).c_str());
        effs->Add(g_eff);
        fakes->Add(g_fake);
    }
    effs->GetXaxis()->SetTitle("p_{T} [MeV]");
    effs->GetYaxis()->SetTitle("efficiency");
    effs->GetYaxis()->SetRangeUser(0.9, 1.0);
    fakes->GetXaxis()->SetTitle("p_{T} [MeV]");
    fakes->GetYaxis()->SetTitle("fake rate");
    fakes->GetYaxis()->SetRangeUser(0.0, 0.4);
    c1->cd();
    effs->Draw("AC");
    c1->BuildLegend();
    c1->SaveAs(("eff_pt_deg_" + program_name + ".png").c_str());
    c2->cd();
    fakes->Draw("AC");
    c2->BuildLegend();
    c2->SaveAs(("fake_pt_deg_" + program_name + ".png").c_str());
}

void graph_noise_deg(string &program_name) {
    int i_particle = 0;
    int i_pt = 0;
    int i_multi = 0;
    TMultiGraph *effs = new TMultiGraph("effs", "eff    e^{-}@50MeV");
    TMultiGraph *fakes = new TMultiGraph("fakes", "fake rate    e^{-}@50MeV");
    TCanvas *c1 = new TCanvas("c1", "c1");
    TCanvas *c2 = new TCanvas("c2", "c2");
    for (int i_deg = 0; i_deg < 9; i_deg += 4) {
        array<double, 13> eff;
        array<double, 13> eff_err;
        array<double, 13> fake;
        array<double, 13> fake_err;
        for (int i_noise = 0; i_noise < 13; i_noise++) {
            string file_name = get_file_name(i_particle, i_pt, i_noise, i_multi,
                                             i_deg, program_name);
            auto *file = new TFile(file_name.c_str());
            auto *tree = dynamic_cast<TTree *>(gDirectory->Get("tree1"));
            auto result = get_eff_fake(tree);
            eff[i_noise] = result[0][0];
            eff_err[i_noise] = result[0][1];
            fake[i_noise] = result[1][0];
            fake_err[i_noise] = result[1][1];
        }
        auto *g_eff = new TGraphErrors(13, noise_double.data(), eff.data(),
                                       nullptr, eff_err.data());
        auto *g_fake = new TGraphErrors(13, noise_double.data(), fake.data(),
                                        nullptr, fake_err.data());
        g_eff->SetLineColor(deg_color[i_deg / 4]);
        g_fake->SetLineColor(deg_color[i_deg / 4]);
        g_eff->SetTitle(("degree " + deg_str[i_deg]).c_str());
        g_fake->SetTitle(("degree " + deg_str[i_deg]).c_str());
        effs->Add(g_eff);
        fakes->Add(g_fake);
    }
    effs->GetXaxis()->SetTitle("noise");
    effs->GetYaxis()->SetTitle("efficiency");
    effs->GetYaxis()->SetRangeUser(0.9, 1.0);
    fakes->GetXaxis()->SetTitle("noise");
    fakes->GetYaxis()->SetTitle("fake rate");
    fakes->GetYaxis()->SetRangeUser(0.0, 0.4);
    c1->cd();
    effs->Draw("AC");
    c1->BuildLegend();
    c1->SaveAs(("eff_noise_deg_" + program_name + ".png").c_str());
    c2->cd();
    fakes->Draw("AC");
    c2->BuildLegend();
    c2->SaveAs(("fake_noise_deg_" + program_name + ".png").c_str());
}

void graph_pt_particle(string &program_name) {
    int i_noise = 9;
    int i_multi = 0;
    int i_deg = 0;
    TMultiGraph *effs = new TMultiGraph("effs", "eff    with 66 noise @90deg");
    TMultiGraph *fakes =
        new TMultiGraph("fakes", "fake rate    with 66 noise @90deg");
    TCanvas *c1 = new TCanvas("c1", "c1");
    TCanvas *c2 = new TCanvas("c2", "c2");
    for (int i_particle = 0; i_particle < 3; i_particle++) {
        array<double, 18> eff;
        array<double, 18> eff_err;
        array<double, 18> fake;
        array<double, 18> fake_err;
        for (int i_pt = 0; i_pt < 18; i_pt++) {
            string file_name = get_file_name(i_particle, i_pt, i_noise, i_multi,
                                             i_deg, program_name);
            auto *file = new TFile(file_name.c_str());
            auto *tree = dynamic_cast<TTree *>(gDirectory->Get("tree1"));
            auto result = get_eff_fake(tree);
            eff[i_pt] = result[0][0];
            eff_err[i_pt] = result[0][1];
            fake[i_pt] = result[1][0];
            fake_err[i_pt] = result[1][1];
        }
        auto *g_eff = new TGraphErrors(18, pt_double.data(), eff.data(),
                                       nullptr, eff_err.data());
        auto *g_fake = new TGraphErrors(18, pt_double.data(), fake.data(),
                                        nullptr, fake_err.data());
        g_eff->SetLineColor(particle_color[i_particle]);
        g_fake->SetLineColor(particle_color[i_particle]);
        g_eff->SetTitle(particle_str[i_particle].c_str());
        g_fake->SetTitle(particle_str[i_particle].c_str());
        effs->Add(g_eff);
        fakes->Add(g_fake);
    }
    effs->GetXaxis()->SetTitle("p_{T} [MeV]");
    effs->GetYaxis()->SetTitle("efficiency");
    effs->GetYaxis()->SetRangeUser(0.75, 1.0);
    fakes->GetXaxis()->SetTitle("p_{T} [MeV]");
    fakes->GetYaxis()->SetTitle("fake rate");
    fakes->GetYaxis()->SetRangeUser(0.0, 0.3);
    c1->cd();
    effs->Draw("AC");
    c1->BuildLegend();
    c1->SaveAs(("eff_pt_particle_" + program_name + ".png").c_str());
    c2->cd();
    fakes->Draw("AC");
    c2->BuildLegend();
    c2->SaveAs(("fake_pt_particle_" + program_name + ".png").c_str());
}

void graph_noise_particle(string &program_name) {
    int i_pt = 2;
    int i_multi = 0;
    int i_deg = 0;
    TMultiGraph *effs = new TMultiGraph("effs", "eff    @60MeV, 90deg");
    TMultiGraph *fakes = new TMultiGraph("fakes", "fake rate    @60MeV, 90deg");
    TCanvas *c1 = new TCanvas("c1", "c1");
    TCanvas *c2 = new TCanvas("c2", "c2");
    for (int i_particle = 0; i_particle < 3; i_particle++) {
        array<double, 13> eff;
        array<double, 13> eff_err;
        array<double, 13> fake;
        array<double, 13> fake_err;
        for (int i_noise = 0; i_noise < 13; i_noise++) {
            string file_name = get_file_name(i_particle, i_pt, i_noise, i_multi,
                                             i_deg, program_name);
            auto *file = new TFile(file_name.c_str());
            auto *tree = dynamic_cast<TTree *>(gDirectory->Get("tree1"));
            auto result = get_eff_fake(tree);
            eff[i_noise] = result[0][0];
            eff_err[i_noise] = result[0][1];
            fake[i_noise] = result[1][0];
            fake_err[i_noise] = result[1][1];
        }
        auto *g_eff = new TGraphErrors(13, noise_double.data(), eff.data(),
                                       nullptr, eff_err.data());
        auto *g_fake = new TGraphErrors(13, noise_double.data(), fake.data(),
                                        nullptr, fake_err.data());
        g_eff->SetLineColor(particle_color[i_particle]);
        g_fake->SetLineColor(particle_color[i_particle]);
        g_eff->SetTitle(particle_str[i_particle].c_str());
        g_fake->SetTitle(particle_str[i_particle].c_str());
        effs->Add(g_eff);
        fakes->Add(g_fake);
    }
    effs->GetXaxis()->SetTitle("noise");
    effs->GetYaxis()->SetTitle("efficiency");
    effs->GetYaxis()->SetRangeUser(0.75, 1.0);
    fakes->GetXaxis()->SetTitle("noise");
    fakes->GetYaxis()->SetTitle("fake rate");
    fakes->GetYaxis()->SetRangeUser(0.0, 0.4);
    c1->cd();
    effs->Draw("AC");
    c1->BuildLegend();
    c1->SaveAs(("eff_noise_particle_" + program_name + ".png").c_str());
    c2->cd();
    fakes->Draw("AC");
    c2->BuildLegend();
    c2->SaveAs(("fake_noise_particle_" + program_name + ".png").c_str());
}

void graph_deg_multi(string program_name) {
    int i_particle = 0;
    int i_noise = 9;
    int i_pt = 0;
    TMultiGraph *effs = new TMultiGraph("effs", "eff    e^{-}@50MeV,66 noise");
    TMultiGraph *fakes =
        new TMultiGraph("fakes", "fake rate    e^{-}@50MeV,66 noise");
    TCanvas *c1 = new TCanvas("c1", "c1");
    TCanvas *c2 = new TCanvas("c2", "c2");
    for (int i_multi = 0; i_multi < 3; i_multi++) {
        array<double, 9> eff;
        array<double, 9> eff_err;
        array<double, 9> fake;
        array<double, 9> fake_err;
        for (int i_deg = 0; i_deg < 9; i_deg++) {
            string file_name = get_file_name(i_particle, i_pt, i_noise, i_multi,
                                             i_deg, program_name);
            auto *file = new TFile(file_name.c_str());
            auto *tree = dynamic_cast<TTree *>(gDirectory->Get("tree1"));
            auto result = get_eff_fake(tree);
            eff[i_deg] = result[0][0];
            eff_err[i_deg] = result[0][1];
            fake[i_deg] = result[1][0];
            fake_err[i_deg] = result[1][1];
        }
        std::cout << std::endl;
        auto *g_eff = new TGraphErrors(9, deg_double.data(), eff.data(),
                                       nullptr, eff_err.data());
        auto *g_fake = new TGraphErrors(9, deg_double.data(), fake.data(),
                                        nullptr, fake_err.data());
        g_eff->SetLineColor(multi_color[i_multi]);
        g_fake->SetLineColor(multi_color[i_multi]);
        g_eff->SetTitle((multi_str[i_multi] + "tracks").c_str());
        g_fake->SetTitle((multi_str[i_multi] + " tracks").c_str());
        effs->Add(g_eff);
        fakes->Add(g_fake);
    }
    effs->GetXaxis()->SetTitle("degree");
    effs->GetYaxis()->SetTitle("efficiency");
    effs->GetYaxis()->SetRangeUser(0.85, 1.0);
    fakes->GetXaxis()->SetTitle("degree");
    fakes->GetYaxis()->SetTitle("fake rate");
    fakes->GetYaxis()->SetRangeUser(0.0, 0.4);
    c1->cd();
    effs->Draw("AC");
    c1->BuildLegend();
    c1->SaveAs(("eff_deg_multi_" + program_name + ".png").c_str());
    c2->cd();
    fakes->Draw("AC");
    c2->BuildLegend();
    c2->SaveAs(("fake_deg_multi_" + program_name + ".png").c_str());
}

void analysis5() {
    array<string, 2> program_name = {"Hough", "Local"};
    for (string &program : program_name) {
        graph_pt_deg(program);
        graph_noise_deg(program);
        graph_pt_particle(program);
        graph_noise_particle(program);
        graph_deg_multi(program);
    }
}