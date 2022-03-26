#include "DataVis/DataVis.h"
#include "TClonesArray.h"
#include "TH2D.h"
#include "TRandom.h"
#include <TCanvas.h>
#include "TH1.h"
#include "TH2.h"
#include "TMath.h"
#include "TF1.h"
#include "TLegend.h"
#include "TCanvas.h"
#include "TAxis.h"
#include "TEllipse.h"
#include "TROOT.h"
#include "TStyle.h"
#include "TMarker.h"
using namespace std;
using namespace OSCAR;

DECLARE_ALGORITHM(DataVis);

int GetChargeFromPDG(int pdgcode)
{
  if (pdgcode == 11 || pdgcode == 13 || pdgcode == -211 || pdgcode == -2212 || pdgcode == -321)
    return -1;
  if (pdgcode == -11 || pdgcode == -13 || pdgcode == 211 || pdgcode == 2212 || pdgcode == 321)
    return 1;
  return 0;
}

DataVis::DataVis(const std::string &name)
    : AlgBase(name)
{
  declProp("SavePic", m_savepic = false);
  declProp("PicNum", m_picNum = -1);
  declProp("FileName", m_filename = "hits.csv");
  declProp("RunID", m_runID = 1);
}

DataVis::~DataVis()
{
}

bool DataVis::initialize()
{
  savedPicNum = 0;

  SniperPtr<MdcGeomSvc> _svc(getParent(), "MdcGeomSvc");
  if (_svc.valid())
  {
    LogInfo << "the MdcGeomSvc instance is retrieved" << std::endl;
  }
  else
  {
    LogError << "Failed to get the MdcGeomSvc instance!" << std::endl;
    return false;
  }
  m_MdcGeomSvc = _svc.data();

  for (int i = 0; i < 11520; i++)
  {
    cout << m_MdcGeomSvc->Wire(i)->Layer() << "," << i << "," << m_MdcGeomSvc->Wire(i)->BWirePos().X() << "," << m_MdcGeomSvc->Wire(i)->BWirePos().Y() << "," << m_MdcGeomSvc->Wire(i)->BWirePos().Z() << "," << m_MdcGeomSvc->Wire(i)->FWirePos().X() << "," << m_MdcGeomSvc->Wire(i)->FWirePos().Y() << "," << m_MdcGeomSvc->Wire(i)->FWirePos().Z() << endl;
  }

  long seed = time(NULL);
  gRandom->SetSeed(seed);
  cout << "******* seed " << seed << endl;

  m_fileout.open(m_filename, std::ios::app);

  LogDebug << "Sucessfully initialized." << std::endl;
  return true;
}
bool DataVis::execute()
{
  LogDebug << "Executing..." << std::endl;

  EvtDataPtr<StcfRawHeader> Edp(this->getRoot(), "/Event/StcfRawEvent");
  StcfRawHeader *RawEventHeader = Edp.data();
  StcfRawEvent *RawEvent = RawEventHeader->event();

  int EventID = RawEvent->eventID();

  vector<OSCAR::MdcRecHit *> MdcHits = RawEvent->MdcRecHitList();
  int nMdcPoints = MdcHits.size();
  vector<OSCAR::ItdRecHit *> ItdHits = RawEvent->ItdRecHitList();
  int nItdPoints = ItdHits.size();

  int typeFlag = -1;

  vector<OSCAR::MdcRecHit *>::iterator iterhit = MdcHits.begin();
  for (; iterhit != MdcHits.end(); iterhit++)
  {
    MdcRecHit *hit = (*iterhit);
    int type = hit->Type();
    if (type >= 8)
      type = type - 8;
    else
      type = type + 1000;
    if (typeFlag != type && type < 1000)
    {
      m_fileout << -99 << "," << m_runID << "," << EventID << "," << hit->PdgCode() << "," << GetChargeFromPDG(hit->PdgCode()) << "," << hit->InitMomMC().X() << "," << hit->InitMomMC().Y() << "," << hit->InitMomMC().Z() << "," << hit->InitPosMC().X() << "," << hit->InitPosMC().Y() << "," << hit->InitPosMC().Z() << "," << type << endl;
    }
    typeFlag = type;
    m_fileout << m_runID << "," << EventID << "," << hit->LayerId() << "," << hit->CellId() << "," << type << "," << hit->DriftDistanceLeft() << endl;
  }

  if (m_savepic && (savedPicNum < m_picNum || m_picNum == -1))
  {

    TString Ctitile = "Map" + to_string(EventID);

    TCanvas *canvas = new TCanvas("canvas", Ctitile, 1200, 1200);
    Double_t w = gPad->GetWw() * gPad->GetAbsWNDC();
    Double_t h = gPad->GetWh() * gPad->GetAbsHNDC();
    Double_t xmin = -150;
    Double_t xmax = 150;
    // Double_t ymin = -150;
    Double_t ymin = -((xmax - xmin) * h / w) / 2.;
    Double_t ymax = ((xmax - xmin) * h / w) / 2.;
    // Double_t ymax = ((xmax-xmin)*h/w)-150;
    // cout<<w<<"    "<<h<<endl;
    //  cout<<xmin<<"   "<<ymin<<"    "<<xmax<<"    "<<ymax<<endl;
    // Double_t ymax = 150;
    canvas->SetFixedAspectRatio();
    canvas->Range(xmin, ymin, xmax, ymax);

    gStyle->SetOptStat(0);

    TH2F *h2 = new TH2F("h", "HitMap", 5, -850, 850, 5, -850, 850);
    TAxis *xAxis = h2->GetXaxis();
    TAxis *yAxis = h2->GetYaxis();

    h2->SetLabelSize(0.02, "Y");
    h2->SetLabelSize(0.02, "x");
    xAxis->SetTitle("x(mm)");
    yAxis->SetTitle("y(mm)");
    xAxis->SetTitleSize(0.02);
    yAxis->SetTitleSize(0.02);
    // yAxis->SetTitleOffset(1);
    yAxis->CenterTitle(true);
    xAxis->CenterTitle(true);
    // cout<<yAxis->GetTitleOffset()<<endl;

    h2->Draw();

    int ii = 0;
    vector<OSCAR::MdcRecHit *>::iterator iter = MdcHits.begin();
    for (; iter != MdcHits.end(); iter++)
    {
      MdcRecHit *hit = (*iter);
      double x1 = hit->WirePoint1().X();
      double y1 = hit->WirePoint1().Y();
      double x2 = hit->WirePoint2().X();
      double y2 = hit->WirePoint2().Y();

      // TMarker cc(u,v, 0);
      TMarker *cc = new TMarker((x1 + x2) / 2., (y1 + y2) / 2., 0);
      cc->SetMarkerStyle(24);
      cc->SetMarkerSize(0.5);
      cc->SetMarkerColor(kRed);
      if (hit->Type() < 8)
      {
        cc->SetMarkerColor(kBlack);
      }
      // cc->SetMarkerStyle(20);
      // cc->SetDrawOption("Psame");
      cc->Draw();
    }

    TString filename = "HitsMap" + to_string(EventID) + ".pdf";

    canvas->SaveAs(filename);
    delete h2;
    delete canvas;
    savedPicNum++;
  }

  return true;
}
bool DataVis::finalize()
{
  m_fileout.close();
  LogDebug << "Sucessfully finalized." << std::endl;
  return true;
}
