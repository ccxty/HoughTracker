#ifndef __BASIC_TREESAVE_CXX_INCLUDE__
#define __BASIC_TREESAVE_CXX_INCLUDE__ 1

#include "TBranch.h"
#include "TFile.h"
#include "TTree.h"
#include "Track.h"

class BasicTreeSave {
 public:
    TFile savefile;
    TTree tree;

    int event_id = -1;
    int track_id = -1;

    double DeltaDXY = -1;
    double pt = 0;
    double pt_true = 0;
    double kz = 0;
    double kz_true = 0;

    double Qxy = 100;
    double Qz = 1;

    double Qe = 0;

    double num_total = 0;
    double num_first_half = 0;
    bool true_track = false;

    explicit BasicTreeSave(const char* filename,
                           const char* tree_name = "tree1");
    void Fill();
    void Write();
    void Close();
};

BasicTreeSave::BasicTreeSave(const char* filename, const char* tree_name)
    : savefile(filename, "RECREATE"), tree(tree_name, tree_name) {
    tree.Branch("eventID", &event_id);
    tree.Branch("trackID", &track_id);
    tree.Branch("DeltaDXY", &DeltaDXY);
    tree.Branch("pt", &pt);
    tree.Branch("pt_true", &pt_true);
    tree.Branch("kz", &kz);
    tree.Branch("kz_true", &kz_true);
    tree.Branch("Qxy", &Qxy);
    tree.Branch("Qz", &Qz);
    tree.Branch("Qe", &Qe);
    tree.Branch("num_total", &num_total);
    tree.Branch("num_first_half", &num_first_half);
    tree.Branch("true_track", &true_track);
}

void BasicTreeSave::Fill() { tree.Fill(); }

void BasicTreeSave::Write() {
    savefile.cd();
    tree.Write();
}

void BasicTreeSave::Close() { savefile.Close(); }
#endif