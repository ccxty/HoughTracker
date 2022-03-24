#include "TFile.h"
#include "TTree.h"
#include "TVector3.h"
#include <iostream>
#include "Event/StcfMCHeader.h"
#include "Event/StcfMCEvent.h"
using namespace std;

// int ReadRoot()
int main()
{
  // gSystem->Load("libFullSim.so");
  // gSystem->Load("libStcfMCEvent.so");

  // string path = "/ustcfs/STCFUser/zhouh/20211018/20211206_lowPt/pi+/";
  //  path += "singlePiplus15.root";
  string n_str[] = {"0", "1", "2", "3", "4", "5", "6", "7", "8",
                    "9", "10", "11", "12", "13", "14", "15", "16", "17"};
  string pts[] = {"50", "55", "60", "65", "70", "75", "80", "85", "90",
                  "95", "100", "105", "110", "115", "120", "125", "130", "135"};

  for (int i = 0; i < 18; i++)
  {
    string path = "/ustcfs/STCFUser/zhouh/20220319/SingleElectron/";
    path += "singleEminus" + n_str[i] + ".root";
    string savepath = "./root_data_source/e-/posPt";
    savepath += pts[i] + ".root";
    TFile *file = new TFile(path.c_str());
    std::cout << "path: " << path << std::endl;
    std::cout << "savepath: " << savepath << std::endl;
    bool status = file->cd("Event/StcfMCEvent");
    if (!status)
      return -1;
    TTree *theader = (TTree *)gDirectory->Get("header");
    TTree *tMCEvent = (TTree *)gDirectory->Get("StcfMCEvent");

    OSCAR::StcfMCHeader *mcHeader = new OSCAR::StcfMCHeader;
    OSCAR::StcfMCEvent *mcEvent = new OSCAR::StcfMCEvent;

    // theader->SetBranchAddress("header", &mcHeader);
    tMCEvent->SetBranchAddress("StcfMCEvent", &mcEvent);

    int events = tMCEvent->GetEntries();
    cout << "There are total " << events << " events\n\n";

    TFile *driftTimeFile = new TFile(savepath.c_str(), "RECREATE");
    TTree *driftTimeTree = new TTree("tree1", "tree1");
    int eventID, layerID;
    double posX, posY, posZ;
    int trackID, charge;
    double Pt;

    driftTimeTree->Branch("posX", &posX);
    driftTimeTree->Branch("posY", &posY);
    driftTimeTree->Branch("posZ", &posZ);
    driftTimeTree->Branch("eventID", &eventID);
    driftTimeTree->Branch("layerID", &layerID);
    driftTimeTree->Branch("trackID", &trackID);
    driftTimeTree->Branch("Pt", &Pt);
    // driftTimeTree->Branch("charge", &charge);

    for (int ie = 0; ie < events; ie++)
    {
      tMCEvent->GetEntry(ie);
      eventID = mcEvent->eventID();
      TClonesArray *ITDHits = mcEvent->itdHitList();
      int nhits = ITDHits->GetEntries();
      for (int it = 0; it < nhits; it++)
      {
        OSCAR::ITDHit *itdhit = (OSCAR::ITDHit *)(ITDHits->At(it));
        // if(mdchit->parentid()==0&&mdchit->InitialMom().Pt()>150){
        layerID = itdhit->layerid();
        posX = itdhit->Position().x();
        posY = itdhit->Position().y();
        posZ = itdhit->Position().z();
        trackID = itdhit->trackid();
        Pt = itdhit->initialMom().Pt();
        driftTimeFile->cd();
        driftTimeTree->Fill();
        //}
      }
    }

    file->Close();
    driftTimeFile->cd();
    driftTimeTree->Write();
    driftTimeFile->Write();
    driftTimeFile->Close();
  }
  return 0;
}
