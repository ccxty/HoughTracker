#include "global.h"

#include <memory>

void ITKAddNoise(int n_noise, Points &points) {
    if (n_noise <= 0) {
        return;
    }
    auto rdm = TRandom3();
    auto rdm_layer = TRandom3();
    auto rdm_z = TRandom3();
    std::random_device rd_device;
    auto len = points.size();
    rdm.SetSeed(rd_device() % kMaxULong);
    rdm_layer.SetSeed(rd_device() % kMaxULong);
    rdm_z.SetSeed(rd_device() % kMaxULong);

    for (int i = 0; i < n_noise; i++) {
        int layerID = static_cast<int>(rdm_layer.Integer(3));
        double posX = NAN, posY = NAN, posZ = NAN;
        rdm.Circle(posX, posY, InnerTrackerR[layerID]);
        posZ = (InnerTrackerR[layerID] / tan(20 * TMath::Pi() / 180.)) *
               (-1 + 2 * rdm_z.Rndm());
        double theta = atan2(posY, posX);
        double err_z = gRandom->Gaus(0, E_Z);
        double err_xy = gRandom->Gaus(0, E_RPhi);
        posZ += err_z;
        posX += -err_xy * sin(theta);
        posY += err_xy * cos(theta);
        auto *point = new HitPoint(posX, posY, posZ, -1, 1, layerID, 0);
        point->SetId(static_cast<int>(len) + i);
        points.push_back(point);
    }
}

std::unique_ptr<std::set<int>> GetRandomSet(int n_tracks_in_event,
                                            std::set<int> &base) {
    auto set_test = std::make_unique<std::set<int>>();
    while (set_test->size() < n_tracks_in_event) {
        auto iter(base.begin());
        advance(iter, rand() % base.size());
        set_test->insert(*iter);
    }
    return set_test;
}
