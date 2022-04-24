#include <fstream>
#include <iostream>
#include <ostream>
#include <sstream>
#include <string>
#include <vector>

#include "Rtypes.h"
#include "TAttMarker.h"
#include "TAxis.h"
#include "TCanvas.h"
#include "TClonesArray.h"
#include "TEllipse.h"
#include "TF1.h"
#include "TGraph.h"
#include "TH1.h"
#include "TH2.h"
#include "TH2D.h"
#include "TLegend.h"
#include "TMarker.h"
#include "TMath.h"
#include "TROOT.h"
#include "TRandom.h"
#include "TStyle.h"

struct Point {
    double x;
    double y;
    double z;
    int id;
    int eventID;
};

struct Track {
    bool is_true;
    std::vector<int> ids;
};

void DataVisual() {
    std::ifstream in_points("./points.txt");
    std::ifstream in_tracks("./tracks.txt");
    std::vector<Point> points;
    std::vector<Track> tracks;
    Track track;
    while (!in_points.eof()) {
        Point point;
        in_points >> point.eventID >> point.id >> point.x >> point.y >> point.z;
        points.push_back(point);
    }
    while (!in_tracks.eof()) {
        Track track;
        std::string line, is_true;
        std::getline(in_tracks, line);
        std::stringstream line_stream(line);
        int identity = -1;
        line_stream >> is_true;
        if (is_true == "true") {
            track.is_true = true;
        }
        if (is_true == "false") {
            track.is_true = false;
        }
        while (line_stream >> identity) {
            track.ids.push_back(identity);
        }
        tracks.push_back(track);
    }
    // for (const auto& track : tracks) {
    //     std::cout << std::boolalpha << track.is_true << "\t";
    //     for (auto identity : track.ids) {
    //         std::cout << identity << "\t";
    //     }
    //     std::cout << "\n";
    // }
    TCanvas* canvas1 = new TCanvas("c1", "Map", 600, 600);
    const int N_Points = static_cast<int>(points.size());
    const int N_Tracks = static_cast<int>(tracks.size());
    TH2F* hg1 =
        new TH2F("h", "e- @ 50MeV with noise 39", 5, -200, 200, 5, -200, 200);
    canvas1->cd();
    hg1->SetLabelSize(0.02, "Y");
    hg1->SetLabelSize(0.02, "x");
    hg1->GetXaxis()->SetTitle("x(mm)");
    hg1->GetYaxis()->SetTitle("y(mm)");
    hg1->GetXaxis()->SetTitleSize(0.02);
    hg1->GetYaxis()->SetTitleSize(0.02);
    hg1->GetXaxis()->CenterTitle(true);
    hg1->GetYaxis()->CenterTitle(true);
    hg1->Draw();
    TMarker* origin = new TMarker(0, 0, 0);
    origin->SetMarkerSize(0.5);
    origin->SetMarkerColor(kBlue);
    origin->SetMarkerStyle(kOpenCircle);
    origin->Draw();
    for (auto point : points) {
        TMarker* hit = new TMarker(point.x, point.y, 0);
        if (point.eventID != -1) {
            hit->SetMarkerColor(kRed);
        } else {
            hit->SetMarkerColor(kBlack);
        }
        hit->SetMarkerStyle(kOpenCircle);
        hit->SetMarkerSize(0.5);
        hit->Draw();
    }
    for (const auto& track : tracks) {
        bool is_true = track.is_true;
        // TGraph g_track(track.ids.size());
        // int i = 0;
        // for (auto identity : track.ids) {
        //     Point point = points.at(identity);
        //     g_track.SetPoint(i, point.x, point.y);
        //     i++;
        // }
        if (is_true == true) {
            // g_track.SetLineColor(kRed);
            for (auto identity : track.ids) {
                Point point = points.at(identity);
                TMarker* hit = new TMarker(point.x, point.y, 0);
                hit->SetMarkerStyle(kFullSquare);
                hit->SetMarkerSize(0.8);
                if (point.eventID != -1) {
                    hit->SetMarkerColor(kRed);
                } else {
                    hit->SetMarkerColor(kBlack);
                }
                hit->Draw();
            }
        }
        if (is_true == false) {
            // g_track.SetLineColor(kBlack);
            for (auto identity : track.ids) {
                Point point = points.at(identity);
                TMarker* hit = new TMarker(point.x, point.y, 0);
                hit->SetMarkerStyle(kFullCircle);
                hit->SetMarkerSize(0.5);
                if (point.eventID != -1) {
                    hit->SetMarkerColor(kRed);
                } else {
                    hit->SetMarkerColor(kBlack);
                }
                hit->Draw();
            }
        }
        // TCanvas* canvas2 = new TCanvas("c2", "c2", 800, 800);
        //  canvas2->cd();
        //  canvas2->DrawFrame(-200, -200, 200, 200);
        //  g_track.Draw();
    }
}
//------------------------------
// 真实点：红色
// 假点：黑色
// 原点：蓝色
// 径迹击中：实心
// 未击中：空心
// 真实径迹：方形
// 假径迹：圆形
//------------------------------