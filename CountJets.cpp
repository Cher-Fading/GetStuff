#include <fstream>
#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <dirent.h>
#include "TFile.h"
#include "TChain.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TApplication.h"
#include "TH2D.h"
#include "TProfile.h"
#include "TSystem.h"
#include <algorithm>
#include "TStyle.h"
#include "TColor.h"
#include "TProfile.h"
#include "TMath.h"
#include "TLorentzVector.h"
#include "TString.h"
#include "TGraph.h"
#include "TGraphAsymmErrors.h"
#include <math.h>

#include <TH2.h>
#include <TStyle.h>

const int myColor[] = {kBlue, kViolet, kMagenta, kPink, kOrange, kYellow, kSpring, kTeal, kCyan, kAzure, kGray, kGray + 1, kGray + 3};
const float eta_cut = 2.1;
const float eta_cut2 = 2.4;
const float JZs = 5;
const float s50kWeight[] = {6.7890E+07 * 6.1692E-06, 6.3996E+05 * 5.8420E-05, 4.7192E+03 * 1.1270E-04, 9.2038E-05 * 2.6602E+01};
const int s50k_size = sizeof(s50kWeight) / sizeof(float);
const float pt_max = 2000.;
const float Weight[] = {6.7890E+07, 6.789E+07, 6.3996E+05, 4.7195E+03, 2.6602E+01, 2.2476E-01};
const float Filter[] = {9.9713E-01, 2.8748E-03, 4.2952E-03, 5.2994E-03, 4.5901E-03, 2.1846E-03};
const int grid_size = sizeof(Weight) / sizeof(float);
char legend1[3][50] = {"b-Jets", "c-jet", "light jet"};
char leg[3][10] = {"B", "C", "U"};
const float FCal_range[] = {0, 0.063719, 0.14414, 0.289595, 0.525092, 0.87541, 1.36875, 2.04651, 2.98931, 5}; // fcal_cuts options

const int cet[] = {0, 2, 2, 5, 5, 8}; //selected centrality sections
const int cet_N = (sizeof(cet) / sizeof(int)) / 2;

const int llr_bins = 200;
const int llrlim = 40;

char suffix[2][10] = {"", "_pnfs"};
char Type[2][10] = {"pp", "PbPb"};
const int indexF[nFlav] = {0, 5, 4};
const int nFlav = 3;
char indexF_str[nFlav][10] = {"Light", "B", "C"};
const int nType = 2;
char type_str[nType][10] = {"Original","Passed"};

//stat for b and light are the same, ratio accounts for additional bad jets expected based on previous small samples.
//stat for c are some ratio to b/light.
void initBranches(TChain *fChain)
{
    fChain->SetBranchStatus("*", 1);
    //fChain->SetBranchStatus("eventnb", 1);
    //fChain->SetBranchStatus("mcwg", 1);
    fChain->SetBranchStatus("njets", 1);
    //fChain->SetBranchStatus("Fcal", 1);

    fChain->SetBranchStatus("jet_pt", 1);
    fChain->SetBranchStatus("jet_eta", 1);
    fChain->SetBranchStatus("jet_LabDr_HadF", 1);
    fChain->SetBranchStatus("jet_aliveAfterOR", 1);
    fChain->SetBranchStatus("jet_nConst", 1);
    fChain->SetBranchStatus("jet_truthMatch", 1);
}

void CountJets(const char *trainname, const char *filename, const char *outputFolder, const char *outputName, bool PbPb = true, bool pnfs = true, bool inclusive = true)
{
    std::ifstream fstat(Form("../GetStuff/%s_stat.txt",trainname);
    std::string line;
    int stat = 0;
    int cStat = 0;
    int outStat = 0;
    int outcStat = 0;
    float ptLim = 50.;
    float aeta = 2.1;
    while (getline(fstat,line)){
        TString Line = line.data();
        if (Line.Contains("stat"))
        {
            TString stat_str = line.substr(4, line.length());
            stat = stat_str.ReplaceAll("k", "000").Atoi();
            outStat = stat;
            cStat = stat * 0.5;
            outcStat = cStat;
        }
        if (Line.Contains("cstat"))
        {
            TString cstat_str = line.substr(5, line.length());
            cStat = cstat_str.ReplaceAll("k", "000").Atoi();
            outcStat = cStat;
        }
        if (Line.Contains("badMargin"))
        {
            float ratio = 1 + (line.substr(9, line.length()) / 100.);
            outStat = ratio * stat;
            outcStat = ratio * cStat;
        }
        if (Line.Contains("pt"))
        {
            float ptLim = std::stof(line.substr(2, line.length()));
        }
        if (Line.Contains("aeta"))
        {
            float aeta = std::stof(line.substr(4, line.length())) / 10.;
        }
    }

    if (inclusive){
        int gridsize = inclusive ? grid_size : s50k_size;

        int JZ_ID[gridsize][2];
        for (int j = 0; j < gridsize; j++)
        {
            for (int jj = 0; jj < sizeof(JZ_ID[j]) / sizeof(int); jj++)
            {

                JZ_ID[j][jj] = 0;
            }
        }
        std::ifstream fJZ_ID("../GetStuff/JZ_ID.txt");
        std::string linej;
        while (std::getline(fJZ_ID, linej))
        {
            std::stringstream linestreamj(linej);
            std::string itemj;
            int linePosj = 0;
            std::string id;
            //cout << linej << endl;
            while (std::getline(linestreamj, itemj, ' '))
            {
                if (itemj == "")
                    continue;

                if (linePosj == 0)
                {
                    id = itemj;
                }
                if (linePosj == 4)
                {
                    if (itemj.find(dataType) == std::string::npos)
                    {
                        //cout << "Wrong file name" << itemj << endl;
                        continue;
                    }
                    int k = itemj.find("JZ");
                    //cout << itemj << endl;
                    int j = 0;
                    if (k == std::string::npos)
                    {
                        cout << "Wrong name" << itemj << endl;
                        return -1;
                    }
                    while (JZ_ID[itemj[k + 2] - 48][j] > 1000000)
                    {
                        j++;
                    }
                    JZ_ID[itemj[k + 2] - 48][j] = std::stoi(id);
                    //cout << itemj[k + 2] - 48 << "; " << j << ": " << id << endl;
                }
                ++linePosj;
            }
        }

        int k = filename.rfind("Akt4HIJets");
        if (k == std::string::npos)
        {
            cout << "Wrong name: " << filename << endl;
            return -1;
        }
        bool found = false;

        for (int j = 0; j < gridsize; j++)
        {
            for (int jj = 0; jj < sizeof(JZ_ID[j]) / sizeof(int); jj++)
            {
                if (std::stoi(filename.substr(k - 9, 8)) == JZ_ID[j][jj])
                {
                    found = true;
                    JZ = j;
                    tag == jj;
                }
            }
        }

        if (!found)
        {
            cout << filename << endl;
            cout << "not found" << endl;
            return -1;
        }

        NUM = std::stoi(filename.substr(filename.length() - 11, 6));
    }
    else {
        JZ = std::stoi(filename[filename.find("JZ") + 2]);
        tag = 0;
        NUM = 0;
    }

    std::ofstream fstatout(Form("/atlasgpfs01/usatlas/data/cher97/%s%s_Counts/%s_%d_%d_%d_counts.txt",dataType,Type[PbPb],trainname,JZ,tag,NUM));

    TChain* fChain = new TChain(chain_name.c_str());
    initBranches(fChain);

    //Int_t eventnb;
    //Float_t mcwg;
    //Float_t Fcal;
    Int_t njets;
    std::vector<float> *jet_pt = 0;     //MV2
    std::vector<float> *jet_eta;        //MV2
    std::vector<int> *jet_LabDr_HadF;   //label
    //std::vector<float> *jet_nConst;     //cuts not used (nConst > 1)
    std::vector<int> *jet_truthMatch;   //cuts (truthMatch == 1)
    std::vector<int> *jet_aliveAfterOR; //cuts (aliveAfterOR == 1) (no overlap with electron)

    //TBranch *b_eventnb;
    //TBranch *b_mcwg;
    //TBranch *b_Fcal;
    TBranch *b_njets;
    TBranch *b_jet_pt;
    TBranch *b_jet_eta;
    TBranch *b_jet_LabDr_HadF;   //label
    //TBranch *b_jet_nConst;       //cuts not used (nConst > 1)
    TBranch *b_jet_truthMatch;   //cuts (truthMatch == 1)
    TBranch *b_jet_aliveAfterOR; //cuts (aliveAfterOR == 1) (no overlap with electron)

        //fChain->SetBranchAddress("eventnb", &eventnb, &b_eventnb);
    //fChain->SetBranchAddress("mcwg", &mcwg, &b_mcwg);
    //fChain->SetBranchStatus("mcwg", 1);
    
    fChain->SetBranchAddress("njets", &njets, &b_njets);
    //fChain->SetBranchAddress("Fcal", &Fcal, &b_Fcal);

    fChain->SetBranchAddress("jet_pt", &jet_pt, &b_jet_pt);
    fChain->SetBranchAddress("jet_eta", &jet_eta, &b_jet_eta);
    fChain->SetBranchAddress("jet_LabDr_HadF", &jet_LabDr_HadF, &b_jet_LabDr_HadF);
    fChain->SetBranchAddress("jet_aliveAfterOR", &jet_aliveAfterOR, &b_jet_aliveAfterOR);
    //fChain->SetBranchAddress("jet_nConst", &jet_nConst, &b_jet_nConst);
    fChain->SetBranchAddress("jet_truthMatch", &jet_truthMatch, &b_jet_truthMatch);

    Long64_t nentries = fChain->GetEntries();
    int n[nFlav][nType];
    for (int f = 0; n < nFlav; n++){
        for (int t = 0; t < nType; t++)
        {
            n[f][t] = 0;
        }
    }
    int nOrder = 0;
    for (int i = 0; i < nFlav; i++){
        nOrder = nOrder >= indexF[i] ? nOrder : indexF[i];
    }
    int order[nOrder + 1];
    for (int o = 0; o < nOrder + 1; o++){
        for (int i = 0; i < nFlav; i++){
            if (o == indexF[i]) order[o] = i;
            else order[o] = -1;
        }
    }

    for (Long64_t jentry = 0; jentry < nentries; jentry++)
    {
        b_njets->GetEntry(jentry);
        b_jet_pt->GetEntry(jentry);
        b_jet_eta->GetEntry(jentry);
        b_jet_LabDr_HadF->GetEntry(jentry); //label
        b_jet_truthMatch->GetEntry(jentry); //cuts (truthMatch == 1)
        b_jet_aliveAfterOR->GetEntry(jentry);
        for (int j = 0; j < njets; j++)
        {
            int flav = jet_LabDr_HadF->at(j);
            if (flav != 0 && flav != 4 && flav != 5)
                continue;
            int passed = jet_pt->at(j) > (ptLim * 1e3) && fabs(jet_eta->at(j)) <= 2.1 && jet_aliveAfterOR->at(j) == 1 && jet_truthMatch->at(j) == 1;
            n[order[flav]][passed]++;
        }
    }

    for (int f = 0; f < nFlav; f++){
        for (int i = 0; i < nType; i++){
            cout << type_str[i] << " for " << indexF_str[f] << " jet is " << n[f][i] << endl;
            fstatout << indexF_str[f] << " " << type_str[i] << " " << n[f][i] << endl;
        }
    }

    fstatout->close();
}