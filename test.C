#include <TRandom.h>
#include <iostream>
#include <random>

void test()
{
    std::random_device rd;
    int n_noise = 10;
    if (n_noise <= 0)
        return;
    else
    {
        auto rdm = new TRandom();
        auto rdm_layer = new TRandom();

        double r[] = {65.115, 115.11, 165.11};
        for (int i = 0; i < n_noise; i++)
        {
            rdm->SetSeed(rd() % kMaxUInt);
            rdm_layer->SetSeed(rd() % kMaxUInt);

            int layerID = rdm_layer->Integer(3);
            double x, y;
            rdm->Circle(x, y, r[layerID]);
            std::cout << layerID << " " << x << " " << y << std::endl;
        }
    }
}