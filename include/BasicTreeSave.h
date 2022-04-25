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
    int num_first_half = 0;
    int num_second_half = 0;
    bool true_track = false;

    bool contain_first_half = false;
    bool contain_second_half = false;

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
    tree.Branch("num_second_half", &num_second_half);
    tree.Branch("true_track", &true_track);
    tree.Branch("contain_first_half", &contain_first_half);
    tree.Branch("contain_second_half", &contain_second_half);
}

void BasicTreeSave::Fill() { tree.Fill(); }

void BasicTreeSave::Write() {
    savefile.cd();
    tree.Write();
}

void BasicTreeSave::Close() { savefile.Close(); }
#endif