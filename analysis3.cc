#include <TGraphErrors.h>
#include <TMultiGraph.h>
#include <math.h>

#include <array>
#include <cmath>
#include <fstream>
#include <iostream>

#include "Rtypes.h"
#include "TCanvas.h"
#include "TFile.h"
#include "TH1F.h"
#include "TTree.h"

using std::array;
using std::cout;
using std::endl;
using std::ifstream;
using std::string;

void analysis3() {
    // 使用的粒子：负电粒子，[0,2,4] 三种粒子
    // 使用的噪声： 0，12，30，48，66
    // 使用的动量：50，75，100，135
    array<string, 6> particle_str = {"e-", "e+", "pi-", "pi+", "mu-", "mu+"};
    array<string, 18> pt_str = {"50",  "55",  "60",  "65",  "70",  "75",
                                "80",  "85",  "90",  "95",  "100", "105",
                                "110", "115", "120", "125", "130", "135"};
    array<string, 10> noise_str = {"0",  "6",  "12", "18", "24",
                                   "30", "39", "48", "57", "66"};
    array<string, 3> multi_str = {"1", "3", "5"};
    array<string, 4> pt_select_str = {"50", "75", "100", "135"};
    array<string, 5> noise_select_str = {"0", "12", "30", "48", "66"};
    array<double, 18> pt_double = {50, 55,  60,  65,  70,  75,  80,  85,  90,
                                   95, 100, 105, 110, 115, 120, 125, 130, 135};
    array<double, 10> noise_double = {0, 6, 12, 18, 24, 30, 39, 48, 57, 66};
    array<double, 3> multi_double = {1, 3, 5};

    array<EColor, 6> particle_color = {kBlack,  kBlue,   kRed,
                                       kYellow, kViolet, kGreen};
    array<EColor, 3> multi_color = {kBlack, kRed, kViolet};
    array<EColor, 4> pt_color = {kBlack, kRed, kViolet, kGray};
    array<EColor, 5> noise_color = {kBlack, kRed, kViolet, kBlue, kGray};
    int i_pt = 0;
    int i_multi = 0;
    int i_noise = 0;
    int i_particle = 0;

    /**
     * @brief part 1
     *60 MeV 不同粒子的 eff/fake -- noise
     *3种粒子
     *
     */
    {
        i_pt = 2;
        i_multi = 0;
        auto c_1 = new TCanvas("c_1", "eff @60 MeV");
        auto c_2 = new TCanvas("c_2", "fake @60 MeV");
        auto g_1 = new TMultiGraph("g1", "eff @60 MeV");
        auto g_2 = new TMultiGraph("g2", "fake @60 MeV");
        for (i_particle = 0; i_particle < 6; i_particle += 2) {
            string total_file = "./total_" + particle_str[i_particle] + ".txt";
            ifstream in_total(total_file);
            double total = 0;
            int i_total = 0;
            while (!in_total.eof()) {
                in_total >> total;
                if (i_total == 2) {
                    break;
                } else {
                    i_total++;
                }
            }
            array<double, 10> eff{};
            array<double, 10> eff_err{};
            array<double, 10> fake{};
            array<double, 10> fake_err{};
            for (i_noise = 0; i_noise < 10; i_noise++) {
                string path =
                    "/home/ubuntu-tyxiao/work/STCF_Oscar2.0.0/HoughTracker/"
                    "data/";
                path += particle_str[i_particle] + "/trackdata_Pt" +
                        pt_str[i_pt] + "_noise" + noise_str[i_noise] +
                        "_multi" + multi_str[i_multi] + ".root";
                auto file = new TFile(path.c_str());
                auto tree = dynamic_cast<TTree *>(gDirectory->Get("tree1"));
                int event_id = 0;
                int track_id = 0;
                bool true_track = false;
                double P_t = NAN;
                double Q_xy = NAN;
                double Q_z = NAN;
                int num_true = 0;
                int num_total = 0;
                int Q_e = 0;
                tree->SetBranchAddress("event_id", &event_id);
                tree->SetBranchAddress("track_id", &track_id);
                tree->SetBranchAddress("true_track", &true_track);
                tree->SetBranchAddress("Pt", &P_t);
                tree->SetBranchAddress("Q", &Q_xy);
                tree->SetBranchAddress("Qz", &Q_z);
                tree->SetBranchAddress("num_true", &num_true);
                tree->SetBranchAddress("num_total", &num_total);
                tree->SetBranchAddress("Qe", &Q_e);
                int n_eff = 0;
                int n_true = 0;
                int event_id_skip = -1;
                auto n_all = tree->GetEntries();
                for (Long64_t ie = 0; ie < n_all; ie++) {
                    tree->GetEntry(ie);
                    if ((event_id != event_id_skip) && (event_id != -1)) {
                        event_id_skip = event_id;
                        n_eff++;
                    }
                    if (event_id != -1) {
                        n_true++;
                    }
                }
                eff[i_noise] = n_eff / total;
                fake[i_noise] = 1 - n_true / static_cast<double>(n_all);
                eff_err[i_noise] =
                    sqrt(eff[i_noise] * (1 - eff[i_noise]) * total) / total;
                fake_err[i_noise] = sqrt(fake[i_noise] * (1 - fake[i_noise]) *
                                         static_cast<double>(n_all)) /
                                    static_cast<double>(n_all);
            }
            auto g_eff = new TGraphErrors(10, noise_double.data(), eff.data(),
                                          0, eff_err.data());
            auto g_fake = new TGraphErrors(10, noise_double.data(), fake.data(),
                                           0, fake_err.data());
            g_eff->SetTitle(particle_str[i_particle].c_str());
            g_fake->SetTitle(particle_str[i_particle].c_str());
            g_eff->SetLineColor(particle_color[i_particle]);
            g_fake->SetLineColor(particle_color[i_particle]);
            g_1->Add(g_eff);
            g_2->Add(g_fake);
        }
        c_1->cd();
        g_1->GetXaxis()->SetTitle("noise");
        g_1->GetYaxis()->SetTitle("eff");
        g_1->Draw("AC");
        c_1->BuildLegend();
        // c_1->SaveAs("eff_60MeV.png");
        c_2->cd();
        g_2->GetXaxis()->SetTitle("noise");
        g_2->GetYaxis()->SetTitle("fake rate");
        g_2->Draw("AC");
        c_2->BuildLegend();
        // c_2->SaveAs("fake_60MeV.png");
    }

    /**
     * @brief part 2
     *e- 的 eff/fake 50MeV
     *不同的 multi 随noise变化
     *
     */
    {
        i_particle = 0;
        i_pt = 0;
        auto c_1 = new TCanvas("c_1", "eff @50 MeV");
        auto c_2 = new TCanvas("c_2", "fake @50 MeV");
        auto g_1 = new TMultiGraph("g1", "eff @50 MeV");
        auto g_2 = new TMultiGraph("g2", "fake @50 MeV");
        for (i_multi = 0; i_multi < 3; i_multi++) {
            string total_file = "./total_" + particle_str[i_particle] + ".txt";
            ifstream in_total(total_file);
            double total = 0;
            int i_total = 0;
            while (!in_total.eof()) {
                in_total >> total;
                if (i_total == i_pt) {
                    break;
                } else {
                    i_total++;
                }
            }
            array<double, 10> eff{};
            array<double, 10> eff_err{};
            array<double, 10> fake{};
            array<double, 10> fake_err{};
            for (i_noise = 0; i_noise < 10; i_noise++) {
                string path =
                    "/home/ubuntu-tyxiao/work/STCF_Oscar2.0.0/HoughTracker/"
                    "data/";
                path += particle_str[i_particle] + "/trackdata_Pt" +
                        pt_str[i_pt] + "_noise" + noise_str[i_noise] +
                        "_multi" + multi_str[i_multi] + ".root";
                auto file = new TFile(path.c_str());
                auto tree = dynamic_cast<TTree *>(gDirectory->Get("tree1"));
                int event_id = 0;
                int track_id = 0;
                bool true_track = false;
                double P_t = NAN;
                double Q_xy = NAN;
                double Q_z = NAN;
                int num_true = 0;
                int num_total = 0;
                int Q_e = 0;
                tree->SetBranchAddress("event_id", &event_id);
                tree->SetBranchAddress("track_id", &track_id);
                tree->SetBranchAddress("true_track", &true_track);
                tree->SetBranchAddress("Pt", &P_t);
                tree->SetBranchAddress("Q", &Q_xy);
                tree->SetBranchAddress("Qz", &Q_z);
                tree->SetBranchAddress("num_true", &num_true);
                tree->SetBranchAddress("num_total", &num_total);
                tree->SetBranchAddress("Qe", &Q_e);
                int n_eff = 0;
                int n_true = 0;
                int event_id_skip = -1;
                auto n_all = tree->GetEntries();
                for (Long64_t ie = 0; ie < n_all; ie++) {
                    tree->GetEntry(ie);
                    if ((event_id != event_id_skip) && (event_id != -1)) {
                        event_id_skip = event_id;
                        n_eff++;
                    }
                    if (event_id != -1) {
                        n_true++;
                    }
                }
                eff[i_noise] = n_eff / total;
                fake[i_noise] = 1 - n_true / static_cast<double>(n_all);
                eff_err[i_noise] =
                    sqrt(eff[i_noise] * (1 - eff[i_noise]) * total) / total;
                fake_err[i_noise] = sqrt(fake[i_noise] * (1 - fake[i_noise]) *
                                         static_cast<double>(n_all)) /
                                    static_cast<double>(n_all);
            }
            auto g_eff = new TGraphErrors(10, noise_double.data(), eff.data(),
                                          0, eff_err.data());
            auto g_fake = new TGraphErrors(10, noise_double.data(), fake.data(),
                                           0, fake_err.data());
            g_eff->SetTitle((multi_str[i_multi] + " tracks").c_str());
            g_fake->SetTitle((multi_str[i_multi] + " tracks").c_str());
            g_eff->SetLineColor(multi_color[i_multi]);
            g_fake->SetLineColor(multi_color[i_multi]);
            g_1->Add(g_eff);
            g_2->Add(g_fake);
        }
        c_1->cd();
        g_1->GetXaxis()->SetTitle("noise");
        g_1->GetYaxis()->SetTitle("eff");
        g_1->Draw("AC");
        c_1->BuildLegend();
        // c_1->SaveAs("eff_multi_50MeV.png");
        c_2->cd();
        g_2->GetXaxis()->SetTitle("noise");
        g_2->GetYaxis()->SetTitle("fake rate");
        g_2->Draw("AC");
        c_2->BuildLegend();
        // c_2->SaveAs("fake_multi_50MeV.png");
    }

    /**
     * @brief part 3
     * e- 的 eff/fake noise 30
     *  不同的 multi 随pt变化
     *
     */
    {
        i_particle = 0;
        i_noise = 5;
        auto c_1 = new TCanvas("c_1", "eff with noise 30");
        auto c_2 = new TCanvas("c_2", "fake with noise 30");
        auto g_1 = new TMultiGraph("g1", "eff with noise 30");
        auto g_2 = new TMultiGraph("g2", "fake with noise 30");
        for (i_multi = 0; i_multi < 3; i_multi++) {
            array<double, 18> eff{};
            array<double, 18> eff_err{};
            array<double, 18> fake{};
            array<double, 18> fake_err{};
            for (i_pt = 0; i_pt < 18; i_pt++) {
                string total_file =
                    "./total_" + particle_str[i_particle] + ".txt";
                ifstream in_total(total_file);
                double total = 0;
                int i_total = 0;
                while (!in_total.eof()) {
                    in_total >> total;
                    if (i_total == i_pt) {
                        break;
                    } else {
                        i_total++;
                    }
                }
                string path =
                    "/home/ubuntu-tyxiao/work/STCF_Oscar2.0.0/HoughTracker/"
                    "data/";
                path += particle_str[i_particle] + "/trackdata_Pt" +
                        pt_str[i_pt] + "_noise" + noise_str[i_noise] +
                        "_multi" + multi_str[i_multi] + ".root";
                auto file = new TFile(path.c_str());
                auto tree = dynamic_cast<TTree *>(gDirectory->Get("tree1"));
                int event_id = 0;
                int track_id = 0;
                bool true_track = false;
                double P_t = NAN;
                double Q_xy = NAN;
                double Q_z = NAN;
                int num_true = 0;
                int num_total = 0;
                int Q_e = 0;
                tree->SetBranchAddress("event_id", &event_id);
                tree->SetBranchAddress("track_id", &track_id);
                tree->SetBranchAddress("true_track", &true_track);
                tree->SetBranchAddress("Pt", &P_t);
                tree->SetBranchAddress("Q", &Q_xy);
                tree->SetBranchAddress("Qz", &Q_z);
                tree->SetBranchAddress("num_true", &num_true);
                tree->SetBranchAddress("num_total", &num_total);
                tree->SetBranchAddress("Qe", &Q_e);
                int n_eff = 0;
                int n_true = 0;
                int event_id_skip = -1;
                auto n_all = tree->GetEntries();
                for (Long64_t ie = 0; ie < n_all; ie++) {
                    tree->GetEntry(ie);
                    if ((event_id != event_id_skip) && (event_id != -1)) {
                        event_id_skip = event_id;
                        n_eff++;
                    }
                    if (event_id != -1) {
                        n_true++;
                    }
                }
                eff[i_pt] = n_eff / total;
                fake[i_pt] = 1 - n_true / static_cast<double>(n_all);
                eff_err[i_pt] =
                    sqrt(eff[i_pt] * (1 - eff[i_pt]) * total) / total;
                fake_err[i_pt] = sqrt(fake[i_pt] * (1 - fake[i_pt]) *
                                      static_cast<double>(n_all)) /
                                 static_cast<double>(n_all);
            }
            auto g_eff = new TGraphErrors(18, pt_double.data(), eff.data(), 0,
                                          eff_err.data());
            auto g_fake = new TGraphErrors(18, pt_double.data(), fake.data(), 0,
                                           fake_err.data());
            g_eff->SetTitle((multi_str[i_multi] + " tracks").c_str());
            g_fake->SetTitle((multi_str[i_multi] + " tracks").c_str());
            g_eff->SetLineColor(multi_color[i_multi]);
            g_fake->SetLineColor(multi_color[i_multi]);
            g_1->Add(g_eff);
            g_2->Add(g_fake);
        }
        c_1->cd();
        g_1->GetXaxis()->SetTitle("Pt [MeV/c]");
        g_1->GetYaxis()->SetTitle("eff");
        g_1->Draw("AC");
        c_1->BuildLegend();
        // c_1->SaveAs("eff_multi_30noise.png");
        c_2->cd();
        g_2->GetXaxis()->SetTitle("Pt [MeV/c]");
        g_2->GetYaxis()->SetTitle("fake rate");
        g_2->Draw("AC");
        c_2->BuildLegend();
        // c_2->SaveAs("fake_multi_30noise.png");
    }

    /**
     * @brief part 4
     * e- 的 eff/fake
     * 不同的 pt 随noise变化
     *
     */
    {
        i_particle = 0;
        i_multi = 0;
        auto c_1 = new TCanvas("c_1", "eff");
        auto c_2 = new TCanvas("c_2", "fake");
        auto g_1 = new TMultiGraph("g1", "eff");
        auto g_2 = new TMultiGraph("g2", "fake");
        for (i_pt = 0; i_pt < 4; i_pt++) {
            string total_file = "./total_" + particle_str[i_particle] + ".txt";
            ifstream in_total(total_file);
            double total = 0;
            int i_total = 0;
            while (!in_total.eof()) {
                in_total >> total;
                if (i_total == 2) {
                    break;
                } else {
                    i_total++;
                }
            }
            array<double, 10> eff{};
            array<double, 10> eff_err{};
            array<double, 10> fake{};
            array<double, 10> fake_err{};
            for (i_noise = 0; i_noise < 10; i_noise++) {
                string path =
                    "/home/ubuntu-tyxiao/work/STCF_Oscar2.0.0/HoughTracker/"
                    "data/";
                path += particle_str[i_particle] + "/trackdata_Pt" +
                        pt_select_str[i_pt] + "_noise" + noise_str[i_noise] +
                        "_multi" + multi_str[i_multi] + ".root";
                auto file = new TFile(path.c_str());
                auto tree = dynamic_cast<TTree *>(gDirectory->Get("tree1"));
                int event_id = 0;
                int track_id = 0;
                bool true_track = false;
                double P_t = NAN;
                double Q_xy = NAN;
                double Q_z = NAN;
                int num_true = 0;
                int num_total = 0;
                int Q_e = 0;
                tree->SetBranchAddress("event_id", &event_id);
                tree->SetBranchAddress("track_id", &track_id);
                tree->SetBranchAddress("true_track", &true_track);
                tree->SetBranchAddress("Pt", &P_t);
                tree->SetBranchAddress("Q", &Q_xy);
                tree->SetBranchAddress("Qz", &Q_z);
                tree->SetBranchAddress("num_true", &num_true);
                tree->SetBranchAddress("num_total", &num_total);
                tree->SetBranchAddress("Qe", &Q_e);
                int n_eff = 0;
                int n_true = 0;
                int event_id_skip = -1;
                auto n_all = tree->GetEntries();
                for (Long64_t ie = 0; ie < n_all; ie++) {
                    tree->GetEntry(ie);
                    if ((event_id != event_id_skip) && (event_id != -1)) {
                        event_id_skip = event_id;
                        n_eff++;
                    }
                    if (event_id != -1) {
                        n_true++;
                    }
                }
                eff[i_noise] = n_eff / total;
                fake[i_noise] = 1 - n_true / static_cast<double>(n_all);
                eff_err[i_noise] =
                    sqrt(eff[i_noise] * (1 - eff[i_noise]) * total) / total;
                fake_err[i_noise] = sqrt(fake[i_noise] * (1 - fake[i_noise]) *
                                         static_cast<double>(n_all)) /
                                    static_cast<double>(n_all);
            }
            auto g_eff = new TGraphErrors(10, noise_double.data(), eff.data(),
                                          0, eff_err.data());
            auto g_fake = new TGraphErrors(10, noise_double.data(), fake.data(),
                                           0, fake_err.data());
            g_eff->SetTitle((pt_select_str[i_pt] + " MeV/c").c_str());
            g_fake->SetTitle((pt_select_str[i_pt] + " MeV/c").c_str());
            g_eff->SetLineColor(pt_color[i_pt]);
            g_fake->SetLineColor(pt_color[i_pt]);
            g_1->Add(g_eff);
            g_2->Add(g_fake);
        }
        c_1->cd();
        g_1->GetXaxis()->SetTitle("noise");
        g_1->GetYaxis()->SetTitle("eff");
        g_1->Draw("AC");
        c_1->BuildLegend();
        // c_1->SaveAs("eff_noise_pt.png");
        c_2->cd();
        g_2->GetXaxis()->SetTitle("noise");
        g_2->GetYaxis()->SetTitle("fake rate");
        g_2->Draw("AC");
        c_2->BuildLegend();
        // c_2->SaveAs("fake_noise_pt.png");
    }

    /**
     * @brief part 5
     * e- 的 eff/fake multi=1
     * 不同的 noise随pt变化
     *
     */
    {
        i_particle = 2;
        i_multi = 0;
        auto c_1 = new TCanvas("c_1", "eff");
        auto c_2 = new TCanvas("c_2", "fake");
        auto g_1 = new TMultiGraph("g1", "eff");
        auto g_2 = new TMultiGraph("g2", "fake");
        for (i_noise = 0; i_noise < 5; i_noise++) {
            array<double, 18> eff{};
            array<double, 18> eff_err{};
            array<double, 18> fake{};
            array<double, 18> fake_err{};
            for (i_pt = 0; i_pt < 18; i_pt++) {
                string total_file =
                    "./total_" + particle_str[i_particle] + ".txt";
                ifstream in_total(total_file);
                double total = 0;
                int i_total = 0;
                while (!in_total.eof()) {
                    in_total >> total;
                    if (i_total == i_pt) {
                        break;
                    } else {
                        i_total++;
                    }
                }
                string path =
                    "/home/ubuntu-tyxiao/work/STCF_Oscar2.0.0/HoughTracker/"
                    "data/";
                path += particle_str[i_particle] + "/trackdata_Pt" +
                        pt_str[i_pt] + "_noise" + noise_select_str[i_noise] +
                        "_multi" + multi_str[i_multi] + ".root";
                auto file = new TFile(path.c_str());
                auto tree = dynamic_cast<TTree *>(gDirectory->Get("tree1"));
                int event_id = 0;
                int track_id = 0;
                bool true_track = false;
                double P_t = NAN;
                double Q_xy = NAN;
                double Q_z = NAN;
                int num_true = 0;
                int num_total = 0;
                int Q_e = 0;
                tree->SetBranchAddress("event_id", &event_id);
                tree->SetBranchAddress("track_id", &track_id);
                tree->SetBranchAddress("true_track", &true_track);
                tree->SetBranchAddress("Pt", &P_t);
                tree->SetBranchAddress("Q", &Q_xy);
                tree->SetBranchAddress("Qz", &Q_z);
                tree->SetBranchAddress("num_true", &num_true);
                tree->SetBranchAddress("num_total", &num_total);
                tree->SetBranchAddress("Qe", &Q_e);
                int n_eff = 0;
                int n_true = 0;
                int event_id_skip = -1;
                auto n_all = tree->GetEntries();
                for (Long64_t ie = 0; ie < n_all; ie++) {
                    tree->GetEntry(ie);
                    if ((event_id != event_id_skip) && (event_id != -1)) {
                        event_id_skip = event_id;
                        n_eff++;
                    }
                    if (event_id != -1) {
                        n_true++;
                    }
                }
                eff[i_pt] = n_eff / total;
                if (n_eff == 0) {
                    eff[i_pt] = 0;
                }
                fake[i_pt] = 1 - n_true / static_cast<double>(n_all);
                if (n_true == 0) {
                    fake[i_pt] = 1;
                }
                eff_err[i_pt] =
                    sqrt(eff[i_pt] * (1 - eff[i_pt]) * total) / total;
                fake_err[i_pt] = sqrt(fake[i_pt] * (1 - fake[i_pt]) *
                                      static_cast<double>(n_all)) /
                                 static_cast<double>(n_all);
            }
            auto g_eff = new TGraphErrors(18, pt_double.data(), eff.data(), 0,
                                          eff_err.data());
            auto g_fake = new TGraphErrors(18, pt_double.data(), fake.data(), 0,
                                           fake_err.data());
            g_eff->SetTitle((noise_select_str[i_noise] + " noise").c_str());
            g_fake->SetTitle((noise_select_str[i_noise] + " noise").c_str());
            g_eff->SetLineColor(noise_color[i_noise]);
            g_fake->SetLineColor(noise_color[i_noise]);
            g_1->Add(g_eff);
            g_2->Add(g_fake);
        }
        c_1->cd();
        g_1->GetXaxis()->SetTitle("Pt [MeV/c]");
        g_1->GetYaxis()->SetTitle("eff");
        g_1->Draw("AC");
        c_1->BuildLegend();
        c_1->SaveAs("eff_pt_noise.png");
        c_2->cd();
        g_2->GetXaxis()->SetTitle("Pt [MeV/c]");
        g_2->GetYaxis()->SetTitle("fake rate");
        g_2->Draw("AC");
        c_2->BuildLegend();
        c_2->SaveAs("fake_pt_noise.png");
    }
}
