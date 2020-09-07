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

//const float trkLim = 2.0;
//const float ptLim = 50;
//const float ptLim2 = 60;

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

const int llrlim = 100;
const int limshift = 20;
const int llr_bins = (int)4 * llrlim;

char suffix[2][10] = {"", "_pnfs"};

void filenum(const char *dataType, bool pnfs, float ptLim, float trkLim, const char *opt, bool PbPb = true){
    int cent_N = PbPb ? cet_N : 1;
    int JZ_ID[grid_size][2];
    int JZ_ct[grid_size][2];
    int JZ = -1;
    for (int j = 0; j < grid_size; j++)
    {
        for (int jj = 0; jj < sizeof(JZ_ID[j]) / sizeof(int); jj++)
        {
            JZ_ID[j][jj] = 0;
            JZ_ct[j][jj] = 0;
        }
    }

    std::ifstream filej("../GetStuff/JZ_ID.txt");
    std::string linej;
    while (std::getline(filej, linej))
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
                    return;
                }
                while (JZ_ID[itemj[k + 2] - 48][j] > 1000000)
                {
                    j++;
                }
                JZ_ID[itemj[k + 2] - 48][j] = std::stoi(id);
                cout << itemj[k + 2] - 48 << "; " << j << ": " << id << endl;
            }
            ++linePosj;
        }
    }

    std::ifstream filein(Form("../GetStuff/%s_root%s.txt", dataType, suffix[pnfs]));
    std::ofstream outF(Form("../GetStuff/%s_root%s_filenum.txt", dataType, suffix[pnfs]));
    std::string filename;

    while (std::getline(filein, filename))
    {
        int k = filename.rfind("Akt4HIJets");
        if (k == std::string::npos)
        {
            cout << "Wrong name: " << filename << endl;
            return;
        }
        bool found = false;

        for (int j = 0; j < grid_size; j++)
        {
            for (int jj = 0; jj < sizeof(JZ_ID[j]) / sizeof(int); jj++)
            {
                if (std::stoi(filename.substr(k - 9, 8)) == JZ_ID[j][jj])
                {
                    found = true;
                    JZ = j;
                    tag = jj;
                }
            }
        }

        if (!found)
        {
            cout << filename << endl;
            cout << "not found" << endl;
            return;
        }

        NUM = std::stoi(filename.substr(filename.length() - 11, 6));
    }
}

float get_weight(const char* dataType, const char* filename, bool pnfs = true, float mcwg = 1.0){
    std::ifstream filein(Form("../GetStuff/%s_evtnb%s.txt", dataType, suffix[pnfs]));
}