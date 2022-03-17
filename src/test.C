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

using namespace std;

const double AlphaMin = 0;
const double AlphaMax = TMath::Pi();
const double DMin = -0.05;
const double DMax = 0.05;
const int NAlpha = 100;
const int ND = 100;
const double AlphaBinWidth = (AlphaMax - AlphaMin) / NAlpha;
const double DBinWidth = (DMax - DMin) / ND;

const double MagFeild = 1.0; // 1.0 T

std::vector<std::vector<HoughGridArea *> *> *GridInit1()
{
    auto ptr1 = new vector<vector<HoughGridArea *> *>;
    for (int i = 0; i < NAlpha; i++)
    {
        auto ptr2 = new vector<HoughGridArea *>;
        for (int j = 0; j < ND; j++)
        {
            auto ptr3 = new HoughGridArea(AlphaMin + i * AlphaBinWidth, AlphaMin + (i + 1) * AlphaBinWidth,
                                          DMin + j * DBinWidth, DMin + (j + 1) * DBinWidth);
            ptr2->push_back(ptr3);
        }
        ptr1->push_back(ptr2);
    }
    std::cout << "Init completed" << std::endl;
    return ptr1;
}

void FillGrid1(std::vector<std::vector<HoughGridArea *> *> *gridMatrix, vector<HoughPoint *> &pointsList)
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
                double alpha1 = grid->xMin();
                double alpha2 = grid->xMax();
                double d1 = point->xConformal() * cos(alpha1) + point->yConformal() * sin(alpha1);
                double d2 = point->xConformal() * cos(alpha2) + point->yConformal() * sin(alpha2);
                if (((d1 >= grid->yMin()) && (d1 < grid->yMax())) || (d2 >= grid->yMin()) && (d2 < grid->yMax()))
                {
                    grid->CountsAddOne();
                    grid->GetPointsHere()->push_back(point);
                }
            }
        }
        cout << ip << " of " << pointsList.size() << endl;
    }
}

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
                    cout << flag << " " << grid->counts() << " "
                         << point->eventID() << " " << point->layerID()
                         << " " << ia << " " << id << endl;
                }
                flag++;
            }
        }
    }
}

std::vector<HoughTrack *> *find_track(std::vector<std::vector<HoughGridArea *> *> *gridMatrix /*, std::vector<int> &PeakGridX, std::vector<int> &PeakGridY*/)
{
    int flag1 = 0;
    int flag2 = 0;
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
                flag1++;
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
                        flag2++;
                        auto ptr_temp = new HoughTrack(points);
                        if (ptr->size() == 0)
                        {
                            ptr->push_back(ptr_temp);
                            ptr_temp->Print();
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
                            if (!is_equal)
                            {
                                ptr->push_back(ptr_temp);
                                ptr_temp->Print();
                            }
                        }

                        // cout << flag2 << " " << ia << " " << id << endl;
                    }
                }
            }
        }
    }
    std::cout << " Num of tracks: " << ptr->size() << std::endl;
    return ptr;
}

std::vector<std::vector<HoughGridArea *> *> *GridInit2()
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
    std::cout << "Init completed" << std::endl;
    return ptr1;
}

void FillGrid2(std::vector<std::vector<HoughGridArea *> *> *gridMatrix, vector<HoughPoint *> &pointsList)
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
        cout << ip << " of " << pointsList.size() << endl;
    }
}

int main(int argc, char **argv)
{
    if (argc != 4)
    {
        cout << "wrong usage" << endl;
        return -1;
    }
    double Pt_data = atof(argv[1]);
    // string path = "/home/txiao/STCF_Oscar2.0.0/share/test2/posPt100.root";
    string path = "/home/txiao/STCF_Oscar2.0.0/share/test2/posPt";
    path += argv[1];
    path += ".root";
    string execMode(argv[2]);

    int eventIDTest;
    if (execMode == "random")
    {
        eventIDTest = rand() % 1000;
    }
    else if (execMode == "given")
    {
        eventIDTest = atoi(argv[3]);
    }
    else
    {
        cout << "Wrong Argument: Mode" << endl
             << "random or given " << endl;
    }

    TFile *file = new TFile(path.c_str());
    TTree *tree = (TTree *)gDirectory->Get("tree1");
    double posX, posY, posZ;
    double Pt;
    int layerID, eventID, trackID;
    int nevents, npoints;
    tree->SetBranchAddress("posX", &posX);
    tree->SetBranchAddress("posY", &posY);
    tree->SetBranchAddress("posZ", &posZ);
    tree->SetBranchAddress("eventID", &eventID);
    tree->SetBranchAddress("layerID", &layerID);
    tree->SetBranchAddress("trackID", &trackID);
    tree->SetBranchAddress("Pt", &Pt);

    nevents = tree->GetEntries();

    std::vector<HoughPoint *> pointsList;
    int eventID_temp = -1;
    int read_count = 0;
    int eventID_skip = -1;
    int ntotal_tracks = 0;
    for (int ie = 0; ie < nevents; ie++)
    {
        tree->GetEntry(ie);

        if ((trackID == 1) && (eventID != eventID_skip))
        {
            HoughPoint *ptr = new HoughPoint(posX, posY, posZ, eventID, trackID, layerID, Pt);
            int identity = pointsList.size();
            ptr->SetId(identity);
            pointsList.push_back(ptr);
            read_count++;
            if (read_count >= 3)
            {
                ntotal_tracks++;
                eventID_skip = eventID;
                read_count = 0;
            }
        }
    }

    npoints = pointsList.size();
    cout << path << endl;
    cout << Pt_data << endl;
    cout << "eventID for Test: " << eventIDTest << endl;
    cout << "number of points: " << npoints << endl;
    cout << "total tracks: " << ntotal_tracks << endl;
    if (npoints == 0)
    {
        std::cout << "no points" << std::endl;
        return 0;
    }

    // auto houghGrid = GridInit2();
    // FillGrid2(houghGrid, pointsList);
    // auto tracks = find_track(houghGrid);
    // std::vector<double> PtReContruction;
    // for (int i = 0; i < tracks->size(); i++)
    // {
    //     double pt;
    //     bool fine = tracks->at(i)->Fit(&pt);
    //     if (fine)
    //         PtReContruction.push_back(pt);
    // }

    // string outpath = "./PtRe";
    // outpath += argv[1];
    // outpath += ".txt";
    // ofstream output(outpath.c_str(), ios::app);
    // for (auto pt : PtReContruction)
    // {
    //     output << pt << endl;
    // }

    for (auto ptr : pointsList)
    {
        // ptr->Print();
        delete ptr;
    }
    return 0;
}
