#include <TCanvas.h>

#include "TAxis.h"
#include "TCanvas.h"
#include "TClonesArray.h"
#include "TEllipse.h"
#include "TF1.h"
#include "TH1.h"
#include "TH2.h"
#include "TH2D.h"
#include "TLegend.h"
#include "TMarker.h"
#include "TMath.h"
#include "TROOT.h"
#include "TRandom.h"
#include "TStyle.h"

void DataVisual() { TCanvas* canvas1 = new TCanvas("c1", "Map", 500, 500); }