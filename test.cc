#include <algorithm>
#include <array>
#include <functional>
#include <iostream>
#include <iterator>
#include <limits>
#include <numeric>
#include <random>
#include <set>
#include <string>
#include <utility>

#include "TCanvas.h"
#include "TClass.h"
#include "TFile.h"
#include "TGraph.h"
#include "TH1.h"
#include "TH2.h"
#include "TH2D.h"
#include "TMarker.h"
#include "TMath.h"
#include "TRandom3.h"
#include "TTree.h"
#include "include/args.h"

using std::accumulate;
using std::array;
using std::cout;
using std::endl;
using std::make_unique;
using std::set;
using std::string;
using std::tuple;
using std::unique_ptr;
using std::vector;
constexpr std::array<double, 3> InnerDetectorR = {65.115, 115.11, 165.11};

std::set<int>* GetRandomSet(int n_tracks_in_event, std::set<int>& base) {
    auto* set = new std::set<int>;
    while (set->size() < n_tracks_in_event) {
        auto iter(base.begin());
        advance(iter, rand() % base.size());
        set->insert(*iter);
    }
    return set;
}

unique_ptr<vector<vector<int>>> test_unique_ptr() {
    auto ptr = make_unique<vector<vector<int>>>();
    for (int i = 0; i < 10; ++i) {
        auto x = make_unique<vector<int>>();
        x->push_back(i);
        ptr->push_back(std::move(*x));
    }
    return ptr;
}

class Test {
 public:
    std::vector<int> _ptr;
    explicit Test(vector<int> ptr) : _ptr(std::move(ptr)) {}
};

class Test2 {
 public:
    std::vector<int> _ptr;
    Test2() = default;
    explicit Test2(vector<int> ptr) : _ptr(std::move(ptr)) {}
};

auto test_unique_ptr2() {
    auto ptr = vector<unique_ptr<vector<unique_ptr<Test2>>>>();
    for (int i = 0; i < 3; i++) {
        auto ptr2 = make_unique<vector<unique_ptr<Test2>>>();
        for (int j = 0; j < 2; j++) {
            auto ptr3 = make_unique<Test2>();
            ptr3->_ptr.push_back(i + j);
            ptr3->_ptr.push_back(i + j);
            ptr2->push_back(move(ptr3));
        }
        ptr.push_back(move(ptr2));
    }
    return std::move(move(ptr));
}

void test_json_out() {
    Args args;
    args.mode = ExecMode::all;
    args.n_noise = 39;
    args.n_track = 1;
    args.particle = "e-";
    args.pt = 50;
    args_out_json(args);
    std::cout << std::boolalpha << args.data_file.empty() << std::endl;
}

void test_pointer() {
    auto* ptr1 = new int(10);
    auto* ptr2 = ptr1;
    cout << (ptr1 == ptr2) << std::endl;
}

void draw_points() {
    auto point0 = new TMarker(InnerDetectorR[0], 0.857237, 0);

    auto point1 = new TMarker(InnerDetectorR[1], 1.44932, 0);
    auto point2 = new TMarker(InnerDetectorR[2], 1.66734, 0);
    TCanvas* c1 = new TCanvas("", "");
    TH2D* h = new TH2D();
    c1->cd();
    h->Draw();
    point0->Draw();
    point1->Draw();
    point2->Draw();
}

void test_posZ_layer0() {
    // std::string file_name = "../root_data_source/e-/posPt50.root";
    // std::string file_name = "../root_data_source/e-/posPt50_deg36.8699.root";
    std::string file_name = "../root_data_source/e-/posPt50_deg66.4218.root";
    double theta = (90 - 66.4218) * TMath::Pi() / 180.;
    auto* file = new TFile(file_name.c_str());
    auto* tree = dynamic_cast<TTree*>(file->Get("tree1"));
    std::vector<double>* posX = nullptr;
    std::vector<double>* posY = nullptr;
    std::vector<double>* posZ = nullptr;
    std::vector<int>* layerID = nullptr;
    tree->SetBranchAddress("posX", &posX);
    tree->SetBranchAddress("posY", &posY);
    tree->SetBranchAddress("posZ", &posZ);
    tree->SetBranchAddress("layerID", &layerID);
    auto* c1 = new TCanvas("c1", "c1");
    auto* hist = new TH1F("hist", "hist", 600, -100, 100);
    auto entries = tree->GetEntries();
    for (int ie = 0; ie < entries; ie++) {
        tree->GetEntry(ie);
        for (int ip = 0; ip < posZ->size(); ip++) {
            if (layerID->at(ip) == 0) {
                hist->Fill(fabs(posZ->at(ip)) - tan(theta) * InnerDetectorR[0]);
                break;
            }
        }
    }
    c1->cd();
    hist->Draw();
}

void test() {
    // std::set<int> base;
    // for (int i = 0; i < 100; i++)
    // {
    //     base.insert(i);
    // }

    // for (int i = 0; i < 1000; i++)
    // {
    //     auto a = GetRandomSet(2, base);
    //     for (auto num : *a)
    //     {
    //         std::cout << num << "\t";
    //     }

    //     std::cout << "\n"
    //               << a->size() << "\n";
    // }
    int n_noise = 10000;
    auto rdm = TRandom3();
    auto rdm_layer = TRandom3();
    auto rdm_z = TRandom3();
    std::random_device rd;
    std::array<double, 3> r = {65.115, 115.11, 165.11};
    rdm.SetSeed(rd() % kMaxULong);
    rdm_layer.SetSeed(rd() % kMaxULong);
    rdm_z.SetSeed(rd() % kMaxULong);
    int n_plus = 0;
    double cos_20 = cos(20 * TMath::Pi() / 180.);
    // auto c1 = new TCanvas("c1", "c1");
    // auto h = new TH1F("h", "h", 1000, -500, 500);
    // for (int i = 0; i < n_noise; i++) {
    //     int layerID = rdm_layer->Integer(3);
    //     double x, y, cos_theta, z;
    //     rdm->Circle(x, y, r[layerID]);
    //     cos_theta = rdm_z->Rndm() * (2 * cos_20) - cos_20;
    //     z = r[layerID] * cos_theta / sqrt(1 - cos_theta * cos_theta);
    //     h->Fill(z);
    // }
    // c1->cd();
    // h->Draw();
    // c1->Draw();
    array<double, 3> test_array = {1, 2, 3};
    vector<int>::size_type i = 18;
    cout << static_cast<int>(i) << "\n\n";
    cout << accumulate(test_array.begin(), test_array.end(), 0.0) << endl;

    auto* file = new TFile("data/e-/track125x125_Pt100_noise66_multi1.root");
    cout << file->IsOpen() << std::endl;

    auto test_ptr = test_unique_ptr();
    cout << test_ptr->size() << endl;
    for (const auto& i : *test_ptr) {
        for (auto j : i) {
            cout << j << "\t";
        }
    }
    cout << "\r";
    auto test_ptr2 = test_unique_ptr2();
    for (const auto& i : test_ptr2) {
        for (const auto& j : *i) {
            for (const auto& k : j->_ptr) {
                cout << k << "\t";
            }
        }
    }
    cout << endl;
    test_json_out();
    test_pointer();
    draw_points();
    test_posZ_layer0();
}
