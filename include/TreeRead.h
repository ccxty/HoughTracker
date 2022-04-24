#include "TBranch.h"
#include "TFile.h"
#include "TTree.h"
#ifndef __TREEREAD_CXX_INCLUDE__
#define __TREEREAD_CXX_INCLUDE__ 1

class TreeRead {
 private:
    TFile *_file_source = nullptr;
    TTree *_tree = nullptr;
    int eventID = -1;
    std::vector<double> *_posX = nullptr;
    std::vector<double> *_posY = nullptr;
    std::vector<double> *_posZ = nullptr;
    std::vector<int> *_trackID = nullptr;
    std::vector<int> *_layerID = nullptr;
    std::vector<double> *_pt = nullptr;
    TBranch *_b_posX = nullptr;
    TBranch *_b_posY = nullptr;
    TBranch *_b_posZ = nullptr;
    TBranch *_b_trackID = nullptr;
    TBranch *_b_pt = nullptr;
    TBranch *_b_layerID = nullptr;
    int _nhits = 0;
    void Clear();

 public:
    // default constructor
    TreeRead() = default;
    //  copy constructor
    TreeRead(const TreeRead &) = delete;
    // copy assignment operator
    TreeRead &operator=(const TreeRead &other) = delete;
    // move constructor
    TreeRead(TreeRead &&other) noexcept;
    // move assignment operator
    TreeRead &operator=(TreeRead &&other) noexcept;
    // deconstructor
    ~TreeRead();
    // constructor
    explicit TreeRead(const char *name, const std::string &tree_name = "tree1");
    bool Reset(char *name, const std::string &tree_name = "tree1");
    bool isEmpty() const;

    // getters
    Long64_t GetEntries() const { return _tree->GetEntries(); };
    inline void GetEntry(Long64_t entry) { _tree->GetEntry(entry); }
    inline int EventID() const { return eventID; }
    inline int NHits() const { return _nhits; }
    inline std::vector<double> *PosX() const { return _posX; }
    inline std::vector<double> *PosY() const { return _posY; }
    inline std::vector<double> *PosZ() const { return _posZ; }
    inline std::vector<int> *TrackID() const { return _trackID; }
    inline std::vector<int> *LayerID() const { return _layerID; }
    inline std::vector<double> *Pt() const { return _pt; }
};

TreeRead::TreeRead(TreeRead &&other) noexcept {
    *this = std::move(other);
    other.Clear();
}

TreeRead &TreeRead::operator=(TreeRead &&other) noexcept {
    _file_source = other._file_source;
    _tree = other._tree;
    eventID = other.eventID;
    _posX = other._posX;
    _posY = other._posY;
    _posZ = other._posZ;
    _trackID = other._trackID;
    _layerID = other._layerID;
    _pt = other._pt;
    _b_posX = other._b_posX;
    _b_posY = other._b_posY;
    _b_posZ = other._b_posZ;
    _b_trackID = other._b_trackID;
    _b_pt = other._b_pt;
    _b_layerID = other._b_layerID;
    _nhits = other._nhits;
    other.Clear();
    return *this;
}

TreeRead::~TreeRead() {
    if (_file_source != nullptr) {
        _file_source->Close();
    };
    this->Clear();
};

TreeRead::TreeRead(const char *name, const std::string &tree_name)
    : _file_source(TFile::Open(name)) {
    _tree = dynamic_cast<TTree *>(gDirectory->Get(tree_name.c_str()));
    _tree->SetBranchAddress("posX", &_posX, &_b_posX);
    _tree->SetBranchAddress("posY", &_posY, &_b_posY);
    _tree->SetBranchAddress("posZ", &_posZ, &_b_posZ);
    _tree->SetBranchAddress("eventID", &eventID);
    _tree->SetBranchAddress("layerID", &_layerID, &_b_layerID);
    _tree->SetBranchAddress("trackID", &_trackID, &_b_trackID);
    _tree->SetBranchAddress("Pt", &_pt, &_b_pt);
    _tree->SetBranchAddress("nhits", &_nhits);
};

void TreeRead::Clear() {
    _tree = nullptr;
    eventID = -1;
    _posX = nullptr;
    _posY = nullptr;
    _posZ = nullptr;
    _trackID = nullptr;
    _layerID = nullptr;
    _pt = nullptr;
    _b_posX = nullptr;
    _b_posY = nullptr;
    _b_posZ = nullptr;
    _b_trackID = nullptr;
    _b_pt = nullptr;
    _b_layerID = nullptr;
    _nhits = 0;
}

bool TreeRead::Reset(char *name, const std::string &tree_name) {
    if (_file_source != nullptr) {
        _file_source->Close();
    };
    _file_source = TFile::Open(name);
    _tree = dynamic_cast<TTree *>(gDirectory->Get(tree_name.c_str()));
    _tree->SetBranchAddress("posX", &_posX, &_b_posX);
    _tree->SetBranchAddress("posY", &_posY, &_b_posY);
    _tree->SetBranchAddress("posZ", &_posZ, &_b_posZ);
    _tree->SetBranchAddress("eventID", &eventID);
    _tree->SetBranchAddress("layerID", &_layerID, &_b_layerID);
    _tree->SetBranchAddress("trackID", &_trackID, &_b_trackID);
    _tree->SetBranchAddress("Pt", &_pt, &_b_pt);
    _tree->SetBranchAddress("nhits", &_nhits);
    return true;
}

bool TreeRead::isEmpty() const { return _tree == nullptr; }

#endif