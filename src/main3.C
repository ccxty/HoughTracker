#include <iostream>
#include <fstream>
#include <vector>
#include "TTree.h"
#include <TVector3.h>
#include "TFile.h"
#include "HoughPoint.h"
#include "HoughGridArea.h"
#include "HoughTrack.h"
#include "TMath.h"
#include "clag.h"
#include <random>
#include "TRandom3.h"
#include <set>
#include <algorithm>
#include "Hough.h"

using namespace std;

void find_peak(std::vector<std::vector<HoughGridArea *> *> *gridMatrix /*, std::vector<int> &PeakGridX, std::vector<int> &PeakGridY*/)
{
    int flag = 0;
    for (int ia = 0; ia < gridMatrix->size(); ia++)
    {
        auto row = gridMatrix->at(ia);
        for (int id = 0; id < row->size(); id++)
        {
            auto grid = row->at(id);
            if (grid->counts() >= 3)
            {
                auto points = grid->GetPointsHere();
                for (int ip = 0; ip < points->size(); ip++)
                {
                    auto point = points->at(ip);
                    // std::cout << flag << " " << grid->counts() << " "
                    //           << point->eventID() << " " << point->layerID()
                    //           << " " << ia << " " << id << endl;
                }
                flag++;
            }
        }
    }
}

std::vector<HoughTrack *> *find_track(std::vector<std::vector<HoughGridArea *> *> *gridMatrix /*, std::vector<int> &PeakGridX, std::vector<int> &PeakGridY*/)
{
    auto ptr = new std::vector<HoughTrack *>;
    for (int ia = 0; ia < gridMatrix->size(); ia++)
    {
        auto row = gridMatrix->at(ia);
        for (int id = 0; id < row->size(); id++)
        {
            auto grid = row->at(id);
            if (grid->counts() >= 3)
            {
                auto points = grid->GetPointsHere();
                for (int ip = 0; ip < points->size(); ip++)
                {
                    auto point = points->at(ip);
                    // cout << flag1 << " " << grid->counts() << " "
                    //      << point->eventID() << " " << point->layerID()
                    //      << " " << ia << " " << id << endl;
                }
                if ((ia > 0) && (id > 0) && (ia + 1 < gridMatrix->size()) && (id + 1 < row->size()))
                {
                    int counts1 = gridMatrix->at(ia - 1)->at(id - 1)->counts();
                    int counts2 = gridMatrix->at(ia - 1)->at(id)->counts();
                    int counts3 = gridMatrix->at(ia - 1)->at(id + 1)->counts();
                    int counts4 = gridMatrix->at(ia)->at(id - 1)->counts();
                    int counts5 = gridMatrix->at(ia)->at(id + 1)->counts();
                    int counts6 = gridMatrix->at(ia + 1)->at(id - 1)->counts();
                    int counts7 = gridMatrix->at(ia + 1)->at(id)->counts();
                    int counts8 = gridMatrix->at(ia + 1)->at(id + 1)->counts();
                    if ((grid->counts() >= counts1) && (grid->counts() >= counts2) && (grid->counts() >= counts3) &&
                        (grid->counts() >= counts4) && (grid->counts() >= counts5) && (grid->counts() >= counts6) &&
                        (grid->counts() >= counts7) && (grid->counts() >= counts8))
                    {
                        auto ptr_temp = new HoughTrack(points);
                        if (ptr->size() == 0)
                        {
                            ptr->push_back(ptr_temp);
                            // ptr_temp->Print();
                        }
                        else
                        {
                            bool is_equal = false;
                            for (int k = 0; k < ptr->size(); k++)
                            {
                                auto exsitingTrack = ptr->at(k);
                                if (exsitingTrack->operator==(ptr_temp))
                                    is_equal = true;
                            }
                            if (!is_equal && (ptr_temp->HitALayers()))
                            {
                                ptr->push_back(ptr_temp);
                                // ptr_temp->Print();
                            }
                        }
                    }
                }
            }
        }
    }
    // std::cout << " Num of tracks: " << ptr->size() << std::endl;
    return ptr;
}

std::vector<std::vector<HoughGridArea *> *> *GridInit()
{
    auto ptr1 = new vector<vector<HoughGridArea *> *>;
    for (int i = 0; i < NAlpha; i++)
    {
        auto ptr2 = new vector<HoughGridArea *>;
        for (int j = 0; j < ND; j++)
        {
            auto ptr3 = new HoughGridArea(AlphaMin + i * AlphaBinWidth, DMin + j * DBinWidth, DMin + (j + 1) * DBinWidth);
            ptr2->push_back(ptr3);
        }
        ptr1->push_back(ptr2);
    }
    // std::cout << "Init completed" << std::endl;
    return ptr1;
}

void FillGrid(std::vector<std::vector<HoughGridArea *> *> *gridMatrix, vector<HoughPoint *> &pointsList)
{
    for (int ip = 0; ip < pointsList.size(); ip++)
    {
        auto point = pointsList.at(ip);
        for (int ia = 0; ia < gridMatrix->size(); ia++)
        {
            auto row = gridMatrix->at(ia);
            for (int id = 0; id < row->size(); id++)
            {
                auto grid = row->at(id);
                double alpha = grid->xMid();
                double d = point->xConformal() * cos(alpha) + point->yConformal() * sin(alpha);

                if ((d >= grid->yMin()) && (d < grid->yMax()))
                {
                    // cout << d << endl;
                    grid->CountsAddOne();
                    grid->GetPointsHere()->push_back(point);
                }
            }
        }
        // std::cout << ip << " of " << pointsList.size() << endl;
    }
}

void AddNoise(int n_noise, std::vector<HoughPoint *> &points)
{
    if (n_noise <= 0)
        return;
    else
    {
        auto rdm = new TRandom3();
        auto rdm_layer = new TRandom3();
        auto rdm_z = new TRandom3();
        std::random_device rd;
        double r[] = {65.115, 115.11, 165.11};
        int len = points.size();
        rdm->SetSeed(rd() % kMaxULong);
        rdm_layer->SetSeed(rd() % kMaxULong);
        rdm_z->SetSeed(rd() % kMaxULong);
        double cos_20 = cos(20 * TMath::Pi() / 180.);

        for (int i = 0; i < n_noise; i++)
        {
            int layerID = rdm_layer->Integer(3);
            double x, y, z, cos_theta;
            rdm->Circle(x, y, r[layerID]);
            cos_theta = rdm_z->Rndm() * (2 * cos_20) - cos_20;
            z = r[layerID] * cos_theta / sqrt(1 - cos_theta * cos_theta);
            auto point = new HoughPoint(x, y, z, -1, 1, layerID, 0);
            point->SetId(len + i - 1);
            points.push_back(point);
        }
        delete rdm;
        delete rdm_layer;
        delete rdm_z;
    }
}

std::set<int> *GetRandomSet(int n_tracks_in_event, std::set<int> &base)
{
    auto set = new std::set<int>;
    while (set->size() < n_tracks_in_event)
    {
        auto it(base.begin());
        advance(it, rand() % base.size());
        set->insert(*it);
    }
    return set;
}

bool SetContain(std::set<int> *set, int num)
{
    bool contain = false;
    for (auto e : *set)
    {
        if (e == num)
            contain = true;
    }
    return contain;
}

int main(int argc, char **argv)
{
    // ./program particle pt n_noise mode multi
    // argv:0        1    2     3     4     5
    string particle(argv[1]);
    double Pt_data = atof(argv[2]);
    string path = "/home/txiao/STCF_Oscar2.0.0/share/pi+/test2/root_data_source/";
    path += particle + "/posPt";
    path += argv[2];
    path += ".root";

    int n_noise = atoi(argv[3]);
    string execMode(argv[4]);
    int n_tracks_in_event = 1;

    std::set<int> *eventIDs_toTest;
    std::set<int> eventIDs_all;
    for (int i = 0; i < 10000; i++)
    {
        eventIDs_all.insert(i);
    }
    if (execMode == "single_multi")
    {
        n_tracks_in_event = atoi(argv[5]);
        eventIDs_toTest = GetRandomSet(n_tracks_in_event, eventIDs_all);
    }
    else if (execMode == "all_multi")
    {
        n_tracks_in_event = atoi(argv[5]);
        eventIDs_toTest = &eventIDs_all;
    }
    else
    {
        std::cout << "wrong usage" << endl;
    }

    TFile *file = new TFile(path.c_str());
    TTree *tree = (TTree *)gDirectory->Get("tree1");
    std::vector<double> *posX = 0;
    std::vector<double> *posY = 0;
    std::vector<double> *posZ = 0;
    std::vector<int> *trackID = 0;
    std::vector<int> *layerID = 0;
    std::vector<double> *Pt = 0;
    TBranch *b_posX = 0;
    TBranch *b_posY = 0;
    TBranch *b_posZ = 0;
    TBranch *b_trackID = 0;
    TBranch *b_Pt = 0;
    TBranch *b_layerID = 0;
    int eventID;
    int nevents, npoints, nhits;
    tree->SetBranchAddress("posX", &posX, &b_posX);
    tree->SetBranchAddress("posY", &posY, &b_posY);
    tree->SetBranchAddress("posZ", &posZ, &b_posZ);
    tree->SetBranchAddress("eventID", &eventID);
    tree->SetBranchAddress("layerID", &layerID, &b_layerID);
    tree->SetBranchAddress("trackID", &trackID, &b_trackID);
    tree->SetBranchAddress("Pt", &Pt, &b_Pt);
    tree->SetBranchAddress("nhits", &nhits);

    nevents = tree->GetEntries();

    string savepath = "./trackdata_Pt" + string(argv[2]) + "_noise" + string(argv[3]);
    if (n_tracks_in_event != 0)
        savepath += "_multi" + string(argv[5]);
    savepath += ".root";
    auto savefile = new TFile(savepath.c_str(), "RECREATE");
    auto savetree = new TTree("tree1", "tree1");
    int event_id, track_id, num_true, num_total, Qe;
    double Q_min, p_t, Q_z;
    bool true_track;
    savetree->Branch("event_id", &event_id);
    savetree->Branch("track_id", &track_id);
    savetree->Branch("true_track", &true_track);
    savetree->Branch("Pt", &p_t);
    savetree->Branch("Q", &Q_min);
    savetree->Branch("Qz", &Q_z);
    savetree->Branch("num_true", &num_true);
    savetree->Branch("num_total", &num_total);
    savetree->Branch("Qe", &Qe);
    int counts_useful_events = 0;
    std::cout << path << endl;
    // std::cout << Pt_data << endl;
    while (eventIDs_toTest->size() >= n_tracks_in_event)
    {
        std::cout << "There are " << eventIDs_toTest->size() << " events left\n";
        int eventIDTest;
        auto test_set = GetRandomSet(n_tracks_in_event, *eventIDs_toTest);
        for (auto event : *test_set)
        {
            eventIDs_toTest->erase(event);
        }
        std::vector<HoughPoint *> pointsList;
        int eventID_temp = -1;
        int eventID_skip = -1;
        for (auto ie : *test_set)
        {
            tree->GetEntry(ie);
            if (nhits < 3)
            {
                continue;
            }
            int read_count = 0;
            counts_useful_events++;
            for (int ip = 0; ip < nhits; ip++)
            {
                if ((trackID->at(ip) == 1) && (eventID != eventID_skip))
                {
                    HoughPoint *ptr = new HoughPoint(posX->at(ip), posY->at(ip), posZ->at(ip),
                                                     eventID, trackID->at(ip), layerID->at(ip), Pt->at(ip));
                    ptr->SetId(pointsList.size());
                    pointsList.push_back(ptr);
                    read_count++;
                }
                if (read_count >= 3)
                {
                    eventID_skip = eventID;
                }
            }
        }
        if (pointsList.size() < 3)
        {
            continue;
        }
        AddNoise(n_noise, pointsList);
        npoints = pointsList.size();

        // std::cout << "number of points: " << npoints << endl;

        auto houghGrid = GridInit();
        FillGrid(houghGrid, pointsList);
        auto tracks = find_track(houghGrid);
        std::vector<double> PtReContruction;
        std::vector<double> QReContruction;

        double Qmin = 1.;
        int n_good_tracks = 0;
        for (int i = 0; i < tracks->size(); i++)
        {
            double pt, Q, Qz;
            auto track = tracks->at(i);
            if (track->HitALayers())
            {
                // track->Print();
                // std::cout << boolalpha << track->ContainTrueTrack() << " "
                //           << noboolalpha << track->RatioTrues() << std::endl;
                bool fit_fine = track->FitLinear(&pt, &Q, &Qz);

                if (fit_fine && (pt > PtMin) && (Q < QCut))
                {
                    event_id = track->GetEventID(test_set);
                    PtReContruction.push_back(pt);
                    QReContruction.push_back(Q);

                    track_id = PtReContruction.size();
                    true_track = track->ContainTrueTrackMulti(test_set);
                    p_t = pt;
                    Q_min = Q;
                    Q_z = Qz;
                    num_true = track->NumTruePointsMulti(test_set);
                    num_total = track->Counts();
                    Qe = track->GetSpin();
                    savefile->cd();
                    savetree->Fill();

                    n_good_tracks++;
                }
            }
        }
        // std::cout << "number of good tracks: " << n_good_tracks << std::endl;

        //
        //
        // delete
        for (auto ptr : pointsList)
        {
            delete ptr;
        }
        for (int i = 0; i < NAlpha; i++)
        {
            for (int j = 0; j < ND; j++)
            {
                delete houghGrid->at(i)->at(j);
            }
            delete houghGrid->at(i);
        }
        delete houghGrid;
    }
    savefile->cd();
    savetree->Write();
    savefile->Write();
    savefile->Close();
    // delete file;
    // delete tree;
    // delete savetree;
    // delete savefile;
    std::cout << "Save Path: " << savepath << std::endl;
    std::cout << "totoal tarcks useful: " << counts_useful_events << endl
              << endl;
    return 0;
}
