#include <fstream>
#include <iostream>
#include <ostream>
#include <sstream>
#include <string>
#include <vector>

#include "../include/global.h"
#include "../include/line.h"
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
    int layerID;
};

struct VisualTrack {
    bool is_true{};
    std::vector<int> ids;
    int event_id;
};

void GetParams(Point& p0, Point& p1, Point& p2, TrackParameters& param) {
    double k1 = -(p1.x - p0.x) / (p1.y - p0.y);
    Point2D mid1{(p1.x + p0.x) / 2, (p1.y + p0.y) / 2};
    double k2 = -(p2.x - p1.x) / (p2.y - p1.y);
    Point2D mid2{(p2.x + p1.x) / 2, (p2.y + p1.y) / 2};
    param.center.x = -(mid1.y - mid2.y - mid1.x * k1 + mid2.x * k2) / (k1 - k2);
    param.center.y =
        -(-mid2.y * k1 + mid1.y * k2 + (mid2.x - mid1.x) * k1 * k2) / (k1 - k2);
    param.R = sqrt((p0.x - param.center.x) * (p0.x - param.center.x) +
                   (p0.y - param.center.y) * (p0.y - param.center.y));
}

void DataVisual() {
    std::ifstream in_points("./points.txt");
    std::ifstream in_tracks("./tracks.txt");
    std::vector<Point> points;
    std::vector<VisualTrack> tracks;
    VisualTrack track;
    while (!in_points.eof()) {
        Point point{};
        in_points >> point.eventID >> point.layerID >> point.id >> point.x >>
            point.y >> point.z;
        points.push_back(point);
    }
    while (!in_tracks.eof()) {
        VisualTrack track;
        std::string line, is_true;
        std::getline(in_tracks, line);
        std::stringstream line_stream(line);
        int identity = -1;
        line_stream >> track.event_id;
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
        if (track.ids.size() == 3) {
            std::array<Point, 3> points_array{};
            int i_p = 0;
            for (auto id : track.ids) {
                for (auto point : points) {
                    if (point.id == id) {
                        points_array[i_p].x = point.x;
                        points_array[i_p].y = point.y;
                        points_array[i_p].z = point.z;
                        points_array[i_p].id = point.id;
                        points_array[i_p].eventID = point.eventID;
                        i_p++;
                    }
                }
            }
            TrackParameters param;
            GetParams(points_array[0], points_array[1], points_array[2], param);
            double d = sqrt(param.center.x * param.center.x +
                            param.center.y * param.center.y);
            std::cout << fabs(d - param.R) << "\t" << d << "\t" << param.R
                      << std::endl;
        }
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
    auto* origin = new TMarker(0, 0, 0);
    origin->SetMarkerSize(0.5);
    origin->SetMarkerColor(kBlue);
    origin->SetMarkerStyle(kOpenCircle);
    origin->Draw();
    for (auto point : points) {
        auto* hit = new TMarker(point.x, point.y, 0);
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
        if (is_true) {
            // g_track.SetLineColor(kRed);
            for (auto identity : track.ids) {
                Point point = points.at(identity);
                auto* hit = new TMarker(point.x, point.y, 0);
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
        if (!is_true) {
            // g_track.SetLineColor(kBlack);
            for (auto identity : track.ids) {
                Point point = points.at(identity);
                auto* hit = new TMarker(point.x, point.y, 0);
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
    // canvas1->SaveAs("data_visual.png");
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