#include <algorithm>
#include <array>
#include <functional>
#include <iostream>
#include <iterator>
#include <limits>
#include <numeric>
#include <random>
#include <set>
#include <utility>

#include "TCanvas.h"
#include "TFile.h"
#include "TH1.h"
#include "TMath.h"
#include "TRandom3.h"
std::set<int> *GetRandomSet(int n_tracks_in_event, std::set<int> &base) {
    auto set = new std::set<int>;
    while (set->size() < n_tracks_in_event) {
        auto iter(base.begin());
        advance(iter, rand() % base.size());
        set->insert(*iter);
    }
    return set;
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
    auto rdm = new TRandom3();
    auto rdm_layer = new TRandom3();
    auto rdm_z = new TRandom3();
    std::random_device rd;
    std::array<double, 3> r = {65.115, 115.11, 165.11};
    rdm->SetSeed(rd() % kMaxULong);
    rdm_layer->SetSeed(rd() % kMaxULong);
    rdm_z->SetSeed(rd() % kMaxULong);
    int n_plus = 0;
    double cos_20 = cos(20 * TMath::Pi() / 180.);
    auto c1 = new TCanvas("c1", "c1");
    auto h = new TH1F("h", "h", 1000, -500, 500);
    for (int i = 0; i < n_noise; i++) {
        int layerID = rdm_layer->Integer(3);
        double x, y, cos_theta, z;
        rdm->Circle(x, y, r[layerID]);
        cos_theta = rdm_z->Rndm() * (2 * cos_20) - cos_20;
        z = r[layerID] * cos_theta / sqrt(1 - cos_theta * cos_theta);
        h->Fill(z);
    }
    c1->cd();
    h->Draw();
    c1->Draw();
    std::array<double, 3> test_array = {1, 2, 3};
    std::vector<int>::size_type i = 18;
    std::cout << static_cast<int>(i) << "\n\n";
    std::cout << std::accumulate(test_array.begin(), test_array.end(), 0)
              << std::endl;

    auto file = new TFile("data/e-/track125x125_Pt100_noise66_multi1.root");
    std::cout << file->IsOpen() << std::endl;
}
